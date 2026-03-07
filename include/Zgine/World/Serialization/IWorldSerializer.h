#pragma once

#include <string>
#include <string_view>
#include <memory>

namespace Zgine {

/**
 * @brief Base serializer interface
 * @brief 序列化器基础接口
 *
 * Defines the contract for all World-level serializers.
 * Concrete implementations (JSON, Binary, XML, etc.) inherit from this.
 *
 * 定义所有场景级序列化器的契约。
 * 具体实现（JSON、二进制、XML等）继承此接口。
 *
 * @note This is not a template to allow runtime polymorphism
 */
class IWorldSerializer {
public:
    virtual ~IWorldSerializer() = default;

    /*
        Purpose : Serialize the World to an in-memory string.
        Args    : World — World to serialize.
        Return  : Serialized string representation.
    */
    [[nodiscard]] virtual std::string Serialize(class World* World) const = 0;

    /*
        Purpose : Deserialize World data from a string into an existing World object.
        Args    : data — serialized text; World — target World to populate.
        Return  : true on success.
    */
    virtual bool Deserialize(std::string_view data, World* World) = 0;

    /*
        Purpose : Serialize the World directly to a file on disk.
        Args    : World — source World; filePath — destination path.
        Return  : true on success.
    */
    [[nodiscard]] virtual bool SerializeToFile(World* World, std::string_view filePath) const = 0;

    /*
        Purpose : Deserialize World data read from a file into an existing World object.
        Args    : filePath — source file path; World — target World.
        Return  : true on success.
    */
    virtual bool DeserializeFromFile(std::string_view filePath, World* World) = 0;

    /*
        Purpose : Return the human-readable format identifier for this serializer.
        Return  : Format name, e.g. "JSON" or "Binary".
    */
    [[nodiscard]] virtual std::string GetFormatName() const = 0;
};

/**
 * @brief Factory function type for creating serializers
 * @brief 创建序列化器的工厂函数类型
 */
using SerializerFactory = std::unique_ptr<IWorldSerializer>(*)();

} // namespace Zgine
