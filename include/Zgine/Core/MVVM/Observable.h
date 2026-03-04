#pragma once

#include <functional>
#include <vector>
#include <memory>

namespace Zgine {

/**
 * @brief Observable property for MVVM data binding
 *
 * Notifies listeners when value changes, enabling reactive UI updates.
 * Used in ViewModel to create bindable properties.
 */
template<typename T>
class Observable {
public:
    using ChangeCallback = std::function<void(const T&)>;
    using CallbackHandle = size_t;

    Observable() = default;

    explicit Observable(const T& initialValue)
        : m_Value(initialValue)
    {}

    /**
     * @brief Get current value
     */
    const T& Get() const {
        return m_Value;
    }

    /**
     * @brief Set value and notify listeners if changed
     */
    void Set(const T& newValue) {
        if (m_Value != newValue) {
            m_Value = newValue;
            NotifyListeners();
        }
    }

    /**
     * @brief Set value without comparison (always notify)
     */
    void SetForce(const T& newValue) {
        m_Value = newValue;
        NotifyListeners();
    }

    /**
     * @brief Subscribe to value changes
     * @return Handle for unsubscribing
     */
    CallbackHandle Subscribe(ChangeCallback callback) {
        m_Listeners.emplace_back(m_NextHandle, std::move(callback));
        return m_NextHandle++;
    }

    /**
     * @brief Unsubscribe from value changes
     */
    void Unsubscribe(CallbackHandle handle) {
        m_Listeners.erase(
            std::remove_if(m_Listeners.begin(), m_Listeners.end(),
                [handle](const auto& pair) { return pair.first == handle; }),
            m_Listeners.end());
    }

    /**
     * @brief Implicit conversion to value
     */
    operator const T&() const {
        return m_Value;
    }

    /**
     * @brief Assignment operator
     */
    Observable& operator=(const T& newValue) {
        Set(newValue);
        return *this;
    }

private:
    void NotifyListeners() {
        for (auto& [handle, callback] : m_Listeners) {
            callback(m_Value);
        }
    }

    T m_Value{};
    std::vector<std::pair<CallbackHandle, ChangeCallback>> m_Listeners;
    CallbackHandle m_NextHandle = 0;
};

} // namespace Zgine
