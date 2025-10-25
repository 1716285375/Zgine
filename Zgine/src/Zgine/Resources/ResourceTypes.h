#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Zgine {
namespace Resources {

    /**
     * @brief 资源类型枚举
     */
    enum class ResourceType {
        Unknown = 0,
        Texture,
        Shader,
        Model,
        Audio,
        Font,
        Material,
        Animation,
        Scene,
        Script,
        Config,
        Data
    };

    /**
     * @brief 资源状态枚举
     */
    enum class ResourceState {
        Unloaded = 0,
        Loading,
        Loaded,
        Failed,
        Error,
        Unloading
    };

    /**
     * @brief 资源加载优先级
     */
    enum class ResourcePriority {
        Low = 0,
        Normal,
        High,
        Critical
    };

    /**
     * @brief 资源加载模式
     */
    enum class ResourceLoadMode {
        Sync = 0,    // 同步加载
        Async,       // 异步加载
        Streaming    // 流式加载
    };

    /**
     * @brief 资源缓存策略
     */
    enum class ResourceCachePolicy {
        KeepInMemory = 0,  // 保持在内存中
        UnloadWhenUnused,  // 未使用时卸载
        NeverCache         // 从不缓存
    };

    /**
     * @brief 资源统计信息
     */
    struct ResourceStatistics {
        uint32_t totalResources = 0;
        uint32_t loadedResources = 0;
        uint32_t failedResources = 0;
        uint32_t cachedResources = 0;
        size_t totalMemoryUsage = 0;
        uint32_t totalLoadTime = 0;
        uint32_t averageLoadTime = 0;
    };

    /**
     * @brief 资源加载配置
     */
    struct ResourceLoadConfig {
        ResourceLoadMode loadMode = ResourceLoadMode::Async;
        ResourcePriority priority = ResourcePriority::Normal;
        ResourceCachePolicy cachePolicy = ResourceCachePolicy::KeepInMemory;
        bool validateOnLoad = true;
        bool retryOnFailure = true;
        uint32_t maxRetryCount = 3;
        uint32_t timeoutMs = 5000;
    };

} // namespace Resources
} // namespace Zgine
