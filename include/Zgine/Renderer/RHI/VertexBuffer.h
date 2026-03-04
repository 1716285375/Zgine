#pragma once

#include <memory>
#include <cstdint>

namespace Zgine {

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetID() const = 0;

        static std::shared_ptr<VertexBuffer> Create(const void* data, uint32_t size);
    };

}
