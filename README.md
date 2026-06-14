# Sven

[![CI](https://github.com/thetuxuser/sven/actions/workflows/ci.yml/badge.svg)](https://github.com/thetuxuser/sven/actions/workflows/ci.yml)

**A simple, beginner-friendly 2D game library written in modern C++.**

Sven is designed to be easier than SDL and simpler than Raylib. If you want to
learn game development without drowning in boilerplate, Sven is for you.

```cpp
#include <sven.h>

int main() {
    Sven::Window game("Hello Sven", 800, 600);

    while (game.running()) {
        game.pollEvents();

        game.clear();
        game.drawRect(100, 100, 64, 64);
        game.present();
    }
}
```

That's the whole program. No `Init()` call, no manual cleanup, no SDL headers
in sight.

---

## Features

| Feature | Status |
|---------|--------|
| Window creation | ✅ |
| Main loop | ✅ |
| Keyboard input | ✅ |
| Filled rectangles | ✅ |
| Rectangle outlines | ✅ |
| Filled circles | ✅ |
| Lines & points | ✅ |
| Colour support | ✅ |
| `Sven::Vec2` (2D vectors) | ✅ *(v0.2)* |
| `setDrawColor()` | ✅ *(v0.2)* |
| Delta time | ✅ *(v0.2)* |
| FPS calculation | ✅ *(v0.2)* |
| `Sven::Texture` (image loading) | ✅ *(v0.3)* |
| `drawTexture()` (sprites) | ✅ *(v0.3)* |
| Sprite-sheet cropping (`Sven::Rect`) | ✅ *(v0.3)* |
| `Sven::Sound` (sound effects) | ✅ *(v0.4)* |
| `Sven::Music` (background music) | ✅ *(v0.4)* |
| SDL3 / SDL3_image / SDL3_mixer hidden | ✅ |

---

## Installing

### Prerequisites

Sven uses [SDL3](https://libsdl.org), [SDL3_image](https://github.com/libsdl-org/SDL_image),
and [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer) internally.
Install all three for your platform:

**Ubuntu / Debian:**
```bash
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-mixer-dev cmake build-essential
```

**Arch Linux:**
```bash
sudo pacman -S sdl3 sdl3_image sdl3_mixer cmake
```

**macOS (Homebrew):**
```bash
brew install sdl3 sdl3_image sdl3_mixer cmake
```

**Windows (vcpkg):**
```powershell
vcpkg install sdl3 sdl3-image sdl3-mixer
```

### Building Sven

```bash
git clone https://github.com/thetuxuser/sven.git
cd sven
cmake -B build
cmake --build build
```

This builds the library (`libsven.a`), the [examples](#running-the-examples),
the [demo games](demos/), and the [unit tests](tests/). Each of these can be
turned off individually:

```bash
cmake -B build -DSVEN_BUILD_EXAMPLES=OFF -DSVEN_BUILD_DEMOS=OFF -DSVEN_BUILD_TESTS=OFF
```

### Running the unit tests

```bash
cmake -B build && cmake --build build
cd build && ctest --output-on-failure
```

The tests cover `Vec2` math, `Rect`, and the failure handling for
`Texture`/`Sound`/`Music` (missing files should never crash). They run
headless via SDL's dummy video/audio drivers, so they work in CI without a
display or sound card.

### Using Sven in your own project

**Option A — as a subdirectory** (simplest, good for game jams / small projects):

```cmake
# Your CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(MyGame LANGUAGES CXX)

add_subdirectory(sven)

add_executable(mygame main.cpp)
target_link_libraries(mygame PRIVATE sven)
```

**Option B — as an installed package** (good once Sven is built and
installed system-wide or to a known prefix):

```bash
cmake -B build && cmake --build build
cmake --install build --prefix /usr/local   # or any prefix you like
```

```cmake
# Your CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(MyGame LANGUAGES CXX)

find_package(Sven CONFIG REQUIRED)

add_executable(mygame main.cpp)
target_link_libraries(mygame PRIVATE Sven::sven)
```

Either way, `#include <sven.h>` and you're done.

---

## Quick start

### 1. Open a window

```cpp
Sven::Window game("My Game", 800, 600);
```

### 2. Main loop

```cpp
while (game.running()) {
    game.pollEvents();   // must be called every frame
    game.clear();
    // ... draw stuff ...
    game.present();
}
```

### 3. Drawing

```cpp
// Filled shapes
game.drawRect  (x, y, w, h);                    // white by default
game.drawRect  (x, y, w, h, Sven::Color::Red());
game.drawCircle(cx, cy, radius);
game.drawCircle(cx, cy, radius, Sven::Color(255, 128, 0));

// Outlines
game.drawRectOutline(x, y, w, h);
game.drawRectOutline(x, y, w, h, Sven::Color::Green(), 2); // 2px border

// Lines and points
game.drawLine (x1, y1, x2, y2);
game.drawPoint(x,  y);
```

### 4. Colours

```cpp
// Named presets
Sven::Color::Black()
Sven::Color::White()
Sven::Color::Red()
Sven::Color::Green()
Sven::Color::Blue()
Sven::Color::Yellow()
Sven::Color::Cyan()
Sven::Color::Magenta()
Sven::Color::Gray()

// Custom RGB / RGBA
Sven::Color(255, 128, 0)        // orange
Sven::Color(255, 255, 255, 128) // semi-transparent white
```

You can also set a "current" colour and reuse it:

```cpp
window.setDrawColor(Sven::Color::Red());
Sven::Color c = window.getDrawColor(); // Sven::Color::Red()
```

### 5. Vec2 — positions and movement *(new in v0.2)*

`Sven::Vec2` is a small (x, y) pair with the usual arithmetic operators,
useful for positions, velocities, and sizes.

```cpp
Sven::Vec2 position(100.0f, 100.0f);
Sven::Vec2 velocity(50.0f, 0.0f);

position += velocity * deltaTime;   // move 50px/sec to the right

position.length();       // magnitude
position.normalized();   // unit vector
Sven::Vec2::Zero();       // (0, 0)
```

`drawRect` and `drawRectOutline` accept `Vec2` for position and size:

```cpp
game.drawRect(position, Sven::Vec2(64, 64), Sven::Color::Cyan());
game.drawRectOutline(position, Sven::Vec2(64, 64), Sven::Color::Yellow(), 2);
```

### 6. Delta time and FPS *(new in v0.2)*

```cpp
while (game.running()) {
    game.pollEvents();

    float dt = game.getDeltaTime();  // seconds since last frame
    position.x += speed * dt;        // speed is in pixels per second

    float fps = game.getFPS();       // updated roughly once per second
}
```

This keeps movement speed consistent no matter how fast the computer runs.

### 7. Keyboard input

```cpp
if (Sven::Keyboard::isKeyDown(SVEN_KEY_SPACE)) {
    // held this frame
}

if (Sven::Keyboard::isKeyPressed(SVEN_KEY_RETURN)) {
    // just pressed — fires once per key press
}
```

**Available key constants:** `SVEN_KEY_A`–`Z`, `SVEN_KEY_0`–`9`,
`SVEN_KEY_SPACE`, `SVEN_KEY_RETURN`, `SVEN_KEY_ESCAPE`, `SVEN_KEY_BACKSPACE`,
`SVEN_KEY_UP`, `SVEN_KEY_DOWN`, `SVEN_KEY_LEFT`, `SVEN_KEY_RIGHT`,
`SVEN_KEY_F1`–`F12`.

Escape always closes the window automatically.

### 8. Textures & sprites *(new in v0.3)*

Load an image once, then draw it every frame. A `Sven::Window` must exist
before you create a `Sven::Texture`.

```cpp
Sven::Window game("My Game", 800, 600);
Sven::Texture player("assets/player.png");

while (game.running()) {
    game.pollEvents();
    game.clear();

    game.drawTexture(player, 100, 100);          // by x, y
    game.drawTexture(player, position);          // or by Vec2

    game.present();
}
```

If the file can't be loaded, `player.isValid()` returns `false` and
`drawTexture()` simply does nothing — your game won't crash over a missing
image. An error is printed to stderr so you can spot the problem.

```cpp
player.getWidth();   // image width in pixels (0 if invalid)
player.getHeight();  // image height in pixels (0 if invalid)
```

**Sprite sheets:** a sprite sheet is one image file containing several
frames laid out in a grid. `Sven::Rect` lets you crop out a single frame:

```cpp
// Grab the 32x32 tile at column 2, row 0 of the sheet.
Sven::Rect frame = {2 * 32, 0, 32, 32};
game.drawTexture(sheet, 100, 100, frame);
```

Sven v0.3 doesn't include an animation system — you choose which frame to
draw each time, e.g. by advancing `frame.x` over time. A full animation
helper may arrive in a later version.

### 9. Sound & music *(new in v0.4)*

Like `Texture`, both `Sound` and `Music` require a `Sven::Window` to exist
first (Sven sets up its audio system alongside the window).

**Sound** — short effects (jumps, hits, coins). Loaded fully into memory and
safe to play repeatedly, even overlapping with itself:

```cpp
Sven::Sound jump("jump.wav");

if (Sven::Keyboard::isKeyPressed(SVEN_KEY_SPACE)) {
    jump.play();
}

jump.setVolume(0.8f); // 0.0 (silent) to 1.0 (full volume)
```

**Music** — streaming background music. Only one track plays at a time;
starting a new one stops the previous track:

```cpp
Sven::Music bgm("theme.ogg");

bgm.play(true);  // true = loop forever
bgm.pause();
bgm.resume();
bgm.stop();

bgm.setVolume(0.5f);
bgm.isPlaying(); // → bool
```

If a file can't be loaded, `isValid()` returns `false` and every playback
function becomes a harmless no-op — your game keeps running silently
instead of crashing. An error is printed to stderr.

```cpp
jump.isValid();
bgm.isValid();
```

Sven v0.4 keeps audio deliberately minimal: no mixer graph, no channel
control, no DSP effects, no 3D/positional audio. `Sound` plays once per
call (overlapping is fine); `Music` is a single global stream.

---

## Running the examples

**hello** — basic window, drawing, and keyboard input:
```bash
./build/examples/hello
```
Use the arrow keys to move the white square around. Close with Escape or the
window's × button.

**movement** — delta-time-based movement, `Vec2`, and FPS *(new in v0.2)*:
```bash
./build/examples/movement
```
A cyan square drifts around the screen and bounces off the edges at a
constant speed (pixels per second), regardless of frame rate. Arrow keys add
extra movement. A green bar in the top-left shows the current FPS.

**sprite** — textures and sprite drawing *(new in v0.3)*:
```bash
./build/examples/sprite
```
Loads `examples/assets/player.png` (a tiny placeholder sprite included with
the repo) and draws it twice: once at full size, moved with the arrow keys,
and once cropped to its top-left 16x16 corner using `Sven::Rect`.

> **Note:** run this from the repository root so the relative path
> `examples/assets/player.png` resolves correctly, or edit the path in
> `examples/sprite.cpp` to point at your own image.

**audio** — sound effects and background music *(new in v0.4)*:
```bash
./build/examples/audio
```
Plays a looping background tone immediately. Press SPACE for a short jump
sound (try mashing it — sounds overlap), M to pause/resume the music, and
UP/DOWN to change the music volume (shown as a green bar). Run from the
repository root so `examples/assets/jump.wav` and `examples/assets/theme.wav`
resolve correctly.

---

## Demo games

The [`demos/`](demos/) directory contains small, complete games built only
with Sven's existing API — see [demos/README.md](demos/README.md) for the
full list.

**pong**:
```bash
./build/pong
```
Two-paddle Pong with delta-time movement, AABB collision, score display
(drawn as rectangles — no text rendering needed), and a bounce sound effect.
Left paddle: W/S. Right paddle: arrow keys. Run from the repository root so
`examples/assets/jump.wav` resolves.

---

## Roadmap

| Version | Focus |
|---------|-------|
| v0.1 | Window, drawing, keyboard |
| v0.2 | Vec2, delta time, FPS, setDrawColor |
| v0.3 | Textures, sprites, sprite-sheet cropping |
| **v0.4** | Sound effects & background music — *current* |
| **v1.0** | Polish: CI, demo games, unit tests, packaging, Windows support |

Sven's core feature set is now considered complete. Between v0.4 and v1.0,
the focus shifts from new features to quality: build infrastructure, tests,
demo games, and a proper CMake package — not new APIs.

### Sven v1.0 goals

**Core**
- Window, input, `Vec2`, `Rect`, `Color`, delta time, FPS

**Graphics**
- Rectangle drawing, textures, sprite rendering, sprite-sheet cropping

**Audio**
- Sound effects, music playback

**Project quality**
- ✅ Linux CI · ⬜ Windows CI (workflow added, unverified — see [#known-gaps](#architecture-notes--known-limitations))
- ✅ Installable CMake package (`find_package(Sven CONFIG REQUIRED)`)
- ✅ CI workflow (build + run unit tests on every push)
- ✅ Documentation
- 🚧 Demo games — Pong done; Snake, Breakout, Flappy Bird planned
- ✅ Unit tests (Vec2 math, Rect, texture/audio failure handling)

### Non-goals (for v1.0 and beyond)

Sven intentionally does **not** include:

- ECS
- Physics engine
- Scene graph
- Networking
- Scripting
- OpenGL/Vulkan abstraction
- 3D rendering

Sven is a tiny, clean, readable C++ 2D game library — not a game engine.

---

## Architecture notes & known limitations

Sven aims to be simple to read end-to-end. A few deliberate simplifications
worth knowing about:

- **Single active window.** Sven currently supports one `Sven::Window` at a
  time. Internally, `Texture`, `Sound`, and `Music` look up the
  "currently active" renderer/audio device via small internal globals
  (`getActiveRenderer()`, `isAudioReady()`) rather than taking a `Window&`,
  since requiring one would complicate the simple, beginner-friendly API.
  This is fine for Sven's target use cases (one game, one window) but means
  multi-window support would need rework.
- **Single global music stream.** `Sven::Music` maps onto SDL3_mixer's
  single global music channel. Starting a new `Music::play()` stops whatever
  was playing before, even if it came from a different `Music` object.
  `Sound` effects, by contrast, can overlap freely.
- **No animation system.** Sprite-sheet frame selection (`Sven::Rect`) is
  manual — Sven doesn't track timers or frame sequences for you. This keeps
  the core small; a thin animation helper may be considered post-v1.0 if it
  can be done without adding engine-like complexity.
- **Windows CI is unverified.** A GitHub Actions workflow for MSVC + vcpkg
  is included (`.github/workflows/ci.yml`), but hasn't yet been confirmed
  green on a real Windows runner — package names and `find_package` config
  paths for SDL3/SDL3_image/SDL3_mixer via vcpkg may need small adjustments.
  Linux builds (apt + GCC) are the primary supported path until this is
  confirmed.

---

## Design goals

- **Simple** — a student should be able to read the entire source in an
  afternoon.
- **No magic** — no ECS, no scene graphs, no scripting engine.
- **No SDL leakage** — SDL3, SDL3_image, and SDL3_mixer are implementation
  details. Your code never sees them.
- **Modern C++20** — but nothing exotic.
- **Cross-platform** — Linux first, with Windows support targeted for v1.0.

---

## Contributing

Issues and PRs are welcome. Please keep changes small and the code readable —
that is Sven's whole point.

---

## License

MIT — see [LICENSE](LICENSE).
