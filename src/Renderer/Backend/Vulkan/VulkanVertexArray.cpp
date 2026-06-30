#include "VulkanVertexArray.h"

#include "VulkanContextAccess.h"

namespace Zgine {

VulkanVertexArray::VulkanVertexArray()
    : m_RendererID(Vulkan::AllocateResourceID())
{
}

void VulkanVertexArray::Bind() const {
    // Vulkan records vertex/index binding directly into command buffers.
}

void VulkanVertexArray::Unbind() const {
    // Vulkan has no global vertex-array binding state.
}

void VulkanVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    if (vertexBuffer) {
        m_VertexBuffers.push_back(vertexBuffer);
    }
}

void VulkanVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    m_IndexBuffer = indexBuffer;
}

} // namespace Zgine
