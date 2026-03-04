#pragma once

#include <string>
#include <memory>
#include <cstdint>

namespace Zgine {

    struct TextureSettings {
        bool GenerateMipmaps = true;
        bool SRGB = true;
        bool ClampToEdge = false;
        bool Linear = true;
    };

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetID() const = 0;
        virtual const std::string& GetFilePath() const = 0;

        static std::shared_ptr<Texture> Create(const std::string& path);
        static std::shared_ptr<Texture> Create(const std::string& path, const TextureSettings& settings);
        static std::shared_ptr<Texture> Create(const unsigned char* data, int size, const std::string& debugName);
        static std::shared_ptr<Texture> Create(const unsigned char* rgbaData, int width, int height, const std::string& debugName);
    };

}
