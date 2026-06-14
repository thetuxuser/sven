/**
 * examples/sprite.cpp — Sprites & Textures
 *
 * Demonstrates the new v0.3 features:
 *   - Sven::Texture            loading an image file
 *   - Window::drawTexture()    drawing a sprite
 *   - Sven::Rect                cropping a region (sprite-sheet style)
 *
 * ...combined with v0.2's Vec2 + delta time for smooth movement.
 *
 * The image used here (assets/player.png) is a tiny 32x32 placeholder
 * sprite included with the repo — swap in your own artwork!
 *
 * Build with CMake (from the repo root):
 *   cmake -B build && cmake --build build
 *   ./build/examples/sprite
 */

#include <sven.h>

int main() {
    Sven::Window game("Sven — Sprites", 800, 600);

    // Load the sprite. A Window must exist first (see above) so Sven has
    // somewhere to upload the image data.
    Sven::Texture player("examples/assets/player.png");

    if (!player.isValid()) {
        // isValid() lets you handle a missing file gracefully instead of
        // the game crashing. Here we just keep running without drawing it.
        // (An error message was already printed to stderr.)
    }

    // Position and movement, just like the v0.2 movement example.
    Sven::Vec2 position(368.0f, 268.0f);
    const float speed = 250.0f; // pixels per second

    while (game.running()) {
        game.pollEvents();
        float dt = game.getDeltaTime();

        // Move with arrow keys.
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_RIGHT)) position.x += speed * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_LEFT))  position.x -= speed * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_DOWN))  position.y += speed * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_UP))    position.y -= speed * dt;

        game.clear(Sven::Color(25, 25, 35));

        // ── Draw the sprite at its native size ────────────────────────────
        game.drawTexture(player, position);

        // ── Draw just the top-left 16x16 corner of the same image,
        //    to show off the cropping (srcRect) overload. This is the
        //    same mechanism used to pick frames out of a sprite sheet.
        Sven::Rect topLeftQuarter = {0, 0, 16, 16};
        game.drawTexture(player, Sven::Vec2(20.0f, 20.0f), topLeftQuarter);

        game.present();
    }

    return 0;
}
