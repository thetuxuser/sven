#pragma once

/**
 * sven_internal.h — Internal Sven Header
 *
 * This header is ONLY used by Sven's own .cpp files.
 * It is never installed alongside sven.h and is invisible to users.
 *
 * It gives Sven's implementation access to SDL3 while keeping SDL3
 * completely hidden from anyone who just includes <sven.h>.
 */

// Pull in SDL3, SDL3_image, and SDL3_mixer. The CMake build adds the
// include paths.
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <sven.h>   // public API (for the types we're implementing)

namespace Sven::Internal {

/** Initialise SDL3. Called once by the first Window constructor. */
void init();

/** Tear down SDL3. Called by the Window destructor. */
void shutdown();

/**
 * Friend bridge that lets Sven's own .cpp files reach the SDL3 renderer
 * stored inside Window::Impl, without exposing it in the public header.
 */
struct RendererAccess {
    static SDL_Renderer* get(Window& window);
};

/**
 * Returns the renderer belonging to the currently-active Window, or
 * nullptr if no Window exists yet.
 *
 * Sven supports a single window, so Texture (which has no Window&
 * available in its constructor) uses this to find the renderer it
 * needs to upload pixel data to.
 */
SDL_Renderer* getActiveRenderer();

/**
 * Returns true if Sven's audio system (SDL3_mixer) is ready to use.
 *
 * Sound and Music check this before loading anything, so a failed audio
 * device doesn't crash the game — sounds simply become "invalid" and
 * play() becomes a no-op.
 */
bool isAudioReady();

/**
 * Friend bridge that lets graphics.cpp reach the SDL3 texture stored
 * inside Texture::Impl, without exposing it in the public header.
 */
struct TextureAccess {
    static SDL_Texture* get(const Texture& texture);
};

/**
 * Friend bridge for Sound's internal Mix_Chunk, used only within audio.cpp.
 */
struct SoundAccess {
    static Mix_Chunk* get(const Sound& sound);
};

/**
 * Friend bridge for Music's internal Mix_Music, used only within audio.cpp.
 */
struct MusicAccess {
    static Mix_Music* get(const Music& music);
};

} // namespace Sven::Internal
