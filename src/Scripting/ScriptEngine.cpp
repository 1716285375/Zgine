#include <Zgine/Scripting/ScriptEngine.h>

namespace Zgine {

// Static registry definition
std::unordered_map<std::string, ScriptEngine::FactoryFn> ScriptEngine::s_Registry;

Scope<IScript> ScriptEngine::Create(std::string_view name) {
    auto it = s_Registry.find(std::string(name));
    if (it == s_Registry.end()) return nullptr;
    return it->second();
}

bool ScriptEngine::Has(std::string_view name) {
    return s_Registry.contains(std::string(name));
}

void ScriptEngine::Unregister(std::string_view name) {
    s_Registry.erase(std::string(name));
}

void ScriptEngine::Clear() {
    s_Registry.clear();
}

} // namespace Zgine
