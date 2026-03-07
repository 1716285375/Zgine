#pragma once

#include <Zgine/World/Core/Entity.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

/**
 * @brief Audio component property inspectors
 */
class AudioInspector {
public:
    static void DrawAudioSourceProperties(Entity entity);
    static void DrawAudioListenerProperties(Entity entity);
};

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
