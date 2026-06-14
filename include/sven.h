#pragma once

/**
 * sven.h — The Sven Game Library (v0.4)
 *
 * A simple, beginner-friendly 2D game library written in modern C++.
 * Sven wraps SDL3 (and SDL3_image, SDL3_mixer) so you never have to think
 * about them.
 *
 * Quick start:
 *
 *   #include <sven.h>
 *
 *   int main() {
 *       Sven::Window game("My Game", 800, 600);
 *       Sven::Texture player("player.png");
 *       Sven::Sound   jump("jump.wav");
 *       Sven::Music   bgm("theme.ogg");
 *
 *       bgm.play(true); // loop
 *
 *       while (game.running()) {
 *           game.pollEvents();
 *
 *           if (Sven::Keyboard::isKeyPressed(SVEN_KEY_SPACE)) {
 *               jump.play();
 *           }
 *
 *           game.clear();
 *           game.drawTexture(player, 100, 100);
 *           game.present();
 *       }
 *   }
 *
 * New in v0.2:
 *   - Sven::Vec2            simple 2D vector for positions and movement
 *   - Window::setDrawColor  set a default colour for future draw calls
 *   - Window::getDeltaTime  frame time in seconds, for smooth movement
 *   - Window::getFPS        current frames-per-second
 *   - drawRect / drawRectOutline overloads that accept Vec2
 *
 * New in v0.3:
 *   - Sven::Rect            simple integer rectangle, used for sprite-sheet cropping
 *   - Sven::Texture         load an image file and draw it as a sprite
 *   - Window::drawTexture   draw a texture (optionally cropped) at a position
 *
 * New in v0.4:
 *   - Sven::Sound           load a short sound effect and play it
 *   - Sven::Music           load streaming background music (play/pause/stop/loop)
 *
 * v0.1, v0.2, and v0.3 code continues to work unchanged.
 */

#include <string>
#include <cstdint>

// ─── Key codes ────────────────────────────────────────────────────────────────
// A small, friendly subset of keys. More will be added in v0.2+.

#define SVEN_KEY_UNKNOWN   0
#define SVEN_KEY_A         4
#define SVEN_KEY_B         5
#define SVEN_KEY_C         6
#define SVEN_KEY_D         7
#define SVEN_KEY_E         8
#define SVEN_KEY_F         9
#define SVEN_KEY_G        10
#define SVEN_KEY_H        11
#define SVEN_KEY_I        12
#define SVEN_KEY_J        13
#define SVEN_KEY_K        14
#define SVEN_KEY_L        15
#define SVEN_KEY_M        16
#define SVEN_KEY_N        17
#define SVEN_KEY_O        18
#define SVEN_KEY_P        19
#define SVEN_KEY_Q        20
#define SVEN_KEY_R        21
#define SVEN_KEY_S        22
#define SVEN_KEY_T        23
#define SVEN_KEY_U        24
#define SVEN_KEY_V        25
#define SVEN_KEY_W        26
#define SVEN_KEY_X        27
#define SVEN_KEY_Y        28
#define SVEN_KEY_Z        29
#define SVEN_KEY_1        30
#define SVEN_KEY_2        31
#define SVEN_KEY_3        32
#define SVEN_KEY_4        33
#define SVEN_KEY_5        34
#define SVEN_KEY_6        35
#define SVEN_KEY_7        36
#define SVEN_KEY_8        37
#define SVEN_KEY_9        38
#define SVEN_KEY_0        39
#define SVEN_KEY_RETURN   40
#define SVEN_KEY_ESCAPE   41
#define SVEN_KEY_BACKSPACE 42
#define SVEN_KEY_SPACE    44
#define SVEN_KEY_RIGHT    79
#define SVEN_KEY_LEFT     80
#define SVEN_KEY_DOWN     81
#define SVEN_KEY_UP       82
#define SVEN_KEY_F1       58
#define SVEN_KEY_F2       59
#define SVEN_KEY_F3       60
#define SVEN_KEY_F4       61
#define SVEN_KEY_F5       62
#define SVEN_KEY_F6       63
#define SVEN_KEY_F7       64
#define SVEN_KEY_F8       65
#define SVEN_KEY_F9       66
#define SVEN_KEY_F10      67
#define SVEN_KEY_F11      68
#define SVEN_KEY_F12      69

namespace Sven {

// Forward declaration: used internally so Sven's own source files can
// access the underlying SDL3 renderer. Not part of the public API.
namespace Internal { struct RendererAccess; struct TextureAccess; struct SoundAccess; struct MusicAccess; }

// ─── Color ────────────────────────────────────────────────────────────────────

/**
 * A simple RGBA color.
 * All channels are in the range [0, 255].
 */
struct Color {
    uint8_t r, g, b, a;

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    // A handful of built-in colors for convenience.
    static constexpr Color Black()   { return {  0,   0,   0}; }
    static constexpr Color White()   { return {255, 255, 255}; }
    static constexpr Color Red()     { return {255,   0,   0}; }
    static constexpr Color Green()   { return {  0, 255,   0}; }
    static constexpr Color Blue()    { return {  0,   0, 255}; }
    static constexpr Color Yellow()  { return {255, 255,   0}; }
    static constexpr Color Cyan()    { return {  0, 255, 255}; }
    static constexpr Color Magenta() { return {255,   0, 255}; }
    static constexpr Color Gray()    { return {128, 128, 128}; }
};

// ─── Vec2 ─────────────────────────────────────────────────────────────────────

/**
 * A simple 2D vector / point, using floats.
 *
 * Used for positions, movement, and anything else that needs an (x, y) pair.
 * Comes with basic arithmetic operators so movement code stays readable.
 *
 * Example:
 *   Sven::Vec2 position(100.0f, 100.0f);
 *   Sven::Vec2 velocity(50.0f, 0.0f);
 *
 *   position = position + velocity * deltaTime;
 */
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float x, float y) : x(x), y(y) {}

    // ── Arithmetic ───────────────────────────────────────────────────────────

    constexpr Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    constexpr Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    constexpr Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    constexpr Vec2 operator/(float scalar) const {
        return Vec2(x / scalar, y / scalar);
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x; y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }

    Vec2& operator*=(float scalar) {
        x *= scalar; y *= scalar;
        return *this;
    }

    constexpr bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    // ── Useful helpers ───────────────────────────────────────────────────────

    /** Returns the length (magnitude) of the vector. */
    [[nodiscard]] float length() const;

    /** Returns a unit-length copy of this vector (zero stays zero). */
    [[nodiscard]] Vec2 normalized() const;

    /** Shorthand for Vec2(0, 0). */
    static constexpr Vec2 Zero() { return Vec2(0.0f, 0.0f); }
};

// ─── Rect ─────────────────────────────────────────────────────────────────────

/**
 * A simple integer rectangle: position + size.
 *
 * Used mainly for cropping a region out of a texture (e.g. picking one
 * frame out of a sprite sheet).
 *
 * Example:
 *   // Grab the 32x32 tile at column 2, row 0 of a sprite sheet.
 *   Sven::Rect frame = {2 * 32, 0, 32, 32};
 *   window.drawTexture(sheet, 100, 100, frame);
 */
struct Rect {
    int x = 0;
    int y = 0;
    int width  = 0;
    int height = 0;
};

// ─── Texture ──────────────────────────────────────────────────────────────────

/**
 * A loaded image, ready to be drawn with Window::drawTexture().
 *
 * Loading happens immediately in the constructor. A Sven::Window must
 * already exist before you create a Texture (Sven needs an active window
 * to upload the image to the graphics hardware).
 *
 * If the file can't be loaded, the Texture becomes "invalid": isValid()
 * returns false, getWidth()/getHeight() return 0, and drawTexture() simply
 * does nothing (no crash). An error message is printed to stderr.
 *
 * Example:
 *   Sven::Texture player("assets/player.png");
 *   if (!player.isValid()) {
 *       // handle missing file — e.g. skip drawing it
 *   }
 */
class Texture {
public:
    /**
     * Loads an image from disk (PNG, JPG, BMP, and other formats supported
     * by SDL3_image).
     *
     * @param path Path to the image file, relative to the working directory
     *             or absolute.
     */
    explicit Texture(const std::string& path);

    /** Frees the underlying GPU texture. */
    ~Texture();

    // Non-copyable (owns a GPU resource) but movable.
    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    /** Returns true if the image loaded successfully. */
    [[nodiscard]] bool isValid() const;

    /** Returns the image width in pixels (0 if invalid). */
    [[nodiscard]] int getWidth() const;

    /** Returns the image height in pixels (0 if invalid). */
    [[nodiscard]] int getHeight() const;

private:
    struct Impl;
    Impl* m_impl;

    // Allows Window::drawTexture() to reach the underlying SDL3 texture
    // without exposing it in this public header.
    friend struct Internal::TextureAccess;
};

// ─── Sound ────────────────────────────────────────────────────────────────────

/**
 * A short sound effect, loaded entirely into memory and ready to play.
 *
 * Use this for things like jump sounds, hits, coins, UI clicks — anything
 * short that might play many times or overlap with itself.
 *
 * A Sven::Window must already exist before you create a Sound (Sven sets
 * up its audio system alongside the window).
 *
 * If the file can't be loaded, the Sound becomes "invalid": isValid()
 * returns false and play() does nothing (no crash). An error message is
 * printed to stderr.
 *
 * Example:
 *   Sven::Sound jump("jump.wav");
 *
 *   if (Sven::Keyboard::isKeyPressed(SVEN_KEY_SPACE)) {
 *       jump.play();
 *   }
 */
class Sound {
public:
    /**
     * Loads a sound effect from disk (WAV, OGG, and other formats
     * supported by SDL3_mixer).
     *
     * @param path Path to the audio file, relative to the working
     *             directory or absolute.
     */
    explicit Sound(const std::string& path);

    /** Frees the loaded sound. */
    ~Sound();

    // Non-copyable (owns an audio resource) but movable.
    Sound(const Sound&)            = delete;
    Sound& operator=(const Sound&) = delete;

    Sound(Sound&& other) noexcept;
    Sound& operator=(Sound&& other) noexcept;

    /** Returns true if the sound loaded successfully. */
    [[nodiscard]] bool isValid() const;

    /**
     * Plays the sound once. If it's already playing, this starts another
     * overlapping copy (handy for rapid-fire effects).
     *
     * Does nothing if the sound failed to load.
     */
    void play() const;

    /**
     * Sets the playback volume for this sound.
     *
     * @param volume 0.0 (silent) to 1.0 (full volume). Values outside this
     *               range are clamped.
     */
    void setVolume(float volume);

    /** Returns the current volume, 0.0 to 1.0. */
    [[nodiscard]] float getVolume() const;

private:
    struct Impl;
    Impl* m_impl;

    friend struct Internal::SoundAccess;
};

// ─── Music ────────────────────────────────────────────────────────────────────

/**
 * Streaming background music.
 *
 * Unlike Sound, only one Music track can play at a time — starting a new
 * one stops whatever was playing before. This matches how most simple
 * games use background music.
 *
 * A Sven::Window must already exist before you create a Music object.
 *
 * If the file can't be loaded, the Music becomes "invalid": isValid()
 * returns false and play()/pause()/etc. do nothing (no crash). An error
 * message is printed to stderr.
 *
 * Example:
 *   Sven::Music bgm("theme.ogg");
 *   bgm.play(true); // loop forever
 *   // ...
 *   bgm.pause();
 *   bgm.resume();
 *   bgm.stop();
 */
class Music {
public:
    /**
     * Loads a music track from disk (OGG, MP3, and other formats
     * supported by SDL3_mixer). Streamed from disk during playback rather
     * than fully decoded into memory, so it's suitable for longer tracks.
     *
     * @param path Path to the audio file, relative to the working
     *             directory or absolute.
     */
    explicit Music(const std::string& path);

    /** Stops playback (if this track is playing) and frees it. */
    ~Music();

    // Non-copyable (owns an audio resource) but movable.
    Music(const Music&)            = delete;
    Music& operator=(const Music&) = delete;

    Music(Music&& other) noexcept;
    Music& operator=(Music&& other) noexcept;

    /** Returns true if the music loaded successfully. */
    [[nodiscard]] bool isValid() const;

    /**
     * Starts playing this track from the beginning, stopping any other
     * music that was playing.
     *
     * @param loop If true, the track repeats forever until stop() is
     *             called or another Music is played. If false, it plays
     *             once and then stops.
     *
     * Does nothing if the music failed to load.
     */
    void play(bool loop = false);

    /** Pauses playback. Can be resumed with resume(). */
    void pause();

    /** Resumes playback after pause(). */
    void resume();

    /** Stops playback entirely (cannot be resumed; use play() again). */
    void stop();

    /** Returns true if this track is currently playing (not paused/stopped). */
    [[nodiscard]] bool isPlaying() const;

    /**
     * Sets the playback volume for music.
     *
     * Note: SDL3_mixer has a single global music volume, so this affects
     * all Music objects, not just this one. It's exposed on Music for a
     * simple, discoverable API.
     *
     * @param volume 0.0 (silent) to 1.0 (full volume). Values outside this
     *               range are clamped.
     */
    void setVolume(float volume);

    /** Returns the current music volume, 0.0 to 1.0. */
    [[nodiscard]] float getVolume() const;

private:
    struct Impl;
    Impl* m_impl;

    friend struct Internal::MusicAccess;
};

// ─── Keyboard ─────────────────────────────────────────────────────────────────

/**
 * Static class for querying keyboard state.
 *
 * Example:
 *   if (Sven::Keyboard::isKeyDown(SVEN_KEY_LEFT)) { ... }
 */
class Keyboard {
public:
    /**
     * Returns true if the given key is currently held down.
     * Use the SVEN_KEY_* constants as the argument.
     */
    static bool isKeyDown(int scancode);

    /**
     * Returns true if the key was just pressed this frame
     * (i.e. it was up last frame and is down this frame).
     */
    static bool isKeyPressed(int scancode);

    // Internal — called by Window::pollEvents(). Do not call directly.
    static void _update();
};

// ─── Window ───────────────────────────────────────────────────────────────────

/**
 * The main Sven class. Creates a window, manages the game loop, and
 * provides drawing primitives.
 *
 * Only one Window should exist at a time.
 *
 * Example:
 *   Sven::Window game("My Game", 800, 600);
 *   while (game.running()) {
 *       game.pollEvents();
 *       game.clear();
 *       game.drawRect(100, 100, 64, 64);
 *       game.present();
 *   }
 */
class Window {
public:
    /**
     * Creates a window with the given title and dimensions.
     * Sven is initialised automatically.
     *
     * @param title  The text shown in the window title bar.
     * @param width  Window width in pixels.
     * @param height Window height in pixels.
     */
    Window(const std::string& title, int width, int height);

    /**
     * Destroys the window and shuts down Sven.
     */
    ~Window();

    // Non-copyable — there should only ever be one window.
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    // ── Loop control ─────────────────────────────────────────────────────────

    /**
     * Returns true while the game should keep running.
     * Becomes false when the user closes the window or presses Escape.
     */
    [[nodiscard]] bool running() const;

    /**
     * Process OS events (keyboard, mouse, close button, etc.).
     * Call this once per frame, before drawing.
     */
    void pollEvents();

    // ── Drawing ──────────────────────────────────────────────────────────────

    /**
     * Sets the "current" draw colour used by clear() and any drawing
     * function called without an explicit colour argument.
     *
     * This is optional — every drawing function still accepts its own
     * colour argument as in v0.1. setDrawColor() is just a convenience
     * for when you want to draw several things in the same colour.
     *
     * Example:
     *   window.setDrawColor(Sven::Color::Red());
     *   window.drawRect(10, 10, 32, 32);   // drawn in red
     *   window.drawRect(50, 10, 32, 32);   // also red
     */
    void setDrawColor(Color color);

    /** Returns the current draw colour set by setDrawColor(). */
    [[nodiscard]] Color getDrawColor() const;

    /**
     * Fills the window with a solid colour (default: black).
     * Call at the start of each frame.
     */
    void clear(Color color = Color::Black());

    /**
     * Draws a filled rectangle.
     *
     * @param x      Left edge of the rectangle in pixels.
     * @param y      Top  edge of the rectangle in pixels.
     * @param width  Width  in pixels.
     * @param height Height in pixels.
     * @param color  Fill colour (default: white).
     */
    void drawRect(int x, int y, int width, int height,
                  Color color = Color::White());

    /**
     * Draws a filled rectangle using a Vec2 for position and size.
     *
     * @param position Top-left corner (position.x, position.y) in pixels.
     * @param size     Width and height (size.x, size.y) in pixels.
     * @param color    Fill colour (default: white).
     */
    void drawRect(Vec2 position, Vec2 size, Color color = Color::White());

    /**
     * Draws only the outline of a rectangle (not filled).
     *
     * @param x           Left edge in pixels.
     * @param y           Top  edge in pixels.
     * @param width       Width  in pixels.
     * @param height      Height in pixels.
     * @param color       Line colour (default: white).
     * @param lineWidth   Thickness of the outline in pixels (default: 1).
     */
    void drawRectOutline(int x, int y, int width, int height,
                         Color color = Color::White(), int lineWidth = 1);

    /**
     * Draws a rectangle outline using a Vec2 for position and size.
     *
     * @param position  Top-left corner (position.x, position.y) in pixels.
     * @param size      Width and height (size.x, size.y) in pixels.
     * @param color     Line colour (default: white).
     * @param lineWidth Thickness of the outline in pixels (default: 1).
     */
    void drawRectOutline(Vec2 position, Vec2 size,
                         Color color = Color::White(), int lineWidth = 1);

    /**
     * Draws a filled circle.
     *
     * @param cx     Centre X in pixels.
     * @param cy     Centre Y in pixels.
     * @param radius Radius in pixels.
     * @param color  Fill colour (default: white).
     */
    void drawCircle(int cx, int cy, int radius,
                    Color color = Color::White());

    /**
     * Draws a line between two points.
     *
     * @param x1, y1  Start point in pixels.
     * @param x2, y2  End   point in pixels.
     * @param color   Line colour (default: white).
     */
    void drawLine(int x1, int y1, int x2, int y2,
                  Color color = Color::White());

    /**
     * Draws a single pixel.
     */
    void drawPoint(int x, int y, Color color = Color::White());

    // ── Sprites ──────────────────────────────────────────────────────────────

    /**
     * Draws a texture (sprite) at the given position, at its native size.
     *
     * If the texture failed to load (texture.isValid() == false), this
     * does nothing.
     *
     * @param texture The texture to draw.
     * @param x       Left edge in pixels.
     * @param y       Top  edge in pixels.
     */
    void drawTexture(const Texture& texture, int x, int y);

    /**
     * Draws a texture at the given position, using a Vec2.
     *
     * @param texture  The texture to draw.
     * @param position Top-left corner (position.x, position.y) in pixels.
     */
    void drawTexture(const Texture& texture, Vec2 position);

    /**
     * Draws a cropped region of a texture at the given position — useful
     * for sprite sheets, where one image file contains many frames.
     *
     * The drawn size on screen matches srcRect's width/height (no scaling).
     *
     * @param texture The texture to draw from.
     * @param x       Left edge of the destination in pixels.
     * @param y       Top  edge of the destination in pixels.
     * @param srcRect The region of the texture to copy, in pixels.
     *
     * Example:
     *   // Draw the second 32x32 frame of a sprite sheet.
     *   Sven::Rect frame = {32, 0, 32, 32};
     *   window.drawTexture(sheet, 100, 100, frame);
     */
    void drawTexture(const Texture& texture, int x, int y, Rect srcRect);

    /**
     * Draws a cropped region of a texture at the given position, using a
     * Vec2 for the destination position.
     *
     * @param texture  The texture to draw from.
     * @param position Top-left corner of the destination in pixels.
     * @param srcRect  The region of the texture to copy, in pixels.
     */
    void drawTexture(const Texture& texture, Vec2 position, Rect srcRect);

    /**
     * Flips the back buffer to the screen.
     * Call at the end of each frame, after all drawing.
     */
    void present();

    // ── Window info ──────────────────────────────────────────────────────────

    /** Returns the window width in pixels. */
    [[nodiscard]] int getWidth()  const;

    /** Returns the window height in pixels. */
    [[nodiscard]] int getHeight() const;

    /** Returns the window title. */
    [[nodiscard]] std::string getTitle() const;

    /**
     * Changes the window title at runtime.
     */
    void setTitle(const std::string& title);

    // ── Timing ────────────────────────────────────────────────────────────────

    /**
     * Returns the time elapsed since the previous frame, in seconds.
     *
     * Call this once per frame (after pollEvents()) and use it to scale
     * movement so your game runs at the same speed regardless of how fast
     * the computer is.
     *
     * Example:
     *   float dt = window.getDeltaTime();
     *   position.x += speed * dt;   // speed is in pixels per second
     *
     * On the very first frame, deltaTime is 0.
     */
    [[nodiscard]] float getDeltaTime() const;

    /**
     * Returns the current frames-per-second, updated roughly once per
     * second. Useful for displaying performance info or debugging.
     *
     * On the first frame (before any measurement), returns 0.
     */
    [[nodiscard]] float getFPS() const;

private:
    // Opaque pointer to internal SDL3 state.
    // Users never touch this.
    struct Impl;
    Impl* m_impl;

    // Allows Sven's own .cpp files (graphics.cpp, input.cpp) to reach the
    // underlying SDL3 renderer without exposing it in this public header.
    friend struct Internal::RendererAccess;
};

} // namespace Sven
