#pragma once

#include <Zgine/Scene/Core/Entity.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

/**
 * @brief Rendering component property inspectors
 *
 * Provides property editors for rendering-related components.
 */
class RenderingInspector {
public:
    /**
     * @brief Draw Sprite Renderer component properties
     * @param entity Entity with SpriteRendererComponent
     */
    static void DrawSpriteRendererProperties(Entity entity);

    /**
     * @brief Draw Camera component properties
     * @param entity Entity with CameraComponent
     */
    static void DrawCameraProperties(Entity entity);

    /**
     * @brief Draw Mesh Renderer component properties
     * @param entity Entity with MeshRendererComponent
     */
    static void DrawMeshRendererProperties(Entity entity);

    /**
     * @brief Draw Color component properties
     * @param entity Entity with ColorComponent
     */
    static void DrawColorProperties(Entity entity);

    /**
     * @brief Draw PBR Material component properties
     * @param entity Entity with PBRMaterialComponent
     */
    static void DrawPBRMaterialProperties(Entity entity);

    /**
     * @brief Draw Directional Light component properties
     * @param entity Entity with DirectionalLightComponent
     */
    static void DrawDirectionalLightProperties(Entity entity);

    /**
     * @brief Draw Point Light component properties
     * @param entity Entity with PointLightComponent
     */
    static void DrawPointLightProperties(Entity entity);

    /**
     * @brief Draw Spot Light component properties
     * @param entity Entity with SpotLightComponent
     */
    static void DrawSpotLightProperties(Entity entity);
};

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
