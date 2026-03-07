#pragma once

#include <string>
#include <uuid.h>
#include <ostream>
#include <compare>

namespace Zgine {

    enum class UUIDVersion {
        None = 0,
        TimeBased = 1,
        DceSecurity = 2,
        NameBasedMd5 = 3,
        RandomNumberBased = 4,
        NameBasedSha1 = 5
    };

    enum class UUIDVariant {
        Ncs,
        Rfc,
        Microsoft,
        Reserved
    };

    class UUID {
    public:
        UUID(); // Nil UUID
        UUID(const uuids::uuid& uuid);

        static UUID New();
        static UUID FromString(std::string_view value);
        static UUID FromString(const std::wstring& value);

        [[nodiscard]] const uuids::uuid& Raw() const { return m_Value; }

        [[nodiscard]] bool IsValid() const { return !IsNil(); }
        [[nodiscard]] bool IsNil() const { return m_Value.is_nil(); }

        [[nodiscard]] UUIDVersion GetVersion() const { return static_cast<UUIDVersion>(m_Value.version()); }
        [[nodiscard]] UUIDVariant GetVariant() const { return static_cast<UUIDVariant>(m_Value.variant()); }

        [[nodiscard]] std::string ToString() const;
        [[nodiscard]] std::wstring ToWString() const;

        [[nodiscard]] bool operator==(const UUID& other) const noexcept { return m_Value == other.m_Value; }
        [[nodiscard]] auto operator<=>(const UUID& other) const noexcept {
            auto a = m_Value.as_bytes();
            auto b = other.m_Value.as_bytes();
            return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
        }

        friend std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
            return os << uuid.ToString();
        }

    private:
        uuids::uuid m_Value;
    };

}

namespace std {
    template <>
    struct hash<Zgine::UUID> {
        size_t operator()(const Zgine::UUID& uuid) const noexcept;
    };
}
