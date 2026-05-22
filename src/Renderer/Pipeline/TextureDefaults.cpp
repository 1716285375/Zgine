#include <Zgine/Renderer/Pipeline/TextureDefaults.h>

namespace Zgine {

std::shared_ptr<Texture> TextureDefaults::s_White;
std::shared_ptr<Texture> TextureDefaults::s_Black;
std::shared_ptr<Texture> TextureDefaults::s_FlatNormal;

void TextureDefaults::Initialize() {
    // 1x1 white (albedo fallback, AO fallback)
    unsigned char white[] = { 255, 255, 255, 255 };
    s_White = Texture::Create(white, 1, 1, "default_white");

    // 1x1 black (metallic fallback = 0.0)
    unsigned char black[] = { 0, 0, 0, 255 };
    s_Black = Texture::Create(black, 1, 1, "default_black");

    // 1x1 flat normal map (tangent-space up = (0,0,1) encoded as (128,128,255))
    unsigned char normal[] = { 128, 128, 255, 255 };
    s_FlatNormal = Texture::Create(normal, 1, 1, "default_normal");
}

void TextureDefaults::Shutdown() {
    s_White.reset();
    s_Black.reset();
    s_FlatNormal.reset();
}

std::shared_ptr<Texture> TextureDefaults::White()      { return s_White; }
std::shared_ptr<Texture> TextureDefaults::Black()      { return s_Black; }
std::shared_ptr<Texture> TextureDefaults::FlatNormal() { return s_FlatNormal; }

} // namespace Zgine
