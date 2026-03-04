#pragma once

#include <Zgine/Scene/Core/Entity.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

/**
 * @brief Script component property inspectors
 */
class ScriptInspector {
public:
    static void DrawNativeScriptProperties(Entity entity);
};

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
