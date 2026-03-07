#pragma once

#include <string>
#include <memory>

namespace Zgine {
    class World;
    class IWorldSerializer;

    /**
     * @brief World serializer facade
     * @brief 场景序列化器外观类
     *
     * Provides a simplified interface for World serialization.
     * Internally delegates to JsonWorldSerializer (or other concrete implementations).
     * Maintains backwards compatibility with existing code.
     *
     * 提供场景序列化的简化接口。
     * 内部委托给 JsonWorldSerializer（或其他具体实现）。
     * 保持与现有代码的向后兼容性。
     */
    class WorldSerializer {
    public:
        WorldSerializer(World* World);
        ~WorldSerializer();

        // 序列化场景到 JSON 字符串
        std::string Serialize() const;

        // 序列化场景到文件
        bool SerializeToFile(const std::string& filePath) const;

        // 从 JSON 字符串反序列化场景
        bool Deserialize(const std::string& jsonString);

        // 从文件反序列化场景
        bool DeserializeFromFile(const std::string& filePath);

    private:
        World* m_Scene = nullptr;
        std::unique_ptr<IWorldSerializer> m_Serializer; // Pimpl pattern
    };
}

