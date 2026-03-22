#pragma once

#include <Zgine/Renderer/RHI/Texture.h>
#include <memory>

namespace Zgine {

class TextureDefaults {
public:
    static void Initialize();

    static std::shared_ptr<Texture> White();
    static std::shared_ptr<Texture> Black();
    static std::shared_ptr<Texture> FlatNormal();

private:
    static std::shared_ptr<Texture> s_White;
    static std::shared_ptr<Texture> s_Black;
    static std::shared_ptr<Texture> s_FlatNormal;
};

} // namespace Zgine
