/**
 * audio.cpp — Sven Audio Implementation
 *
 * Implements Sven::Sound (short effects) and Sven::Music
 * (streaming background music) via SDL3_mixer 3.0.
 *
 * If Sven's audio system failed to initialise (Internal::isAudioReady()
 * returns false), both classes simply act as if every file failed to
 * load: isValid() is false and all playback functions are no-ops.
 */

#include "sven_internal.h"

#include <cstdio>
#include <algorithm> // std::clamp

namespace {

/** Clamp a gain (volume) to the valid [0, inf) range. */
float clampGain(float v) {
    return std::max(0.0f, v);
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
        return;
    }

    // SDL3_mixer 3.x: MIX_LoadAudio requires a mixer, a path, and a predecode flag.
    m_impl->audio = MIX_LoadAudio(Internal::getMixer(), path.c_str(), false);

    if (!m_impl->audio) {
        std::fprintf(stderr,
            "Sven: Failed to load sound \"%s\": %s\n",
            path.c_str(), SDL_GetError());
        return;
    }

    // Apply initial volume.
    MIX_SetAudioGain(m_impl->audio, m_impl->volume);
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

    // SDL3_mixer 3.x: MIX_PlayAudio is the "fire-and-forget" API.
    MIX_PlayAudio(Internal::getMixer(), m_impl->audio);
}

void Sound::setVolume(float volume) {
    if (!m_impl) return;
    m_impl->volume = clampGain(volume);
    if (m_impl->audio) {
        MIX_SetAudioGain(m_impl->audio, m_impl->volume);
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
            if (m_impl->audio) MIX_DestroyAudio(m_impl->audio);
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

    MIX_Track* track = Internal::getMusicTrack();
    if (!track) return;

    // Assign this audio to the dedicated music track and start playback.
    MIX_SetTrackAudio(track, m_impl->audio);
    // SDL3_mixer 3.x loop count: -1 = infinite, 0 = once.
    MIX_PlayTrack(track, loop ? MIX_DURATION_INFINITE : 0, 0);
}

void Music::pause() {
    MIX_Track* track = Internal::getMusicTrack();
    if (track) MIX_PauseTrack(track);
}

void Music::resume() {
    MIX_Track* track = Internal::getMusicTrack();
    if (track) MIX_ResumeTrack(track);
}

void Music::stop() {
    MIX_Track* track = Internal::getMusicTrack();
    if (track) MIX_HaltTrack(track);
}

bool Music::isPlaying() const {
    MIX_Track* track = Internal::getMusicTrack();
    if (!track || !isValid()) return false;

    // In SDL3_mixer, a track is considered "playing" if its assigned
    // audio is what we expect.
    return MIX_GetTrackAudio(track) == m_impl->audio;
}

void Music::setVolume(float volume) {
    MIX_Track* track = Internal::getMusicTrack();
    if (track) MIX_SetTrackGain(track, clampGain(volume));
}

float Music::getVolume() const {
    MIX_Track* track = Internal::getMusicTrack();
    return track ? MIX_GetTrackGain(track) : 0.0f;
}

} // namespace Sven

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers — provided for internal access to audio objects.
// ─────────────────────────────────────────────────────────────────────────────

namespace Sven::Internal {

MIX_Audio* SoundAccess::get(const Sound& sound) {
    return sound.m_impl ? sound.m_impl->audio : nullptr;
}

MIX_Audio* MusicAccess::get(const Music& music) {
    return music.m_impl ? music.m_impl->audio : nullptr;
}

} // namespace Sven::Internal
