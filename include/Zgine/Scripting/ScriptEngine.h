#pragma once

#include <Zgine/Scripting/IScript.h>
#include <Zgine/Core/Foundation/Types.h>
#include <string_view>
#include <unordered_map>
#include <memory>

namespace Zgine {

/**
 * @brief Manages the scripting runtime (currently: native C++ scripts).
 *
 * ScriptEngine is a lightweight registry that maps script names to factory
 * functions. It is intentionally simple — a Lua binding layer can be added
 * on top later without changing the interface here.
 *
 * Typical usage:
 *   // Registration (at startup):
 *   ScriptEngine::Register<PlayerController>("PlayerController");
 *
 *   // Instantiation (when loading a World):
 *   auto script = ScriptEngine::Create("PlayerController");
 *   script->OnStart();
 */
class ScriptEngine {
public:
    using FactoryFn = std::function<Scope<IScript>()>;

    // --- Registry ---

    /*
        Purpose : Register a script class under a given name.
        Notes   : T must satisfy ScriptConcept and derive from IScript.
    */
    template<typename T>
        requires ScriptConcept<T> && std::derived_from<T, IScript>
    static void Register(std::string_view name) {
        s_Registry[std::string(name)] = []() -> Scope<IScript> {
            return CreateScope<T>();
        };
    }

    /*
        Purpose : Instantiate a script by name; returns nullptr if not registered.
    */
    [[nodiscard]] static Scope<IScript> Create(std::string_view name);

    /*
        Purpose : Check whether a script name is registered.
    */
    [[nodiscard]] static bool Has(std::string_view name);

    /*
        Purpose : Remove a script registration.
    */
    static void Unregister(std::string_view name);

    /*
        Purpose : Remove all registrations (call at engine shutdown).
    */
    static void Clear();

private:
    static std::unordered_map<std::string, FactoryFn> s_Registry;
};

} // namespace Zgine
