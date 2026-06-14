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
}

namespace Sven::Internal {

void init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error(
            std::string("Sven: SDL_Init failed: ") + SDL_GetError()
        );
    }

    // Open the default audio device with SDL3_mixer's recommended
    // defaults (nullptr spec = let SDL_mixer choose a sensible format).
    if (!g_audioReady) {
        if (Mix_OpenAudio(0, nullptr)) {
            g_audioReady = true;
        } else {
            std::fprintf(stderr,
                "Sven: Could not initialise audio (Mix_OpenAudio failed: %s). "
                "Sounds and music will be silently disabled.\n",
                SDL_GetError());
            g_audioReady = false;
        }
    }
}

void shutdown() {
    if (g_audioReady) {
        Mix_CloseAudio();
        g_audioReady = false;
    }
    SDL_Quit();
}

bool isAudioReady() {
    return g_audioReady;
}

} // namespace Sven::Internal
