# Sven Demos

Small, complete games built with Sven, using nothing beyond the v0.1–v0.4
API. Each demo is meant to be read top-to-bottom in a few minutes — no
classes, no ECS, no engine scaffolding.

| Demo | What it shows |
|------|----------------|
| [pong](pong/pong.cpp) | `Vec2` movement, delta time, AABB collision, `Sound`, simple score display using rectangles |

## Building

Demos are built alongside the library and examples:

```bash
cmake -B build && cmake --build build
./build/pong
```

Run from the repository root so relative asset paths (e.g.
`examples/assets/jump.wav`) resolve correctly.

To skip building demos, configure with `-DSVEN_BUILD_DEMOS=OFF`.

## More demos

Snake, Breakout, and Flappy Bird are planned as part of Sven's v1.0 polish
pass (see the [roadmap](../README.md#roadmap)). Contributions following the
same "one file, no engine scaffolding" style are welcome.
