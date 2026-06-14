/**
 * graphics.cpp — Sven Drawing Primitives
 *
 * Implements Window::drawRect, drawCircle, drawLine, drawPoint, etc.
 * All rendering goes through the SDL3 renderer; none of that is visible
 * to the user.
 */

#include "sven_internal.h"

#include <cmath>     // std::sqrt, std::round
#include <algorithm> // std::max

namespace Sven {

// ─── Vec2 ─────────────────────────────────────────────────────────────────────

float Vec2::length() const {
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::normalized() const {
    float len = length();
    if (len == 0.0f) return Vec2::Zero();
    return Vec2(x / len, y / len);
}

// ─── Filled rectangle ─────────────────────────────────────────────────────────

void Window::drawRect(int x, int y, int width, int height, Color color) {
    SDL_Renderer* r = Internal::RendererAccess::get(*this);

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    SDL_FRect rect {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height)
    };
    SDL_RenderFillRect(r, &rect);
}

void Window::drawRect(Vec2 position, Vec2 size, Color color) {
    drawRect(
        static_cast<int>(position.x), static_cast<int>(position.y),
        static_cast<int>(size.x),     static_cast<int>(size.y),
        color
    );
}

// ─── Rectangle outline ────────────────────────────────────────────────────────

void Window::drawRectOutline(int x, int y, int width, int height,
                             Color color, int lineWidth)
{
    // Draw multiple inset rectangles to fake line thickness.
    int lw = std::max(1, lineWidth);
    for (int i = 0; i < lw; ++i) {
        drawRect(x + i,         y + i,          width  - 2*i, lw - i, color); // top
        drawRect(x + i,         y + height - lw,width  - 2*i, lw - i, color); // bottom
        drawRect(x + i,         y + i,          lw - i, height - 2*i, color); // left
        drawRect(x + width - lw,y + i,          lw - i, height - 2*i, color); // right
    }
}

void Window::drawRectOutline(Vec2 position, Vec2 size, Color color, int lineWidth) {
    drawRectOutline(
        static_cast<int>(position.x), static_cast<int>(position.y),
        static_cast<int>(size.x),     static_cast<int>(size.y),
        color, lineWidth
    );
}

// ─── Filled circle ────────────────────────────────────────────────────────────
// Uses a midpoint-circle scan-line fill: no trig needed, just integer math.

void Window::drawCircle(int cx, int cy, int radius, Color color) {
    SDL_Renderer* r = Internal::RendererAccess::get(*this);

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    // For each row in the bounding box, draw a horizontal span.
    for (int dy = -radius; dy <= radius; ++dy) {
        // Width of the chord at this y-offset.
        int dx = static_cast<int>(
            std::round(std::sqrt(static_cast<double>(radius * radius - dy * dy)))
        );

        SDL_FRect span {
            static_cast<float>(cx - dx),
            static_cast<float>(cy + dy),
            static_cast<float>(dx * 2 + 1),
            1.0f
        };
        SDL_RenderFillRect(r, &span);
    }
}

// ─── Line ─────────────────────────────────────────────────────────────────────

void Window::drawLine(int x1, int y1, int x2, int y2, Color color) {
    SDL_Renderer* r = Internal::RendererAccess::get(*this);

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderLine(r,
        static_cast<float>(x1), static_cast<float>(y1),
        static_cast<float>(x2), static_cast<float>(y2));
}

// ─── Point ────────────────────────────────────────────────────────────────────

void Window::drawPoint(int x, int y, Color color) {
    SDL_Renderer* r = Internal::RendererAccess::get(*this);

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(r,
        static_cast<float>(x),
        static_cast<float>(y));
}

// ─── Sprites (textures) ────────────────────────────────────────────────────────

void Window::drawTexture(const Texture& texture, int x, int y) {
    // Skip silently if the texture failed to load — keeps games from
    // crashing over a missing image file.
    if (!texture.isValid()) return;

    SDL_Renderer* r       = Internal::RendererAccess::get(*this);
    SDL_Texture*  sdlTex  = Internal::TextureAccess::get(texture);

    SDL_FRect dst {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(texture.getWidth()),
        static_cast<float>(texture.getHeight())
    };

    // nullptr source rect = use the whole texture.
    SDL_RenderTexture(r, sdlTex, nullptr, &dst);
}

void Window::drawTexture(const Texture& texture, Vec2 position) {
    drawTexture(texture, static_cast<int>(position.x), static_cast<int>(position.y));
}

void Window::drawTexture(const Texture& texture, int x, int y, Rect srcRect) {
    if (!texture.isValid()) return;

    SDL_Renderer* r      = Internal::RendererAccess::get(*this);
    SDL_Texture*  sdlTex = Internal::TextureAccess::get(texture);

    SDL_FRect src {
        static_cast<float>(srcRect.x),
        static_cast<float>(srcRect.y),
        static_cast<float>(srcRect.width),
        static_cast<float>(srcRect.height)
    };

    SDL_FRect dst {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(srcRect.width),
        static_cast<float>(srcRect.height)
    };

    SDL_RenderTexture(r, sdlTex, &src, &dst);
}

void Window::drawTexture(const Texture& texture, Vec2 position, Rect srcRect) {
    drawTexture(texture, static_cast<int>(position.x), static_cast<int>(position.y), srcRect);
}

} // namespace Sven
