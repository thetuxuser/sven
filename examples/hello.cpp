/**
 * examples/hello.cpp — Hello Sven
 *
 * The simplest possible Sven program:
 *   - Open a window
 *   - Draw a white square that moves when you press arrow keys
 *   - Close on Escape or the window's X button
 *
 * Build with CMake (from the repo root):
 *   cmake -B build && cmake --build build
 *   ./build/examples/hello
 */

#include <sven.h>

int main() {
    Sven::Window game("Hello Sven", 800, 600);

    int x = 368;  // Start the square roughly centred.
    int y = 268;
    const int speed = 4;

    while (game.running()) {
        // ── 1. Process input ──────────────────────────────────────────────
        game.pollEvents();

        if (Sven::Keyboard::isKeyDown(SVEN_KEY_RIGHT)) x += speed;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_LEFT))  x -= speed;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_DOWN))  y += speed;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_UP))    y -= speed;

        // ── 2. Draw ───────────────────────────────────────────────────────
        game.clear(Sven::Color(30, 30, 30));   // dark grey background

        // Draw a white square at the current position.
        game.drawRect(x, y, 64, 64, Sven::Color::White());

        // Draw a red circle in the top-left as decoration.
        game.drawCircle(80, 80, 40, Sven::Color::Red());

        // Draw a line across the bottom.
        game.drawLine(0, 580, 800, 580, Sven::Color(80, 80, 80));

        // ── 3. Show the frame ─────────────────────────────────────────────
        game.present();
    }

    return 0;
}
