/**
 * demos/pong/pong.cpp — Pong
 *
 * A complete, tiny Pong implementation using only Sven's v0.1–v0.4
 * features: Window, Vec2, Color, delta time, keyboard input, rectangle
 * drawing, and a sound effect on each bounce.
 *
 * Controls:
 *   Left paddle:  W (up) / S (down)
 *   Right paddle: UP / DOWN arrows
 *   Escape:       quit (built into Sven)
 *
 * This file intentionally has no classes, no ECS, no scene graph — just a
 * handful of Vec2s, some if-statements, and a loop. That's the point.
 *
 * Build with CMake (from the repo root):
 *   cmake -B build && cmake --build build
 *   ./build/pong
 *
 * Run from the repo root so examples/assets/jump.wav resolves (reused
 * here as the bounce sound effect, to avoid adding a new asset).
 */

#include <sven.h>
#include <cstdlib>

namespace {

constexpr int WINDOW_WIDTH  = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr float PADDLE_WIDTH  = 16.0f;
constexpr float PADDLE_HEIGHT = 100.0f;
constexpr float PADDLE_SPEED  = 400.0f; // pixels per second

constexpr float BALL_SIZE  = 16.0f;
constexpr float BALL_SPEED = 350.0f; // pixels per second

/** Keeps a paddle's Y position within the window. */
void clampPaddle(Sven::Vec2& paddle) {
    if (paddle.y < 0.0f) paddle.y = 0.0f;
    if (paddle.y + PADDLE_HEIGHT > WINDOW_HEIGHT) {
        paddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
    }
}

/** Resets the ball to the centre with a pseudo-random direction. */
Sven::Vec2 resetBall() {
    Sven::Vec2 velocity(BALL_SPEED, BALL_SPEED * 0.6f);
    if (std::rand() % 2 == 0) velocity.x = -velocity.x;
    if (std::rand() % 2 == 0) velocity.y = -velocity.y;
    return velocity;
}

} // namespace

int main() {
    Sven::Window game("Sven — Pong", WINDOW_WIDTH, WINDOW_HEIGHT);

    // Reused as the bounce sound effect — see file comment above.
    Sven::Sound bounce("examples/assets/jump.wav");

    // Paddles: position is the top-left corner.
    Sven::Vec2 leftPaddle(40.0f, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2.0f);
    Sven::Vec2 rightPaddle(WINDOW_WIDTH - 40.0f - PADDLE_WIDTH,
                            (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2.0f);

    // Ball.
    Sven::Vec2 ballPos((WINDOW_WIDTH - BALL_SIZE) / 2.0f,
                        (WINDOW_HEIGHT - BALL_SIZE) / 2.0f);
    Sven::Vec2 ballVel = resetBall();

    int leftScore  = 0;
    int rightScore = 0;

    while (game.running()) {
        game.pollEvents();
        float dt = game.getDeltaTime();

        // ── Input: left paddle (W/S) ────────────────────────────────────────
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_W)) leftPaddle.y -= PADDLE_SPEED * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_S)) leftPaddle.y += PADDLE_SPEED * dt;

        // ── Input: right paddle (arrow keys) ────────────────────────────────
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_UP))   rightPaddle.y -= PADDLE_SPEED * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_DOWN)) rightPaddle.y += PADDLE_SPEED * dt;

        clampPaddle(leftPaddle);
        clampPaddle(rightPaddle);

        // ── Ball movement ────────────────────────────────────────────────────
        ballPos += ballVel * dt;

        // Bounce off top/bottom walls.
        if (ballPos.y < 0.0f) {
            ballPos.y = 0.0f;
            ballVel.y = -ballVel.y;
            bounce.play();
        }
        if (ballPos.y + BALL_SIZE > WINDOW_HEIGHT) {
            ballPos.y = WINDOW_HEIGHT - BALL_SIZE;
            ballVel.y = -ballVel.y;
            bounce.play();
        }

        // Bounce off left paddle.
        if (ballVel.x < 0.0f
            && ballPos.x <= leftPaddle.x + PADDLE_WIDTH
            && ballPos.x + BALL_SIZE >= leftPaddle.x
            && ballPos.y + BALL_SIZE >= leftPaddle.y
            && ballPos.y <= leftPaddle.y + PADDLE_HEIGHT)
        {
            ballPos.x = leftPaddle.x + PADDLE_WIDTH;
            ballVel.x = -ballVel.x;
            bounce.play();
        }

        // Bounce off right paddle.
        if (ballVel.x > 0.0f
            && ballPos.x + BALL_SIZE >= rightPaddle.x
            && ballPos.x <= rightPaddle.x + PADDLE_WIDTH
            && ballPos.y + BALL_SIZE >= rightPaddle.y
            && ballPos.y <= rightPaddle.y + PADDLE_HEIGHT)
        {
            ballPos.x = rightPaddle.x - BALL_SIZE;
            ballVel.x = -ballVel.x;
            bounce.play();
        }

        // Scoring: ball passes a paddle's edge of the screen.
        if (ballPos.x < 0.0f) {
            ++rightScore;
            ballPos = Sven::Vec2((WINDOW_WIDTH - BALL_SIZE) / 2.0f,
                                  (WINDOW_HEIGHT - BALL_SIZE) / 2.0f);
            ballVel = resetBall();
        }
        if (ballPos.x + BALL_SIZE > WINDOW_WIDTH) {
            ++leftScore;
            ballPos = Sven::Vec2((WINDOW_WIDTH - BALL_SIZE) / 2.0f,
                                  (WINDOW_HEIGHT - BALL_SIZE) / 2.0f);
            ballVel = resetBall();
        }

        // ── Draw ────────────────────────────────────────────────────────────
        game.clear(Sven::Color::Black());

        // Centre line (dashed, drawn as small rectangles).
        for (int y = 0; y < WINDOW_HEIGHT; y += 30) {
            game.drawRect(WINDOW_WIDTH / 2 - 2, y, 4, 16, Sven::Color::Gray());
        }

        // Paddles and ball.
        game.drawRect(leftPaddle,  Sven::Vec2(PADDLE_WIDTH, PADDLE_HEIGHT), Sven::Color::White());
        game.drawRect(rightPaddle, Sven::Vec2(PADDLE_WIDTH, PADDLE_HEIGHT), Sven::Color::White());
        game.drawRect(ballPos,     Sven::Vec2(BALL_SIZE, BALL_SIZE),        Sven::Color::White());

        // Score, drawn as simple blocks: one block per point (keeps this
        // demo free of any text-rendering dependency).
        for (int i = 0; i < leftScore; ++i) {
            game.drawRect(WINDOW_WIDTH / 2 - 40 - i * 20, 20, 12, 12, Sven::Color::Yellow());
        }
        for (int i = 0; i < rightScore; ++i) {
            game.drawRect(WINDOW_WIDTH / 2 + 28 + i * 20, 20, 12, 12, Sven::Color::Yellow());
        }

        game.present();
    }

    return 0;
}
