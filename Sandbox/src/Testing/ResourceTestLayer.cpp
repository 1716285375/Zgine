#include "sandbox_pch.h"
#include "ResourceTestLayer.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/ImGui/ImGuiWrapper.h"

namespace Sandbox {

    ResourceTestLayer::ResourceTestLayer()
        : ImGuiLayer()
    {
        ZG_CORE_INFO("Resource Test Layer created");
    }

    void ResourceTestLayer::OnAttach()
    {
        ZG_CORE_INFO("Resource Test Layer attached");
        
        // 初始化AssetLoader
        if (!m_AssetLoader.Initialize()) {
            ZG_CORE_ERROR("Failed to initialize AssetLoader");
        }
        
        // 设置测试资源路径
        m_TestTexturePaths = {
            "assets/textures/Checkerboard.png",
            "assets/textures/TestTexture.jpg"
        };
        
        m_TestShaderPaths = {
            "assets/shaders/Texture.glsl",
            "assets/shaders/Basic.glsl"
        };
        
        m_TestModelPaths = {
            "assets/models/cube.obj",
            "assets/models/sphere.obj"
        };
        
        m_TestAudioPaths = {
            "assets/audio/test.wav",
            "assets/audio/background.mp3"
        };
    }

    void ResourceTestLayer::OnDetach()
    {
        ZG_CORE_INFO("Resource Test Layer detached");
        m_AssetLoader.Shutdown();
    }

    void ResourceTestLayer::OnUpdate(Zgine::Timestep ts)
    {
        // 更新统计信息
        m_LastUpdateTime += ts.GetSeconds();
        if (m_LastUpdateTime >= 1.0f) {
            m_LastStats = m_AssetLoader.GetStatistics();
            m_LastUpdateTime = 0.0f;
        }
        
        // 检查异步任务状态
        for (auto it = m_ActiveTasks.begin(); it != m_ActiveTasks.end();) {
            if (m_AssetLoader.IsTaskCompleted(*it)) {
                m_LoadingResults.push_back("Task " + std::to_string(*it) + " completed");
                it = m_ActiveTasks.erase(it);
            } else {
                ++it;
            }
        }
    }

    void ResourceTestLayer::OnEvent(Zgine::Event& e)
    {
        // Handle events if necessary
    }

    void ResourceTestLayer::RenderCustomUI()
    {
        if (Zgine::IG::Begin("Resource Management Test", &m_ShowResourceManager))
        {
            if (Zgine::IG::BeginTabBar("ResourceTabs"))
            {
                if (Zgine::IG::BeginTabItem("Asset Loader"))
                {
                    RenderAssetLoaderUI();
                    Zgine::IG::EndTabItem();
                }
                
                if (Zgine::IG::BeginTabItem("Texture Manager"))
                {
                    RenderTextureManagerUI();
                    Zgine::IG::EndTabItem();
                }
                
                if (Zgine::IG::BeginTabItem("Shader Manager"))
                {
                    RenderShaderManagerUI();
                    Zgine::IG::EndTabItem();
                }
                
                if (Zgine::IG::BeginTabItem("Model Manager"))
                {
                    RenderModelManagerUI();
                    Zgine::IG::EndTabItem();
                }
                
                if (Zgine::IG::BeginTabItem("Audio Manager"))
                {
                    RenderAudioManagerUI();
                    Zgine::IG::EndTabItem();
                }
                
                if (Zgine::IG::BeginTabItem("Statistics"))
                {
                    RenderStatisticsUI();
                    Zgine::IG::EndTabItem();
                }
                
                Zgine::IG::EndTabBar();
            }
        }
        Zgine::IG::End();
    }

    void ResourceTestLayer::RenderAssetLoaderUI()
    {
        Zgine::IG::Text("Asset Loader Test");
        Zgine::IG::Separator();
        
        // 同步加载测试
        if (Zgine::IG::CollapsingHeader("Synchronous Loading"))
        {
            if (Zgine::IG::Button("Load Test Texture"))
            {
                auto resource = m_AssetLoader.LoadTexture(m_TestTexturePaths[0]);
                if (resource) {
                    ZG_CORE_INFO("Loaded texture: {}", m_TestTexturePaths[0]);
                }
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Load Test Shader"))
            {
                auto resource = m_AssetLoader.LoadShader(m_TestShaderPaths[0]);
                if (resource) {
                    ZG_CORE_INFO("Loaded shader: {}", m_TestShaderPaths[0]);
                }
            }
            
            if (Zgine::IG::Button("Load Test Model"))
            {
                auto resource = m_AssetLoader.LoadModel(m_TestModelPaths[0]);
                if (resource) {
                    ZG_CORE_INFO("Loaded model: {}", m_TestModelPaths[0]);
                }
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Load Test Audio"))
            {
                auto resource = m_AssetLoader.LoadAudio(m_TestAudioPaths[0]);
                if (resource) {
                    ZG_CORE_INFO("Loaded audio: {}", m_TestAudioPaths[0]);
                }
            }
        }
        
        // 异步加载测试
        if (Zgine::IG::CollapsingHeader("Asynchronous Loading"))
        {
            if (Zgine::IG::Button("Load All Async"))
            {
                std::vector<std::string> allPaths;
                allPaths.insert(allPaths.end(), m_TestTexturePaths.begin(), m_TestTexturePaths.end());
                allPaths.insert(allPaths.end(), m_TestShaderPaths.begin(), m_TestShaderPaths.end());
                allPaths.insert(allPaths.end(), m_TestModelPaths.begin(), m_TestModelPaths.end());
                allPaths.insert(allPaths.end(), m_TestAudioPaths.begin(), m_TestAudioPaths.end());
                
                uint32_t taskID = m_AssetLoader.LoadAssetsBatch(allPaths, [](const std::vector<Zgine::Resources::ResourceRef>& resources) {
                    ZG_CORE_INFO("Batch loading completed: {} resources loaded", resources.size());
                });
                
                m_ActiveTasks.push_back(taskID);
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Cancel All Tasks"))
            {
                for (uint32_t taskID : m_ActiveTasks) {
                    m_AssetLoader.CancelTask(taskID);
                }
                m_ActiveTasks.clear();
            }
        }
        
        // 任务状态
        if (Zgine::IG::CollapsingHeader("Task Status"))
        {
            Zgine::IG::Text("Active Tasks: %d", m_AssetLoader.GetActiveTaskCount());
            
            if (Zgine::IG::BeginChild("LoadingResults", ImVec2(0, 100), true))
            {
                for (const auto& result : m_LoadingResults) {
                    Zgine::IG::Text("%s", result.c_str());
                }
            }
            Zgine::IG::EndChild();
            
            if (Zgine::IG::Button("Clear Results"))
            {
                m_LoadingResults.clear();
            }
        }
    }

    void ResourceTestLayer::RenderTextureManagerUI()
    {
        Zgine::IG::Text("Texture Manager Test");
        Zgine::IG::Separator();
        
        // 这里可以添加纹理管理器的特定UI
        Zgine::IG::Text("Texture management features will be implemented here");
    }

    void ResourceTestLayer::RenderShaderManagerUI()
    {
        Zgine::IG::Text("Shader Manager Test");
        Zgine::IG::Separator();
        
        // 这里可以添加着色器管理器的特定UI
        Zgine::IG::Text("Shader management features will be implemented here");
    }

    void ResourceTestLayer::RenderModelManagerUI()
    {
        Zgine::IG::Text("Model Manager Test");
        Zgine::IG::Separator();
        
        // 原始几何体创建测试
        if (Zgine::IG::CollapsingHeader("Primitive Models"))
        {
            if (Zgine::IG::Button("Create Cube"))
            {
                auto resource = m_AssetLoader.LoadModel("primitive://cube_test");
                if (resource) {
                    ZG_CORE_INFO("Created cube model");
                }
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Create Sphere"))
            {
                auto resource = m_AssetLoader.LoadModel("primitive://sphere_test");
                if (resource) {
                    ZG_CORE_INFO("Created sphere model");
                }
            }
            
            if (Zgine::IG::Button("Create Plane"))
            {
                auto resource = m_AssetLoader.LoadModel("primitive://plane_test");
                if (resource) {
                    ZG_CORE_INFO("Created plane model");
                }
            }
        }
    }

    void ResourceTestLayer::RenderAudioManagerUI()
    {
        Zgine::IG::Text("Audio Manager Test");
        Zgine::IG::Separator();
        
        // 音频播放控制
        if (Zgine::IG::CollapsingHeader("Audio Playback"))
        {
            Zgine::IG::SliderFloat("Volume", &m_AudioVolume, 0.0f, 1.0f);
            Zgine::IG::Checkbox("Loop", &m_AudioLoop);
            
            if (Zgine::IG::Button("Play Test Audio"))
            {
                if (m_CurrentPlayID != 0) {
                    m_AssetLoader.GetAsset("audio://test")->GetType(); // 简化调用
                }
                ZG_CORE_INFO("Playing test audio");
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Stop Audio"))
            {
                if (m_CurrentPlayID != 0) {
                    // 停止音频播放
                    m_CurrentPlayID = 0;
                }
                ZG_CORE_INFO("Stopped audio");
            }
        }
        
        // 音频资源管理
        if (Zgine::IG::CollapsingHeader("Audio Resources"))
        {
            Zgine::IG::Text("Audio resource management features will be implemented here");
        }
    }

    void ResourceTestLayer::RenderStatisticsUI()
    {
        Zgine::IG::Text("Resource Statistics");
        Zgine::IG::Separator();
        
        if (Zgine::IG::BeginChild("StatsContent", ImVec2(0, 0), true))
        {
            Zgine::IG::TextUnformatted(m_LastStats.c_str());
        }
        Zgine::IG::EndChild();
        
        Zgine::IG::Separator();
        
        if (Zgine::IG::Button("Refresh Statistics"))
        {
            m_LastStats = m_AssetLoader.GetStatistics();
        }
        
        Zgine::IG::SameLine();
        if (Zgine::IG::Button("Cleanup Unused"))
        {
            uint32_t cleaned = m_AssetLoader.CleanupUnusedAssets();
            ZG_CORE_INFO("Cleaned up {} unused assets", cleaned);
        }
    }

    void ResourceTestLayer::LoadTestResources()
    {
        // 加载测试资源
        for (const auto& path : m_TestTexturePaths) {
            m_AssetLoader.LoadTexture(path);
        }
        
        for (const auto& path : m_TestShaderPaths) {
            m_AssetLoader.LoadShader(path);
        }
        
        for (const auto& path : m_TestModelPaths) {
            m_AssetLoader.LoadModel(path);
        }
        
        for (const auto& path : m_TestAudioPaths) {
            m_AssetLoader.LoadAudio(path);
        }
    }

    void ResourceTestLayer::UnloadTestResources()
    {
        // 卸载所有资源
        m_AssetLoader.UnloadAllAssets();
    }

    void ResourceTestLayer::ReloadTestResources()
    {
        // 重新加载资源
        UnloadTestResources();
        LoadTestResources();
    }

} // namespace Sandbox