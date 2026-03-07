#pragma once

#include <cstdint>

namespace Zgine {

//=============================================================================
// Editor State Types
//=============================================================================

/**
 * @brief Editor play mode state
 *
 * Controls whether the editor is in edit mode, play mode, or paused.
 */
enum class EditorMode : uint8_t {
    Edit,   ///< Editor is in edit mode (World is editable)
    Play,   ///< Editor is in play mode (game is running)
    Pause   ///< Editor is paused (game logic frozen)
};

//=============================================================================
// Audio Types
//=============================================================================

/**
 * @brief Audio-related actions
 *
 * Used by audio components and systems to trigger audio operations.
 */
enum class AudioAction : uint8_t {
    Create,         ///< Create audio source
    Destroy,        ///< Destroy audio source
    Play,           ///< Start playing audio
    Stop,           ///< Stop playing audio
    Pause,          ///< Pause audio playback
    ApplySettings,  ///< Apply audio settings changes
    Recreate        ///< Recreate audio source (e.g., after format change)
};

//=============================================================================
// Gizmo & Transform Types
//=============================================================================

/**
 * @brief Gizmo operation mode
 *
 * Determines which transformation gizmo is active in the viewport.
 */
enum class GizmoMode : uint8_t {
    None,       ///< No gizmo active
    Translate,  ///< Translation gizmo (move)
    Rotate,     ///< Rotation gizmo
    Scale       ///< Scale gizmo
};

/**
 * @brief Coordinate space for transformations
 */
enum class TransformSpace : uint8_t {
    Local,  ///< Local/Object space
    World   ///< World/Global space
};

/**
 * @brief Grid snap mode
 */
enum class SnapMode : uint8_t {
    None,    ///< No snapping
    Grid,    ///< Snap to grid
    Vertex,  ///< Snap to vertices
    Surface  ///< Snap to surfaces
};

//=============================================================================
// Selection Types
//=============================================================================

/**
 * @brief Selection behavior mode
 */
enum class SelectionMode : uint8_t {
    Replace,   ///< Replace current selection
    Add,       ///< Add to selection (Ctrl+Click)
    Subtract,  ///< Remove from selection (Ctrl+Click on selected)
    Toggle     ///< Toggle selection state
};

//=============================================================================
// UI Layout Types
//=============================================================================

/**
 * @brief Panel dock position
 */
enum class DockPosition : uint8_t {
    None,
    Left,
    Right,
    Top,
    Bottom,
    Center,
    Floating
};

/**
 * @brief UI theme type
 */
enum class ThemeType : uint8_t {
    Dark,    ///< Dark theme (default)
    Light,   ///< Light theme
    Custom   ///< Custom user-defined theme
};

//=============================================================================
// Viewport Types
//=============================================================================

/**
 * @brief Viewport rendering mode
 */
enum class ViewportRenderMode : uint8_t {
    Lit,         ///< Fully lit with lighting
    Unlit,       ///< No lighting (texture only)
    Wireframe,   ///< Wireframe mode
    Normals,     ///< Show normals
    UVs          ///< Show UV coordinates
};

/**
 * @brief Camera projection type
 */
enum class CameraProjection : uint8_t {
    Perspective,  ///< Perspective projection
    Orthographic  ///< Orthographic projection
};

//=============================================================================
// Build & Export Types
//=============================================================================

/**
 * @brief Build target platform
 */
enum class BuildPlatform : uint8_t {
    Windows,
    Linux,
    MacOS,
    WebGL,
    Android,
    iOS
};

/**
 * @brief Build configuration
 */
enum class BuildConfiguration : uint8_t {
    Debug,     ///< Debug build with symbols
    Release,   ///< Optimized release build
    Profile    ///< Release with profiling enabled
};

//=============================================================================
// Project Types
//=============================================================================

/**
 * @brief Project template type
 */
enum class ProjectTemplate : uint8_t {
    Empty,      ///< Empty project
    Basic2D,    ///< 2D game template
    Basic3D,    ///< 3D game template
    FPS,        ///< First-person shooter template
    Platformer  ///< Platformer game template
};

} // namespace Zgine
