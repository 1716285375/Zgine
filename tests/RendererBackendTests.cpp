#include <gtest/gtest.h>

#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Renderer/RHI/VertexArray.h>

namespace {

class RendererAPIGuard {
public:
    RendererAPIGuard()
        : m_Previous(Zgine::RendererAPI::GetAPI())
    {
    }

    ~RendererAPIGuard() {
        Zgine::RendererAPI::SetAPI(m_Previous);
    }

private:
    Zgine::RendererAPI::API m_Previous;
};

} // namespace

TEST(RendererBackendTest, ParsesBackendNamesAndAliases) {
    EXPECT_EQ(Zgine::RendererAPI::FromString("OpenGL"), Zgine::RendererAPI::API::OpenGL);
    EXPECT_EQ(Zgine::RendererAPI::FromString("gl"), Zgine::RendererAPI::API::OpenGL);
    EXPECT_EQ(Zgine::RendererAPI::FromString("DirectX12"), Zgine::RendererAPI::API::DirectX12);
    EXPECT_EQ(Zgine::RendererAPI::FromString("dx12"), Zgine::RendererAPI::API::DirectX12);
    EXPECT_EQ(Zgine::RendererAPI::FromString("d3d12"), Zgine::RendererAPI::API::DirectX12);
    EXPECT_EQ(Zgine::RendererAPI::FromString("Vulkan"), Zgine::RendererAPI::API::Vulkan);
    EXPECT_EQ(Zgine::RendererAPI::FromString("vk"), Zgine::RendererAPI::API::Vulkan);
    EXPECT_EQ(Zgine::RendererAPI::FromString("None"), Zgine::RendererAPI::API::None);
}

TEST(RendererBackendTest, ReportsImplementedBackendAvailability) {
    EXPECT_TRUE(Zgine::RendererAPI::IsAvailable(Zgine::RendererAPI::API::OpenGL));
    EXPECT_FALSE(Zgine::RendererAPI::IsAvailable(Zgine::RendererAPI::API::DirectX12));
#if ZGINE_HAS_VULKAN
    EXPECT_TRUE(Zgine::RendererAPI::IsAvailable(Zgine::RendererAPI::API::Vulkan));
#else
    EXPECT_FALSE(Zgine::RendererAPI::IsAvailable(Zgine::RendererAPI::API::Vulkan));
#endif
    EXPECT_FALSE(Zgine::RendererAPI::IsAvailable(Zgine::RendererAPI::API::None));
}

TEST(RendererBackendTest, UnsupportedBackendFactoriesFailExplicitly) {
    RendererAPIGuard guard;

    for (Zgine::RendererAPI::API api : {
             Zgine::RendererAPI::API::DirectX12,
#if !ZGINE_HAS_VULKAN
             Zgine::RendererAPI::API::Vulkan,
#endif
             Zgine::RendererAPI::API::None,
         }) {
        Zgine::RendererAPI::SetAPI(api);

        EXPECT_EQ(Zgine::RendererAPI::Create(), nullptr);
        EXPECT_EQ(Zgine::VertexArray::Create(), nullptr);
    }
}

#if ZGINE_HAS_VULKAN
TEST(RendererBackendTest, VulkanRendererAPIFactoryIsAvailableWhenSDKIsPresent) {
    RendererAPIGuard guard;

    Zgine::RendererAPI::SetAPI(Zgine::RendererAPI::API::Vulkan);

    EXPECT_NE(Zgine::RendererAPI::Create(), nullptr);
    EXPECT_EQ(Zgine::VertexArray::Create(), nullptr);
}
#endif
