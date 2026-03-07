#pragma once

#include <string>
#include <string_view>
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

    /*
        Purpose : Return the component type name used for JSON key identification.
        Return  : String view into a stable identifier (e.g. "Transform", "MeshComponent").
    */
    [[nodiscard]] virtual std::string_view GetComponentTypeName() const = 0;

    /*
        Purpose : Serialize this component from the given entity into a JSON object.
        Args    : entity — source entity; out — output JSON node to populate.
    */
    virtual void Serialize(const Entity& entity, nlohmann::json& out) const = 0;

    /*
        Purpose : Deserialize component data from JSON and attach it to the target entity.
        Args    : data — source JSON node; entity — entity to receive the component.
        Return  : true on success, false on failure.
    */
    [[nodiscard]] virtual bool Deserialize(const nlohmann::json& data, Entity& entity) const = 0;

    /*
        Purpose : Check whether the given entity currently has this component.
        Return  : true if the component is present.
    */
    [[nodiscard]] virtual bool HasComponent(const Entity& entity) const = 0;
};

} // namespace Zgine
