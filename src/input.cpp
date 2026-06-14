/**
 * input.cpp — Sven Keyboard Input
 *
 * Wraps SDL3's keyboard state so users only ever see Sven::Keyboard.
 * Tracks both "currently held" (isKeyDown) and "just pressed" (isKeyPressed)
 * states.
 */

#include "sven_internal.h"

#include <array>
#include <cstring>  // std::memcpy

namespace Sven {

// ─── Internal state ───────────────────────────────────────────────────────────

namespace {
    // SDL3 has 512 possible scancodes.
    constexpr int NUM_KEYS = 512;

    // Snapshot from the previous frame — used to detect "just pressed".
    static bool s_prevState[NUM_KEYS] = {};
}

// ─── Keyboard::_update ────────────────────────────────────────────────────────
// Called by Window::pollEvents() before SDL_PollEvent processes new events.
// This saves the keyboard state *before* it changes, giving us "last frame".

void Keyboard::_update() {
    int numKeys = 0;
    const bool* current = SDL_GetKeyboardState(&numKeys);

    int count = (numKeys < NUM_KEYS) ? numKeys : NUM_KEYS;
    for (int i = 0; i < count; ++i) {
        s_prevState[i] = current[i];
    }
}

// ─── Keyboard::isKeyDown ──────────────────────────────────────────────────────
// Returns true every frame while the key is held.

bool Keyboard::isKeyDown(int scancode) {
    if (scancode < 0 || scancode >= NUM_KEYS) return false;

    int numKeys = 0;
    const bool* state = SDL_GetKeyboardState(&numKeys);

    if (scancode >= numKeys) return false;
    return state[scancode];
}

// ─── Keyboard::isKeyPressed ───────────────────────────────────────────────────
// Returns true only on the frame the key transitions from up → down.

bool Keyboard::isKeyPressed(int scancode) {
    if (scancode < 0 || scancode >= NUM_KEYS) return false;

    int numKeys = 0;
    const bool* state = SDL_GetKeyboardState(&numKeys);

    if (scancode >= numKeys) return false;

    // "Was up last frame AND is down this frame" = just pressed.
    return !s_prevState[scancode] && state[scancode];
}

} // namespace Sven
