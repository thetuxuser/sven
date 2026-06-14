/**
 * examples/audio.cpp — Sound & Music
 *
 * Demonstrates the new v0.4 features:
 *   - Sven::Sound   short, overlapping sound effects
 *   - Sven::Music   streaming background music with loop/pause/stop
 *
 * Controls:
 *   SPACE  — play the "jump" sound effect (can overlap itself)
 *   M      — toggle background music play / pause
 *   UP     — increase music volume
 *   DOWN   — decrease music volume
 *
 * The assets used here (assets/jump.wav, assets/theme.wav) are tiny
 * generated placeholder tones included with the repo — swap in your own!
 *
 * Build with CMake (from the repo root):
 *   cmake -B build && cmake --build build
 *   ./build/examples/audio
 */

#include <sven.h>
#include <cstdio>

int main() {
    Sven::Window game("Sven — Audio", 800, 600);

    // Short sound effect — loaded fully into memory, safe to play often
    // and even overlap with itself.
    Sven::Sound jump("examples/assets/jump.wav");

    // Background music — streamed from disk, only one plays at a time.
    Sven::Music bgm("examples/assets/theme.wav");

    if (!jump.isValid() || !bgm.isValid()) {
        // isValid() lets you handle missing audio files gracefully.
        // (An error was already printed to stderr.)
        std::fprintf(stderr, "Sven: one or more audio files failed to load.\n");
    }

    // Start the music looping immediately.
    bgm.play(true);
    bool musicPlaying = true;

    float volume = 0.5f;
    bgm.setVolume(volume);

    while (game.running()) {
        game.pollEvents();

        // ── Sound effect ──────────────────────────────────────────────────
        if (Sven::Keyboard::isKeyPressed(SVEN_KEY_SPACE)) {
            jump.play();
        }

        // ── Toggle music ──────────────────────────────────────────────────
        if (Sven::Keyboard::isKeyPressed(SVEN_KEY_M)) {
            if (musicPlaying) {
                bgm.pause();
            } else {
                bgm.resume();
            }
            musicPlaying = !musicPlaying;
        }

        // ── Volume control ────────────────────────────────────────────────
        if (Sven::Keyboard::isKeyPressed(SVEN_KEY_UP)) {
            volume += 0.1f;
            bgm.setVolume(volume);
        }
        if (Sven::Keyboard::isKeyPressed(SVEN_KEY_DOWN)) {
            volume -= 0.1f;
            bgm.setVolume(volume);
        }

        // ── Draw ──────────────────────────────────────────────────────────
        game.clear(Sven::Color(20, 20, 30));

        // A simple volume bar so you can see the current music volume.
        float currentVolume = bgm.getVolume();
        int barWidth = static_cast<int>(currentVolume * 200.0f);
        game.drawRectOutline(20, 20, 200, 20, Sven::Color::Gray());
        game.drawRect(20, 20, barWidth, 20, Sven::Color::Green());

        // A square that lights up briefly isn't tracked here to keep this
        // example focused on audio — press SPACE and listen for the jump
        // sound, and M to pause/resume the music.
        game.drawRect(350, 268, 100, 64,
            musicPlaying ? Sven::Color::Cyan() : Sven::Color::Gray());

        game.present();
    }

    return 0;
}
