#pragma once

#include <Zgine/World/Serialization/IComponentSerializer.h>

namespace Zgine {

/**
 * @brief Serializer for TransformComponent
 */
class TransformSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Transform"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

/**
 * @brief Serializer for CameraComponent
 */
class CameraSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Camera"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

/**
 * @brief Serializer for PrimitiveComponent
 */
class PrimitiveSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Primitive"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

/**
 * @brief Serializer for PBRMaterialComponent
 */
class PBRMaterialSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "PBRMaterial"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

/**
 * @brief Serializer for all light components
 */
class LightSerializers {
public:
    class DirectionalLight : public IComponentSerializer {
    public:
        std::string_view GetComponentTypeName() const override { return "DirectionalLight"; }
        void Serialize(const Entity& entity, nlohmann::json& out) const override;
        bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
        bool HasComponent(const Entity& entity) const override;
    };

    class PointLight : public IComponentSerializer {
    public:
        std::string_view GetComponentTypeName() const override { return "PointLight"; }
        void Serialize(const Entity& entity, nlohmann::json& out) const override;
        bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
        bool HasComponent(const Entity& entity) const override;
    };

    class SpotLight : public IComponentSerializer {
    public:
        std::string_view GetComponentTypeName() const override { return "SpotLight"; }
        void Serialize(const Entity& entity, nlohmann::json& out) const override;
        bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
        bool HasComponent(const Entity& entity) const override;
    };
};

} // namespace Zgine
