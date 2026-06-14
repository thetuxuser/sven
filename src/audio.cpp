/**
 * audio.cpp — Sven Audio Implementation
 *
 * Implements Sven::Sound (short effects, via Mix_Chunk) and Sven::Music
 * (streaming background music, via Mix_Music). SDL3_mixer types never
 * appear in the public header.
 *
 * If Sven's audio system failed to initialise (Internal::isAudioReady()
 * returns false), both classes simply act as if every file failed to
 * load: isValid() is false and all playback functions are no-ops.
 */

#include "sven_internal.h"

#include <cstdio>
#include <algorithm> // std::clamp

namespace {

/** Clamp a volume to the valid [0, 1] range. */
float clampVolume(float v) {
    return std::clamp(v, 0.0f, 1.0f);
}

/** Convert a 0.0–1.0 volume to SDL_mixer's 0–MIX_MAX_VOLUME integer range. */
int volumeToMix(float v) {
    return static_cast<int>(clampVolume(v) * MIX_MAX_VOLUME);
}

/** Convert an SDL_mixer 0–MIX_MAX_VOLUME integer back to 0.0–1.0. */
float mixToVolume(int mixVolume) {
    return static_cast<float>(mixVolume) / static_cast<float>(MIX_MAX_VOLUME);
}

} // namespace

namespace Sven {

// ════════════════════════════════════════════════════════════════════════════
// Sound
// ════════════════════════════════════════════════════════════════════════════

struct Sound::Impl {
    Mix_Chunk* chunk  = nullptr;
    float      volume = 1.0f;
};

Sound::Sound(const std::string& path)
    : m_impl(new Impl())
{
    if (!Internal::isAudioReady()) {
        // Audio system unavailable — stay invalid, but don't print an
        // error for every sound; core.cpp already warned once.
        return;
    }

    m_impl->chunk = Mix_LoadWAV(path.c_str());

    if (!m_impl->chunk) {
        std::fprintf(stderr,
            "Sven: Failed to load sound \"%s\": %s\n",
            path.c_str(), SDL_GetError());
        return;
    }

    // Apply the default volume to the freshly-loaded chunk.
    Mix_VolumeChunk(m_impl->chunk, volumeToMix(m_impl->volume));
}

Sound::~Sound() {
    if (m_impl) {
        if (m_impl->chunk) {
            Mix_FreeChunk(m_impl->chunk);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

Sound::Sound(Sound&& other) noexcept
    : m_impl(other.m_impl)
{
    other.m_impl = nullptr;
}

Sound& Sound::operator=(Sound&& other) noexcept {
    if (this != &other) {
        if (m_impl) {
            if (m_impl->chunk) Mix_FreeChunk(m_impl->chunk);
            delete m_impl;
        }
        m_impl = other.m_impl;
        other.m_impl = nullptr;
    }
    return *this;
}

bool Sound::isValid() const {
    return m_impl && m_impl->chunk != nullptr;
}

void Sound::play() const {
    if (!isValid()) return;

    // -1 = first free channel, 0 = don't loop (play once).
    Mix_PlayChannel(-1, m_impl->chunk, 0);
}

void Sound::setVolume(float volume) {
    if (!m_impl) return;

    m_impl->volume = clampVolume(volume);

    if (m_impl->chunk) {
        Mix_VolumeChunk(m_impl->chunk, volumeToMix(m_impl->volume));
    }
}

float Sound::getVolume() const {
    return m_impl ? m_impl->volume : 0.0f;
}

// ════════════════════════════════════════════════════════════════════════════
// Music
// ════════════════════════════════════════════════════════════════════════════

struct Music::Impl {
    Mix_Music* music = nullptr;
};

Music::Music(const std::string& path)
    : m_impl(new Impl())
{
    if (!Internal::isAudioReady()) {
        return;
    }

    m_impl->music = Mix_LoadMUS(path.c_str());

    if (!m_impl->music) {
        std::fprintf(stderr,
            "Sven: Failed to load music \"%s\": %s\n",
            path.c_str(), SDL_GetError());
        return;
    }
}

Music::~Music() {
    if (m_impl) {
        if (m_impl->music) {
            // SDL_mixer only tracks one "current" music track globally and
            // doesn't expose which Mix_Music* that is. To avoid freeing a
            // track while it's still playing, halt playback unconditionally
            // before freeing — this is harmless if a different track (or
            // nothing) was playing.
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
            Mix_FreeMusic(m_impl->music);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

Music::Music(Music&& other) noexcept
    : m_impl(other.m_impl)
{
    other.m_impl = nullptr;
}

Music& Music::operator=(Music&& other) noexcept {
    if (this != &other) {
        if (m_impl) {
            if (m_impl->music) {
                if (Mix_PlayingMusic()) Mix_HaltMusic();
                Mix_FreeMusic(m_impl->music);
            }
            delete m_impl;
        }
        m_impl = other.m_impl;
        other.m_impl = nullptr;
    }
    return *this;
}

bool Music::isValid() const {
    return m_impl && m_impl->music != nullptr;
}

void Music::play(bool loop) {
    if (!isValid()) return;

    // SDL_mixer loop count: -1 means "loop forever", 0 means "play once".
    Mix_PlayMusic(m_impl->music, loop ? -1 : 0);
}

void Music::pause() {
    if (!isValid()) return;
    Mix_PauseMusic();
}

void Music::resume() {
    if (!isValid()) return;
    Mix_ResumeMusic();
}

void Music::stop() {
    if (!isValid()) return;
    Mix_HaltMusic();
}

bool Music::isPlaying() const {
    if (!isValid()) return false;
    return Mix_PlayingMusic() != 0 && Mix_PausedMusic() == 0;
}

void Music::setVolume(float volume) {
    if (!isValid()) return;

    // SDL_mixer's music volume is global (shared across all Music
    // objects), but exposing it here keeps the API simple and obvious.
    Mix_VolumeMusic(volumeToMix(volume));
}

float Music::getVolume() const {
    if (!Internal::isAudioReady()) return 0.0f;

    // Passing -1 queries the current global music volume without
    // changing it.
    return mixToVolume(Mix_VolumeMusic(-1));
}

} // namespace Sven

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers — currently unused outside this file, but provided for
// consistency with TextureAccess and potential future use (e.g. checking
// which Mix_Music is currently playing).
// ─────────────────────────────────────────────────────────────────────────────

namespace Sven::Internal {

Mix_Chunk* SoundAccess::get(const Sound& sound) {
    return sound.m_impl ? sound.m_impl->chunk : nullptr;
}

Mix_Music* MusicAccess::get(const Music& music) {
    return music.m_impl ? music.m_impl->music : nullptr;
}

} // namespace Sven::Internal
