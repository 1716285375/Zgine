#pragma once

#include <string>
#include <memory>

namespace Zgine {

/**
 * @brief Base serializer interface
 * @brief 序列化器基础接口
 *
 * Defines the contract for all scene-level serializers.
 * Concrete implementations (JSON, Binary, XML, etc.) inherit from this.
 *
 * 定义所有场景级序列化器的契约。
 * 具体实现（JSON、二进制、XML等）继承此接口。
 *
 * @note This is not a template to allow runtime polymorphism
 */
class ISceneSerializer {
public:
    virtual ~ISceneSerializer() = default;

    /**
     * @brief Serialize scene to string format
     * @brief 将场景序列化为字符串格式
     * @param scene Scene to serialize
     * @return Serialized string
     */
    virtual std::string Serialize(class Scene* scene) const = 0;

    /**
     * @brief Deserialize scene from string format
     * @brief 从字符串格式反序列化场景
     * @param data Serialized data
     * @param scene Target scene to populate
     * @return true if successful, false otherwise
     */
    virtual bool Deserialize(const std::string& data, Scene* scene) = 0;

    /**
     * @brief Serialize scene to file
     * @brief 将场景序列化到文件
     * @param scene Scene to serialize
     * @param filePath Target file path
     * @return true if successful, false otherwise
     */
    virtual bool SerializeToFile(Scene* scene, const std::string& filePath) const = 0;

    /**
     * @brief Deserialize scene from file
     * @brief 从文件反序列化场景
     * @param filePath Source file path
     * @param scene Target scene to populate
     * @return true if successful, false otherwise
     */
    virtual bool DeserializeFromFile(const std::string& filePath, Scene* scene) = 0;

    /**
     * @brief Get the format name (e.g., "JSON", "Binary")
     * @brief 获取格式名称（例如"JSON"、"Binary"）
     */
    virtual std::string GetFormatName() const = 0;
};

/**
 * @brief Factory function type for creating serializers
 * @brief 创建序列化器的工厂函数类型
 */
using SerializerFactory = std::unique_ptr<ISceneSerializer>(*)();

} // namespace Zgine
