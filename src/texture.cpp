/**
 * texture.cpp — Sven Texture Implementation
 *
 * Loads images via SDL3_image and wraps the resulting SDL_Texture.
 * SDL types never appear in the public header.
 */

#include "sven_internal.h"

#include <cstdio>
#include <utility>

namespace Sven {

// ─── Private implementation struct ────────────────────────────────────────────

struct Texture::Impl {
    SDL_Texture* sdlTexture = nullptr;
    int          width      = 0;
    int          height     = 0;
};

// ─── Constructor ──────────────────────────────────────────────────────────────

Texture::Texture(const std::string& path)
    : m_impl(new Impl())
{
    SDL_Renderer* renderer = Internal::getActiveRenderer();

    if (!renderer) {
        // No window exists yet — Sven needs one to create GPU textures.
        std::fprintf(stderr,
            "Sven: Texture(\"%s\") failed: no Sven::Window exists yet. "
            "Create a Window before loading textures.\n",
            path.c_str());
        return; // m_impl stays zero-initialised => isValid() == false
    }

    // IMG_LoadTexture reads the file and uploads it to the GPU in one step.
    m_impl->sdlTexture = IMG_LoadTexture(renderer, path.c_str());

    if (!m_impl->sdlTexture) {
        // Loading failed (bad path, unsupported format, corrupt file, etc).
        // We don't throw — a missing texture shouldn't crash the game.
        std::fprintf(stderr,
            "Sven: Failed to load texture \"%s\": %s\n",
            path.c_str(), SDL_GetError());
        return;
    }

    // Query the real pixel dimensions of the loaded image.
    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(m_impl->sdlTexture, &w, &h);
    m_impl->width  = static_cast<int>(w);
    m_impl->height = static_cast<int>(h);
}

// ─── Destructor ───────────────────────────────────────────────────────────────

Texture::~Texture() {
    if (m_impl) {
        if (m_impl->sdlTexture) {
            SDL_DestroyTexture(m_impl->sdlTexture);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

// ─── Move semantics ───────────────────────────────────────────────────────────

Texture::Texture(Texture&& other) noexcept
    : m_impl(other.m_impl)
{
    other.m_impl = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_impl) {
            if (m_impl->sdlTexture) SDL_DestroyTexture(m_impl->sdlTexture);
            delete m_impl;
        }
        m_impl = other.m_impl;
        other.m_impl = nullptr;
    }
    return *this;
}

// ─── Accessors ────────────────────────────────────────────────────────────────

bool Texture::isValid() const {
    return m_impl && m_impl->sdlTexture != nullptr;
}

int Texture::getWidth() const {
    return m_impl ? m_impl->width : 0;
}

int Texture::getHeight() const {
    return m_impl ? m_impl->height : 0;
}

} // namespace Sven

// ─────────────────────────────────────────────────────────────────────────────
// Internal helper: let graphics.cpp reach the SDL texture for drawing.
// ─────────────────────────────────────────────────────────────────────────────

namespace Sven::Internal {

SDL_Texture* TextureAccess::get(const Texture& texture) {
    return texture.m_impl ? texture.m_impl->sdlTexture : nullptr;
}

} // namespace Sven::Internal
