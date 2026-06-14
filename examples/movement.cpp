/**
 * examples/movement.cpp — Delta Time Movement
 *
 * Demonstrates the new v0.2 features:
 *   - Sven::Vec2          for position and velocity
 *   - Window::getDeltaTime() for frame-rate-independent movement
 *   - Window::getFPS()       for a simple on-screen performance readout
 *   - Window::setDrawColor() as a convenience for repeated draws
 *
 * The square moves at a constant speed in pixels-per-second, no matter
 * how fast or slow the computer runs. Arrow keys steer it; it bounces
 * off the window edges.
 *
 * Build with CMake (from the repo root):
 *   cmake -B build && cmake --build build
 *   ./build/examples/movement
 */

#include <sven.h>

int main() {
    Sven::Window game("Sven — Delta Time Movement", 800, 600);

    // Position and size of our square.
    Sven::Vec2 position(368.0f, 268.0f);
    const Sven::Vec2 size(64.0f, 64.0f);

    // Speed in pixels per second — same on every machine, thanks to dt.
    const float speed = 300.0f;

    // The square also drifts on its own so you can see motion even
    // without pressing any keys.
    Sven::Vec2 velocity(120.0f, 90.0f);

    while (game.running()) {
        // ── 1. Input & timing ─────────────────────────────────────────────
        game.pollEvents();
        float dt = game.getDeltaTime();

        // Arrow keys add to the automatic drift.
        Sven::Vec2 input = Sven::Vec2::Zero();
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_RIGHT)) input.x += 1.0f;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_LEFT))  input.x -= 1.0f;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_DOWN))  input.y += 1.0f;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_UP))    input.y -= 1.0f;

        // ── 2. Update ──────────────────────────────────────────────────────
        // Move by the automatic drift plus any keyboard input,
        // scaled by delta time so speed is in "pixels per second".
        position += velocity * dt;
        position += input * speed * dt;

        // Bounce off the edges of the window.
        if (position.x < 0.0f || position.x + size.x > static_cast<float>(game.getWidth())) {
            velocity.x = -velocity.x;
            // Clamp so the square doesn't get stuck outside the window.
            if (position.x < 0.0f) position.x = 0.0f;
            if (position.x + size.x > static_cast<float>(game.getWidth()))
                position.x = static_cast<float>(game.getWidth()) - size.x;
        }
        if (position.y < 0.0f || position.y + size.y > static_cast<float>(game.getHeight())) {
            velocity.y = -velocity.y;
            if (position.y < 0.0f) position.y = 0.0f;
            if (position.y + size.y > static_cast<float>(game.getHeight()))
                position.y = static_cast<float>(game.getHeight()) - size.y;
        }

        // ── 3. Draw ───────────────────────────────────────────────────────
        game.clear(Sven::Color(20, 20, 30));

        // Filled square at the current position.
        game.drawRect(position, size, Sven::Color::Cyan());

        // An outlined square for comparison, drawn with setDrawColor().
        game.setDrawColor(Sven::Color::Yellow());
        game.drawRectOutline(Sven::Vec2(20.0f, 20.0f), Sven::Vec2(100.0f, 30.0f),
                             game.getDrawColor(), 2);

        // A simple FPS bar: width scales with FPS (capped at 144).
        float fps = game.getFPS();
        int barWidth = static_cast<int>((fps / 144.0f) * 200.0f);
        if (barWidth > 200) barWidth = 200;
        if (barWidth < 0)   barWidth = 0;
        game.drawRect(20, 60, barWidth, 10, Sven::Color::Green());
        game.drawRectOutline(20, 60, 200, 10, Sven::Color::Gray());

        game.present();
    }

    return 0;
}
