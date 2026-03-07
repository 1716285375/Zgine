#pragma once

#include <functional>
#include <memory>

namespace Zgine {

/**
 * @brief Command pattern for Editor MVVM UI actions
 *
 * Encapsulates UI actions with execute and canExecute logic.
 * Binds to UI buttons/menu items for clean separation of concerns.
 */
template<typename... Args>
class Command {
public:
    using ExecuteCallback   = std::function<void(Args...)>;
    using CanExecuteCallback = std::function<bool(Args...)>;

    Command() = default;

    explicit Command(ExecuteCallback execute)
        : m_Execute(std::move(execute))
        , m_CanExecute([](Args...) { return true; })
    {}

    Command(ExecuteCallback execute, CanExecuteCallback canExecute)
        : m_Execute(std::move(execute))
        , m_CanExecute(std::move(canExecute))
    {}

    void Execute(Args... args) {
        if (m_Execute && CanExecute(args...))
            m_Execute(std::forward<Args>(args)...);
    }

    [[nodiscard]] bool CanExecute(Args... args) const {
        return m_CanExecute ? m_CanExecute(std::forward<Args>(args)...) : true;
    }

    void SetExecute(ExecuteCallback execute)           { m_Execute    = std::move(execute); }
    void SetCanExecute(CanExecuteCallback canExecute)  { m_CanExecute = std::move(canExecute); }

    [[nodiscard]] explicit operator bool() const { return m_Execute != nullptr; }

private:
    ExecuteCallback    m_Execute;
    CanExecuteCallback m_CanExecute;
};

/** @brief Specialized command with no parameters (common case) */
using SimpleCommand = Command<>;

} // namespace Zgine
