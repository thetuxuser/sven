/**
 * window.cpp — Sven Window Implementation
 *
 * Wraps SDL_Window and SDL_Renderer.
 * None of the SDL types are exposed in the public header.
 */

#include "sven_internal.h"

#include <stdexcept>
#include <string>
#include <cstdint>

// ─── Active renderer tracking ──────────────────────────────────────────────────
// Sven supports a single window at a time. Texture (which has no access to
// a Window&) needs a renderer to upload pixel data to, so the Window
// constructor/destructor register themselves here. File-scope (TU-local)
// so it's reachable from both Sven:: and Sven::Internal::.
namespace {
    SDL_Renderer* g_activeRenderer = nullptr;
}

namespace Sven {

// ─── Private implementation struct ────────────────────────────────────────────
// By keeping this in the .cpp file we make sure SDL types never leak
// into the public header.

struct Window::Impl {
    SDL_Window*   sdlWindow   = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    bool          isRunning   = true;
    int           width       = 0;
    int           height      = 0;
    std::string   title;

    // The "current" colour set via setDrawColor(). Used by clear() when
    // no explicit colour is given... actually clear() always takes a
    // default argument, so this is mainly for user convenience / future use.
    Color drawColor = Color::White();

    // ── Timing state ────────────────────────────────────────────────────────
    uint64_t lastFrameTicks = 0;   // SDL_GetTicks() value at the last frame
    float    deltaTime      = 0.0f;

    // FPS is recalculated about once per second by averaging frame times
    // over that window, rather than reporting a single noisy frame's FPS.
    float    fps               = 0.0f;
    uint64_t fpsAccumulatorMs   = 0;
    int      fpsFrameCount      = 0;
};

// ─── Constructor / Destructor ─────────────────────────────────────────────────

Window::Window(const std::string& title, int width, int height)
    : m_impl(new Impl())
{
    // Initialise SDL3 (safe to call multiple times).
    Internal::init();

    m_impl->title  = title;
    m_impl->width  = width;
    m_impl->height = height;

    // Create the OS window.
    m_impl->sdlWindow = SDL_CreateWindow(
        title.c_str(),
        width, height,
        SDL_WINDOW_RESIZABLE
    );

    if (!m_impl->sdlWindow) {
        throw std::runtime_error(
            std::string("Sven: Could not create window: ") + SDL_GetError()
        );
    }

    // Create the GPU renderer that draws into the window.
    // "nullptr" lets SDL pick the best backend (Metal, D3D12, Vulkan, etc.).
    m_impl->sdlRenderer = SDL_CreateRenderer(m_impl->sdlWindow, nullptr);

    if (!m_impl->sdlRenderer) {
        SDL_DestroyWindow(m_impl->sdlWindow);
        throw std::runtime_error(
            std::string("Sven: Could not create renderer: ") + SDL_GetError()
        );
    }

    // Enable alpha blending by default so semi-transparent colours work.
    SDL_SetRenderDrawBlendMode(m_impl->sdlRenderer, SDL_BLENDMODE_BLEND);

    // Register this renderer so Texture can find it.
    g_activeRenderer = m_impl->sdlRenderer;

    // Start the timing clock. The first frame will report deltaTime == 0.
    m_impl->lastFrameTicks = SDL_GetTicks();
}

Window::~Window() {
    if (m_impl) {
        if (g_activeRenderer == m_impl->sdlRenderer) {
            g_activeRenderer = nullptr;
        }
        if (m_impl->sdlRenderer) SDL_DestroyRenderer(m_impl->sdlRenderer);
        if (m_impl->sdlWindow)   SDL_DestroyWindow(m_impl->sdlWindow);
        delete m_impl;
        m_impl = nullptr;
    }
    Internal::shutdown();
}

// ─── Loop control ─────────────────────────────────────────────────────────────

bool Window::running() const {
    return m_impl->isRunning;
}

void Window::pollEvents() {
    // ── Timing ──────────────────────────────────────────────────────────────
    // Measure how long the previous frame took, in seconds.
    uint64_t now = SDL_GetTicks();
    uint64_t elapsedMs = now - m_impl->lastFrameTicks;
    m_impl->lastFrameTicks = now;
    m_impl->deltaTime = static_cast<float>(elapsedMs) / 1000.0f;

    // Update the FPS counter roughly once per second by averaging the
    // frames seen during that second. This avoids a jittery, single-frame
    // FPS readout.
    m_impl->fpsAccumulatorMs += elapsedMs;
    m_impl->fpsFrameCount    += 1;
    if (m_impl->fpsAccumulatorMs >= 1000) {
        m_impl->fps = static_cast<float>(m_impl->fpsFrameCount) * 1000.0f
                     / static_cast<float>(m_impl->fpsAccumulatorMs);
        m_impl->fpsAccumulatorMs = 0;
        m_impl->fpsFrameCount    = 0;
    }

    // ── Events ──────────────────────────────────────────────────────────────
    // Let the keyboard module snapshot its "previous" state before
    // SDL processes new events.
    Keyboard::_update();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                m_impl->isRunning = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                // Escape always closes the window — handy default.
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    m_impl->isRunning = false;
                }
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                m_impl->width  = event.window.data1;
                m_impl->height = event.window.data2;
                break;

            default:
                break;
        }
    }
}

// ─── Window info ──────────────────────────────────────────────────────────────

int Window::getWidth()  const { return m_impl->width; }
int Window::getHeight() const { return m_impl->height; }

std::string Window::getTitle() const { return m_impl->title; }

void Window::setTitle(const std::string& title) {
    m_impl->title = title;
    SDL_SetWindowTitle(m_impl->sdlWindow, title.c_str());
}

// ─── Frame management ─────────────────────────────────────────────────────────

void Window::setDrawColor(Color color) {
    m_impl->drawColor = color;
}

Color Window::getDrawColor() const {
    return m_impl->drawColor;
}

void Window::clear(Color color) {
    SDL_SetRenderDrawColor(
        m_impl->sdlRenderer,
        color.r, color.g, color.b, color.a
    );
    SDL_RenderClear(m_impl->sdlRenderer);
}

void Window::present() {
    SDL_RenderPresent(m_impl->sdlRenderer);
}

// ─── Timing ───────────────────────────────────────────────────────────────────

float Window::getDeltaTime() const {
    return m_impl->deltaTime;
}

float Window::getFPS() const {
    return m_impl->fps;
}

// ─── Drawing primitives (implemented in graphics.cpp) ─────────────────────────
// Graphics functions need the renderer, which lives in Impl, so they are
// defined in graphics.cpp but declared via forward access to m_impl here.
// We expose the renderer through a small internal accessor below.

} // namespace Sven

// ─────────────────────────────────────────────────────────────────────────────
// Internal helper: let graphics.cpp and input.cpp reach the renderer.
// This is NOT in the public header.
// ─────────────────────────────────────────────────────────────────────────────

namespace Sven::Internal {

SDL_Renderer* RendererAccess::get(Window& window) {
    return window.m_impl->sdlRenderer;
}

SDL_Renderer* getActiveRenderer() {
    return g_activeRenderer;
}

} // namespace Sven::Internal
