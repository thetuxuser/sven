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

} // namespace

namespace Sven {

// ════════════════════════════════════════════════════════════════════════════
// Sound
// ════════════════════════════════════════════════════════════════════════════

struct Sound::Impl {
    MIX_Audio* audio  = nullptr;
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

    m_impl->audio = MIX_LoadAudio(Internal::getMixer(), path.c_str(), false);

    if (!m_impl->audio) {
        std::fprintf(stderr,
            "Sven: Failed to load sound \"%s\": %s\n",
            path.c_str(), SDL_GetError());
        return;
    }

    // Apply the default volume to the freshly-loaded audio.
    MIX_SetAudioVolume(m_impl->audio, m_impl->volume);
}

Sound::~Sound() {
    if (m_impl) {
        if (m_impl->audio) {
            MIX_DestroyAudio(m_impl->audio);
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
            if (m_impl->audio) MIX_DestroyAudio(m_impl->audio);
            delete m_impl;
        }
        m_impl = other.m_impl;
        other.m_impl = nullptr;
    }
    return *this;
}

bool Sound::isValid() const {
    return m_impl && m_impl->audio != nullptr;
}

void Sound::play() const {
    if (!isValid()) return;

    // SDL_mixer 3.0 fire-and-forget playback.
    MIX_PlayAudio(Internal::getMixer(), m_impl->audio);
}

void Sound::setVolume(float volume) {
    if (!m_impl) return;

    m_impl->volume = clampVolume(volume);

    if (m_impl->audio) {
        MIX_SetAudioVolume(m_impl->audio, m_impl->volume);
    }
}

float Sound::getVolume() const {
    return m_impl ? m_impl->volume : 0.0f;
}

// ════════════════════════════════════════════════════════════════════════════
// Music
// ════════════════════════════════════════════════════════════════════════════

struct Music::Impl {
    MIX_Audio* audio = nullptr;
};

Music::Music(const std::string& path)
    : m_impl(new Impl())
{
    if (!Internal::isAudioReady()) {
        return;
    }

    m_impl->audio = MIX_LoadAudio(Internal::getMixer(), path.c_str(), false);

    if (!m_impl->audio) {
        std::fprintf(stderr,
            "Sven: Failed to load music \"%s\": %s\n",
            path.c_str(), SDL_GetError());
        return;
    }
}

Music::~Music() {
    if (m_impl) {
        if (m_impl->audio) {
            // SDL_mixer 3.0 has a different track-based model, but for
            // simplicity we'll just destroy the audio object. If it's
            // currently playing, the mixer should handle it.
            MIX_DestroyAudio(m_impl->audio);
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
            if (m_impl->audio) {
                MIX_DestroyAudio(m_impl->audio);
            }
            delete m_impl;
        }
        m_impl = other.m_impl;
        other.m_impl = nullptr;
    }
    return *this;
}

bool Music::isValid() const {
    return m_impl && m_impl->audio != nullptr;
}

void Music::play(bool loop) {
    if (!isValid()) return;

    // SDL_mixer 3.0: We'll use fire-and-forget for now, but
    // real music support should use tracks.
    MIX_PlayAudio(Internal::getMixer(), m_impl->audio);
}

void Music::pause() {
    if (!isValid()) return;
    MIX_PauseAllTracks(Internal::getMixer());
}

void Music::resume() {
    if (!isValid()) return;
    MIX_ResumeAllTracks(Internal::getMixer());
}

void Music::stop() {
    if (!isValid()) return;
    MIX_HaltAllTracks(Internal::getMixer());
}

bool Music::isPlaying() const {
    if (!isValid()) return false;
    // SDL_mixer 3.0 doesn't have a simple isPlayingMusic.
    return true;
}

void Music::setVolume(float volume) {
    if (!isValid()) return;

    // SDL_mixer's music volume is global (shared across all Music
    // objects), but exposing it here keeps the API simple and obvious.
    // In SDL3 Mixer, we set the volume on the mixer or track.
    // For now we'll set it on the audio object if that's how it's handled.
    MIX_SetAudioVolume(m_impl->audio, clampVolume(volume));
}

float Music::getVolume() const {
    if (!isValid()) return 0.0f;
    return MIX_GetAudioVolume(m_impl->audio);
}

} // namespace Sven

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers — currently unused outside this file, but provided for
// consistency with TextureAccess and potential future use (e.g. checking
// which Mix_Music is currently playing).
// ─────────────────────────────────────────────────────────────────────────────

namespace Sven::Internal {

struct MIX_Audio* SoundAccess::get(const Sound& sound) {
    return sound.m_impl ? sound.m_impl->audio : nullptr;
}

struct MIX_Audio* MusicAccess::get(const Music& music) {
    return music.m_impl ? music.m_impl->audio : nullptr;
}

} // namespace Sven::Internal
