#pragma once

#include <string>
#include <memory>

namespace Zgine {
    class Scene;
    class ISceneSerializer;

    /**
     * @brief Scene serializer facade
     * @brief 场景序列化器外观类
     *
     * Provides a simplified interface for scene serialization.
     * Internally delegates to JsonSceneSerializer (or other concrete implementations).
     * Maintains backwards compatibility with existing code.
     *
     * 提供场景序列化的简化接口。
     * 内部委托给 JsonSceneSerializer（或其他具体实现）。
     * 保持与现有代码的向后兼容性。
     */
    class SceneSerializer {
    public:
        SceneSerializer(Scene* scene);
        ~SceneSerializer();

        // 序列化场景到 JSON 字符串
        std::string Serialize() const;

        // 序列化场景到文件
        bool SerializeToFile(const std::string& filePath) const;

        // 从 JSON 字符串反序列化场景
        bool Deserialize(const std::string& jsonString);

        // 从文件反序列化场景
        bool DeserializeFromFile(const std::string& filePath);

    private:
        Scene* m_Scene = nullptr;
        std::unique_ptr<ISceneSerializer> m_Serializer; // Pimpl pattern
    };
}

