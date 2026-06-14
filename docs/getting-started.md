# Getting Started with Sven

This guide takes you from zero to a moving square on screen in about five
minutes.

---

## Step 1 — Install dependencies

Sven wraps SDL3, SDL3_image, and SDL3_mixer, so you need all three on your
machine.

### Ubuntu / Debian
```bash
sudo apt update
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-mixer-dev cmake build-essential git
```

### Arch Linux
```bash
sudo pacman -S sdl3 sdl3_image sdl3_mixer cmake base-devel git
```

### macOS
```bash
brew install sdl3 sdl3_image sdl3_mixer cmake
```

### Windows
Install [Visual Studio](https://visualstudio.microsoft.com/) with the C++
workload, then:
```powershell
vcpkg install sdl3:x64-windows sdl3-image:x64-windows sdl3-mixer:x64-windows
```

---

## Step 2 — Clone and build Sven

```bash
git clone https://github.com/thetuxuser/sven.git
cd sven
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If it succeeds you'll see `libsven.a` (or `sven.lib` on Windows) in the
`build/` directory, plus the examples under `build/examples/`, the demo
games (e.g. `build/pong`), and the unit test binaries under `build/`.

To verify everything works, run the unit tests:

```bash
cd build && ctest --output-on-failure
```

---

## Step 3 — Run the examples

```bash
./build/examples/hello
```
A window opens with a white square. Move it with the arrow keys. Press Escape
or click the × to quit.

```bash
./build/examples/movement
```
A cyan square drifts and bounces around the window at a constant speed,
demonstrating `Sven::Vec2` and `getDeltaTime()`. A green bar shows the
current FPS. Arrow keys give it an extra push.

```bash
./build/examples/sprite
```
Loads a small placeholder image (`examples/assets/player.png`) and draws it
with `drawTexture()`. Move it with the arrow keys. A second, cropped copy in
the corner shows how `Sven::Rect` selects part of an image — the same
technique used for sprite sheets. Run this from the repo root so the image
path resolves correctly.

```bash
./build/examples/audio
```
Plays a looping background tone via `Sven::Music`. Press SPACE to play a
short jump sound (`Sven::Sound`) — try pressing it rapidly to hear sounds
overlap. Press M to pause/resume the music, and UP/DOWN to adjust its
volume. Run from the repo root so the audio files resolve correctly.

```bash
./build/pong
```
A complete two-paddle Pong game — left paddle on W/S, right paddle on the
arrow keys. Demonstrates `Vec2` movement, delta time, simple AABB collision,
and `Sound` for bounce effects, all in a single readable file
(`demos/pong/pong.cpp`). Run from the repo root so its sound effect resolves.

---

## Step 4 — Create your own project

Make a new directory **outside** the Sven repo:

```
mygame/
├── CMakeLists.txt
├── main.cpp
└── sven/          ← the cloned Sven repo (or a git submodule)
```

**CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.20)
project(MyGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(sven)

add_executable(mygame main.cpp)
target_link_libraries(mygame PRIVATE sven)
```

**main.cpp**
```cpp
#include <sven.h>

int main() {
    Sven::Window game("My Game", 800, 600);

    // Position as a Vec2 — easy to do math with.
    Sven::Vec2 player(368.0f, 268.0f);
    const float speed = 200.0f; // pixels per second

    while (game.running()) {
        game.pollEvents();
        float dt = game.getDeltaTime();

        // Move the player with arrow keys, scaled by delta time so the
        // speed is the same on every computer.
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_RIGHT)) player.x += speed * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_LEFT))  player.x -= speed * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_DOWN))  player.y += speed * dt;
        if (Sven::Keyboard::isKeyDown(SVEN_KEY_UP))    player.y -= speed * dt;

        game.clear(Sven::Color(20, 20, 40));  // dark blue background
        game.drawRect(player, Sven::Vec2(64, 64), Sven::Color::Yellow());
        game.present();
    }
}
```

Build it:
```bash
cd mygame
cmake -B build && cmake --build build
./build/mygame
```

---

## API summary

### Window

```cpp
Sven::Window win("Title", width, height);
win.running()           // → bool: keep looping?
win.pollEvents()        // process keyboard / close events, update timing
win.clear()             // fill with black
win.clear(color)        // fill with a colour
win.present()           // flip the frame to screen
win.getWidth()          // → int
win.getHeight()         // → int
win.setTitle("New title")
win.setDrawColor(color) // set a "current" colour for convenience
win.getDrawColor()      // → Color
win.getDeltaTime()      // → float: seconds since last frame
win.getFPS()            // → float: current frames-per-second
```

### Drawing

```cpp
win.drawRect(x, y, w, h)
win.drawRect(x, y, w, h, color)
win.drawRect(position, size)              // Vec2 overload (v0.2)
win.drawRect(position, size, color)       // Vec2 overload (v0.2)
win.drawRectOutline(x, y, w, h)
win.drawRectOutline(x, y, w, h, color, lineWidth)
win.drawRectOutline(position, size, color, lineWidth) // Vec2 overload (v0.2)
win.drawCircle(cx, cy, radius)
win.drawCircle(cx, cy, radius, color)
win.drawLine(x1, y1, x2, y2)
win.drawLine(x1, y1, x2, y2, color)
win.drawPoint(x, y)
win.drawPoint(x, y, color)
win.drawTexture(texture, x, y)              // sprite (v0.3)
win.drawTexture(texture, position)          // Vec2 overload (v0.3)
win.drawTexture(texture, x, y, srcRect)     // cropped sprite (v0.3)
win.drawTexture(texture, position, srcRect) // cropped, Vec2 overload (v0.3)
```

### Vec2 *(new in v0.2)*

```cpp
Sven::Vec2 v(x, y);
v + other,  v - other,  v * scalar,  v / scalar
v += other, v -= other, v *= scalar
v == other
v.length()        // → float
v.normalized()    // → Vec2
Sven::Vec2::Zero()
```

### Texture & Rect *(new in v0.3)*

```cpp
Sven::Texture tex("path/to/image.png"); // create AFTER a Window exists

tex.isValid()    // → bool: did the file load successfully?
tex.getWidth()   // → int: pixel width  (0 if invalid)
tex.getHeight()  // → int: pixel height (0 if invalid)

Sven::Rect r = {x, y, width, height}; // a crop region, in pixels
```

A `Texture` cannot be copied (it owns a GPU resource) but it can be moved:

```cpp
Sven::Texture a("image.png");
Sven::Texture b = std::move(a); // 'a' is now empty/invalid
```

### Sound & Music *(new in v0.4)*

```cpp
Sven::Sound jump("jump.wav"); // create AFTER a Window exists

jump.isValid()      // → bool: did the file load successfully?
jump.play()         // play once; can overlap with itself
jump.setVolume(v)   // 0.0 (silent) – 1.0 (full)
jump.getVolume()    // → float
```

```cpp
Sven::Music bgm("theme.ogg"); // create AFTER a Window exists

bgm.isValid()
bgm.play(loop)      // loop: bool, default false
bgm.pause()
bgm.resume()
bgm.stop()
bgm.isPlaying()     // → bool
bgm.setVolume(v)    // 0.0 – 1.0 (global music volume)
bgm.getVolume()     // → float
```

Both `Sound` and `Music` are non-copyable but movable, same as `Texture`.
If the audio device or file fails, `isValid()` is `false` and every
playback call becomes a no-op — no crashes, no exceptions.

### Colour

```cpp
Sven::Color(r, g, b)           // opaque
Sven::Color(r, g, b, a)        // with alpha (0 = transparent, 255 = opaque)
Sven::Color::Black()   White()  Red()    Green()
Sven::Color::Blue()    Yellow() Cyan()   Magenta()  Gray()
```

### Keyboard

```cpp
Sven::Keyboard::isKeyDown(SVEN_KEY_SPACE)    // held this frame
Sven::Keyboard::isKeyPressed(SVEN_KEY_SPACE) // just pressed this frame
```

Key constants: `SVEN_KEY_A`–`Z`, `SVEN_KEY_0`–`9`, `SVEN_KEY_SPACE`,
`SVEN_KEY_RETURN`, `SVEN_KEY_ESCAPE`, `SVEN_KEY_UP/DOWN/LEFT/RIGHT`,
`SVEN_KEY_F1`–`F12`.

---

## Common mistakes

**"CMake can't find SDL3" / "SDL3_image" / "SDL3_mixer"**
Make sure you installed `libsdl3-dev`, `libsdl3-image-dev`, and
`libsdl3-mixer-dev` (apt), or `sdl3`/`sdl3_image`/`sdl3_mixer`
(brew/pacman). If installed in a non-standard location, pass
`-DSDL3_DIR=...`, `-DSDL3_image_DIR=...`, and `-DSDL3_mixer_DIR=...` to
CMake.

**"Nothing appears on screen"**
Make sure you call `game.pollEvents()` every frame, and that you call
`game.present()` after drawing.

**"The window freezes"**
`pollEvents()` is missing from your loop.

**"My texture won't load" / nothing is drawn**
Check that:
- A `Sven::Window` was created *before* the `Sven::Texture`.
- The file path is correct relative to where you *run* the program (not
  where the source file lives). Sven prints an error to stderr if loading
  fails, and `texture.isValid()` will be `false`.

**"My sound/music won't play"**
- Make sure a `Sven::Window` was created first.
- Check the file path, same as for textures.
- If no audio device is available at all (e.g. a headless CI machine),
  Sven prints one warning at startup and every `Sound`/`Music` becomes
  silently invalid — your game keeps running without crashing.

---

## Next steps

Once you're comfortable, check out the [roadmap](../README.md#roadmap) to see
what's coming in future versions — text rendering and beyond.
