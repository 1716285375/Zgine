#pragma once

#include <concepts>
#include <string_view>

namespace Zgine {

/**
 * @brief C++20 concept: a type is a valid Engine Script if it provides
 *        OnStart, OnUpdate, and OnDestroy.
 *
 * This lets the engine validate script classes at compile time rather than
 * at runtime via virtual dispatch. Use ScriptBase (below) for a conventional
 * virtual-dispatch binding approach.
 */
template<typename T>
concept ScriptConcept = requires(T t, float dt) {
    { t.OnStart()         } -> std::same_as<void>;
    { t.OnUpdate(dt)      } -> std::same_as<void>;
    { t.OnDestroy()       } -> std::same_as<void>;
};

// -------------------------------------------------------------------------

/**
 * @brief Optional CRTP / virtual base for script classes bound to entities.
 *
 * Inherit ScriptBase and override the lifecycle callbacks.
 * The ScriptEngine manages ownership through IScript pointers.
 *
 * Example:
 *   class PlayerController : public ScriptBase {
 *   public:
 *       void OnUpdate(float dt) override { ... }
 *   };
 */
class IScript {
public:
    virtual ~IScript() = default;

    /*
        Purpose : Called once when the script is first attached to an entity.
    */
    virtual void OnStart() {}

    /*
        Purpose : Called every frame; implement per-frame game logic here.
        Args    : deltaTime — seconds since last frame.
    */
    virtual void OnUpdate(float deltaTime) = 0;

    /*
        Purpose : Called once just before the script is detached or destroyed.
    */
    virtual void OnDestroy() {}

    /*
        Purpose : Human-readable name for editor display and Lua binding.
    */
    [[nodiscard]] virtual std::string_view GetName() const = 0;
};

// Verify that IScript itself satisfies the concept (sanity check)
// Note: IScript::OnStart is not pure, so we use a concrete helper:
// static_assert(ScriptConcept<ConcreteScript>); // done per subclass

} // namespace Zgine
