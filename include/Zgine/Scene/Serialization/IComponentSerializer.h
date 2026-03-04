#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

namespace Zgine {

class Entity;

/**
 * @brief Component-level serialization interface
 * @brief 组件级序列化接口
 *
 * Each component type should have its own serializer implementing this interface.
 * This allows modular, extensible component serialization.
 *
 * 每个组件类型都应该有自己的序列化器实现此接口。
 * 这允许模块化、可扩展的组件序列化。
 */
class IComponentSerializer {
public:
    virtual ~IComponentSerializer() = default;

    /**
     * @brief Get the component type name for identification
     * @brief 获取组件类型名称用于识别
     * @return Component type name (e.g., "Transform", "MeshComponent")
     */
    virtual std::string GetComponentTypeName() const = 0;

    /**
     * @brief Serialize component to JSON
     * @brief 将组件序列化为JSON
     * @param entity Entity containing the component
     * @param out Output JSON object
     */
    virtual void Serialize(const Entity& entity, nlohmann::json& out) const = 0;

    /**
     * @brief Deserialize component from JSON
     * @brief 从JSON反序列化组件
     * @param data Input JSON object
     * @param entity Target entity to add component to
     * @return true if successful, false otherwise
     */
    virtual bool Deserialize(const nlohmann::json& data, Entity& entity) const = 0;

    /**
     * @brief Check if entity has this component
     * @brief 检查实体是否有此组件
     * @param entity Entity to check
     * @return true if entity has the component
     */
    virtual bool HasComponent(const Entity& entity) const = 0;
};

} // namespace Zgine
