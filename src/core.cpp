/**
 * core.cpp — Sven Library Core
 *
 * Handles library initialisation and shutdown, including SDL3 itself and
 * the optional SDL3_mixer audio subsystem.
 *
 * This is internal — users never call these functions directly.
 */

#include "sven_internal.h"

#include <stdexcept>
#include <cstdio>

namespace {
    // Tracks whether SDL3_mixer was successfully opened. If audio
    // initialisation fails (e.g. no sound device available), Sven keeps
    // running silently rather than crashing — Sound/Music just become
    // "invalid" and play() becomes a no-op.
    bool g_audioReady = false;
    MIX_Mixer* g_mixer = nullptr;
    MIX_Track* g_musicTrack = nullptr;
}

namespace Sven::Internal {

void init() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
        throw std::runtime_error(
            std::string("Sven: SDL_Init failed: ") + SDL_GetError()
        );
    }

    // Open the default audio device with SDL3_mixer's recommended
    // defaults.
    if (!g_audioReady) {
        g_mixer = MIX_CreateMixerDevice(0);
        if (g_mixer) {
            // Create a dedicated track for music so we can control it
            // independently of fire-and-forget sounds.
            g_musicTrack = MIX_CreateTrack(g_mixer);
            if (g_musicTrack) {
                g_audioReady = true;
            } else {
                std::fprintf(stderr,
                    "Sven: Could not create music track: %s\n",
                    SDL_GetError());
                MIX_DestroyMixer(g_mixer);
                g_mixer = nullptr;
                g_audioReady = false;
            }
        } else {
            std::fprintf(stderr,
                "Sven: Could not initialise audio (MIX_CreateMixerDevice failed: %s). "
                "Sounds and music will be silently disabled.\n",
                SDL_GetError());
            g_audioReady = false;
        }
    }
}

void shutdown() {
    if (g_audioReady) {
        if (g_musicTrack) MIX_DestroyTrack(g_musicTrack);
        MIX_DestroyMixer(g_mixer);
        g_musicTrack = nullptr;
        g_mixer = nullptr;
        g_audioReady = false;
    }
    SDL_Quit();
}

MIX_Mixer* getMixer() {
    return g_mixer;
}

MIX_Track* getMusicTrack() {
    return g_musicTrack;
}

bool isAudioReady() {
    return g_audioReady;
}

} // namespace Sven::Internal
