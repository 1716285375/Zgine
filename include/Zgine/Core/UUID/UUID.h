#pragma once

#include <string>
#include <uuid.h>
#include <ostream>

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
        static UUID FromString(const std::string& value);
        static UUID FromString(const std::wstring& value);

        const uuids::uuid& Raw() const { return m_Value; }
        
        bool IsValid() const { return !IsNil(); }
        bool IsNil() const { return m_Value.is_nil(); }
        
        UUIDVersion GetVersion() const { return static_cast<UUIDVersion>(m_Value.version()); }
        UUIDVariant GetVariant() const { return static_cast<UUIDVariant>(m_Value.variant()); }

        std::string ToString() const;
        std::wstring ToWString() const;

        bool operator==(const UUID& other) const { return m_Value == other.m_Value; }
        bool operator!=(const UUID& other) const { return m_Value != other.m_Value; }
        bool operator<(const UUID& other) const { return m_Value < other.m_Value; }

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
