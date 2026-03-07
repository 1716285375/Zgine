#include <Zgine/Core/UUID/UUID.h>
#include <random>
#include <algorithm>

namespace Zgine {

    static uuids::uuid_random_generator CreateGenerator() {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        static std::mt19937 generator(seq);
        return uuids::uuid_random_generator{generator};
    }

    static uuids::uuid_random_generator& GetGenerator() {
        static uuids::uuid_random_generator gen = CreateGenerator();
        return gen;
    }

    UUID::UUID() {
        // Default to nil UUID by stduuid's default constructor
    }

    UUID::UUID(const uuids::uuid& uuid) : m_Value(uuid) {
    }

    UUID UUID::New() {
        return UUID(GetGenerator()());
    }

    UUID UUID::FromString(std::string_view value) {
        auto id = uuids::uuid::from_string(value);
        if (id.has_value()) {
            return UUID(id.value());
        }
        return UUID();
    }

    UUID UUID::FromString(const std::wstring& value) {
        auto id = uuids::uuid::from_string(value);
        if (id.has_value()) {
            return UUID(id.value());
        }
        return UUID();
    }

    std::string UUID::ToString() const {
        return uuids::to_string(m_Value);
    }

    std::wstring UUID::ToWString() const {
        return uuids::to_string<wchar_t>(m_Value);
    }

}

namespace std {
    size_t hash<Zgine::UUID>::operator()(const Zgine::UUID& uuid) const noexcept {
        return std::hash<uuids::uuid>{}(uuid.Raw());
    }
}
