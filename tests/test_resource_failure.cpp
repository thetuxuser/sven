/**
 * test_resource_failure.cpp — Texture / Sound / Music failure handling
 *
 * These tests create a real Sven::Window (using SDL's "dummy" video and
 * audio drivers, so they run headless on CI) and confirm that loading a
 * nonexistent file:
 *
 *   - does NOT throw or crash
 *   - leaves the resource in an "invalid" state (isValid() == false)
 *   - leaves Texture's width/height at 0
 *   - makes playback functions (play/pause/etc.) safe no-ops
 *
 * This is the behaviour the rest of Sven relies on to stay
 * crash-free when asset files are missing.
 */

#include "sven_test.h"
#include <sven.h>

#include <cstdlib>

// Force headless/dummy drivers so this test can run on CI machines with no
// display or sound card. Must be set before the first SDL_Init call, which
// happens inside the Sven::Window constructor.
static void useDummyDrivers() {
#if defined(_WIN32)
    _putenv_s("SDL_VIDEODRIVER", "dummy");
    _putenv_s("SDL_AUDIODRIVER", "dummy");
#else
    setenv("SDL_VIDEODRIVER", "dummy", 1);
    setenv("SDL_AUDIODRIVER", "dummy", 1);
#endif
}

TEST(Texture_MissingFile_IsInvalidNotCrashing) {
    useDummyDrivers();
    Sven::Window window("Sven Tests", 320, 240);

    Sven::Texture texture("this/file/does/not/exist.png");

    EXPECT_FALSE(texture.isValid());
    EXPECT_EQ(texture.getWidth(), 0);
    EXPECT_EQ(texture.getHeight(), 0);

    // Drawing an invalid texture must not crash. drawTexture() should
    // silently do nothing.
    window.clear();
    window.drawTexture(texture, 0, 0);
    window.drawTexture(texture, Sven::Vec2(10.0f, 10.0f));
    window.drawTexture(texture, 0, 0, Sven::Rect{0, 0, 16, 16});
    window.present();
}

TEST(Texture_MoveSemantics_TransferOwnership) {
    useDummyDrivers();
    Sven::Window window("Sven Tests", 320, 240);

    Sven::Texture a("this/file/does/not/exist.png");
    EXPECT_FALSE(a.isValid());

    Sven::Texture b = std::move(a);
    // Moved-from texture should also report invalid (it had nothing to
    // begin with), and the moved-to texture should still be safely usable.
    EXPECT_FALSE(b.isValid());
    EXPECT_EQ(b.getWidth(), 0);
}

TEST(Sound_MissingFile_IsInvalidNotCrashing) {
    useDummyDrivers();
    Sven::Window window("Sven Tests", 320, 240);

    Sven::Sound sound("this/file/does/not/exist.wav");

    EXPECT_FALSE(sound.isValid());

    // play() on an invalid sound must be a safe no-op.
    sound.play();

    // Volume getters/setters should also be safe.
    sound.setVolume(0.5f);
    EXPECT_TRUE(sound.getVolume() >= 0.0f);
}

TEST(Music_MissingFile_IsInvalidNotCrashing) {
    useDummyDrivers();
    Sven::Window window("Sven Tests", 320, 240);

    Sven::Music music("this/file/does/not/exist.ogg");

    EXPECT_FALSE(music.isValid());

    // All playback controls must be safe no-ops on an invalid track.
    music.play(true);
    music.pause();
    music.resume();
    music.stop();

    EXPECT_FALSE(music.isPlaying());

    music.setVolume(0.5f);
    // getVolume() should not crash even if audio failed to initialise.
    float v = music.getVolume();
    EXPECT_TRUE(v >= 0.0f && v <= 1.0f);
}

TEST(Sound_And_Music_MoveSemantics) {
    useDummyDrivers();
    Sven::Window window("Sven Tests", 320, 240);

    Sven::Sound s1("this/file/does/not/exist.wav");
    Sven::Sound s2 = std::move(s1);
    EXPECT_FALSE(s2.isValid());
    s2.play(); // still safe after move

    Sven::Music m1("this/file/does/not/exist.ogg");
    Sven::Music m2 = std::move(m1);
    EXPECT_FALSE(m2.isValid());
    m2.play(); // still safe after move
}

int main() {
    return Sven::Test::runAll();
}
