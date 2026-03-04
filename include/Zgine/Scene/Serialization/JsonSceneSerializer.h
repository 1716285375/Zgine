#pragma once

#include "ISceneSerializer.h"
#include "IComponentSerializer.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace Zgine {

/**
 * @brief JSON implementation of scene serializer
 * @brief 场景序列化器的JSON实现
 *
 * Uses nlohmann::json for serialization and delegates component
 * serialization to registered IComponentSerializer instances.
 *
 * 使用nlohmann::json进行序列化，并将组件序列化委托给
 * 注册的IComponentSerializer实例。
 */
class JsonSceneSerializer : public ISceneSerializer {
public:
    JsonSceneSerializer();
    virtual ~JsonSceneSerializer() = default;

    // ISceneSerializer interface
    std::string Serialize(Scene* scene) const override;
    bool Deserialize(const std::string& data, Scene* scene) override;
    bool SerializeToFile(Scene* scene, const std::string& filePath) const override;
    bool DeserializeFromFile(const std::string& filePath, Scene* scene) override;
    std::string GetFormatName() const override { return "JSON"; }

    /**
     * @brief Register a component serializer
     * @brief 注册组件序列化器
     * @param serializer Component serializer to register
     */
    void RegisterComponentSerializer(std::unique_ptr<IComponentSerializer> serializer);

    /**
     * @brief Get registered component serializers
     * @brief 获取已注册的组件序列化器
     */
    const std::vector<std::unique_ptr<IComponentSerializer>>& GetComponentSerializers() const {
        return m_ComponentSerializers;
    }

private:
    std::vector<std::unique_ptr<IComponentSerializer>> m_ComponentSerializers;
    std::unordered_map<std::string, IComponentSerializer*> m_SerializerMap; // For quick lookup

    static constexpr uint32_t kSceneVersion = 2;
};

} // namespace Zgine
