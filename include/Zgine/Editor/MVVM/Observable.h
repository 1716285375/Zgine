#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <concepts>
#include <algorithm>

namespace Zgine {

/**
 * @brief Observable property for Editor MVVM data binding
 *
 * Notifies listeners when value changes, enabling reactive UI updates.
 * Used in ViewModel to create bindable properties.
 */
template<std::equality_comparable T>
class Observable {
public:
    using ChangeCallback = std::function<void(const T&)>;
    using CallbackHandle = size_t;

    Observable() = default;

    explicit Observable(const T& initialValue)
        : m_Value(initialValue)
    {}

    [[nodiscard]] const T& Get() const { return m_Value; }

    void Set(const T& newValue) {
        if (m_Value != newValue) {
            m_Value = newValue;
            NotifyListeners();
        }
    }

    void Set(T&& newValue) {
        if (m_Value != newValue) {
            m_Value = std::move(newValue);
            NotifyListeners();
        }
    }

    void SetForce(const T& newValue) {
        m_Value = newValue;
        NotifyListeners();
    }

    [[nodiscard]] CallbackHandle Subscribe(ChangeCallback callback) {
        m_Listeners.emplace_back(m_NextHandle, std::move(callback));
        return m_NextHandle++;
    }

    void Unsubscribe(CallbackHandle handle) {
        std::erase_if(m_Listeners,
            [handle](const auto& pair) { return pair.first == handle; });
    }

    operator const T&() const { return m_Value; }

    Observable& operator=(const T& newValue) {
        Set(newValue);
        return *this;
    }

private:
    void NotifyListeners() {
        for (auto& [handle, callback] : m_Listeners)
            callback(m_Value);
    }

    T m_Value{};
    std::vector<std::pair<CallbackHandle, ChangeCallback>> m_Listeners;
    CallbackHandle m_NextHandle = 0;
};

} // namespace Zgine
