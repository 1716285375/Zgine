#pragma once

#include <Zgine/Renderer/RHI/VertexArray.h>

namespace Zgine {

class VulkanVertexArray final : public VertexArray {
public:
    VulkanVertexArray();
    ~VulkanVertexArray() override = default;

    void Bind() const override;
    void Unbind() const override;

    void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    uint32_t GetID() const override { return m_RendererID; }
    const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

    [[nodiscard]] const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const {
        return m_VertexBuffers;
    }

private:
    uint32_t m_RendererID = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

} // namespace Zgine
