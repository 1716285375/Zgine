#pragma once

#include <Zgine/Core/Foundation/Prerequisites.h>
#include <string>
#include <functional>
#include <imgui.h>

namespace Zgine {

class EditorPanelManager;
class EditorContext;

/**
 * @brief Base class for all editor panels
 *
 * EditorPanel provides a common interface for all panels in the editor,
 * handling window management, docking behavior, visibility, and lifecycle.
 *
 * **Lifecycle**:
 * 1. Construction
 * 2. OnAttach() - Called when panel is first registered
 * 3. OnUpdate(dt) - Called every frame (optional)
 * 4. OnGuiRender() - Called every frame to render UI (required)
 * 5. OnDetach() - Called when panel is removed
 * 6. Destruction
 *
 * **Features**:
 * - Automatic window begin/end handling
 * - Focus and hover detection
 * - Docking state tracking
 * - Closable/non-closable panels
 * - Floatable/non-floatable panels
 * - Visibility toggle
 *
 * **Usage**:
 * @code
 * class MyPanel : public EditorPanel {
 * public:
 *     MyPanel(EditorContext& ctx) : EditorPanel("My Panel", ctx) {}
 *
 *     void OnGuiRender() override {
 *         BeginPanel();
 *         ImGui::Text("Hello Panel!");
 *         EndPanel();
 *     }
 * };
 * @endcode
 */
class EditorPanel {
    friend class EditorPanelManager;

public:
    /**
     * @brief Construct a new Editor Panel
     * @param name Display name of the panel
     * @param context Reference to editor context for accessing services
     * @param openByDefault Whether the panel starts visible
     */
    EditorPanel(const std::string& name, EditorContext& context, bool openByDefault = true);

    virtual ~EditorPanel();

    ZGINE_NON_COPYABLE_NON_MOVABLE(EditorPanel)

    // ========================================================================
    // Lifecycle Hooks
    // ========================================================================

    /**
     * @brief Called when panel is attached to the editor
     * Override to perform initialization (e.g., loading resources)
     */
    virtual void OnAttach() {}

    /**
     * @brief Called when panel is detached from the editor
     * Override to perform cleanup (e.g., releasing resources)
     */
    virtual void OnDetach() {}

    /**
     * @brief Called every frame before rendering
     * @param deltaTime Time since last frame in seconds
     * Override for frame-based logic that doesn't involve ImGui
     */
    virtual void OnUpdate(float deltaTime);

    /**
     * @brief Called every frame to render the panel's UI
     * Must be implemented by derived classes
     * Call BeginPanel() and EndPanel() to wrap your ImGui code
     */
    virtual void OnGuiRender() = 0;

    // ========================================================================
    // Panel Properties
    // ========================================================================

    /** Get the panel's display name */
    const std::string& GetName() const;

    /** Check if panel is currently visible */
    bool IsOpen() const;

    /** Set panel visibility (respects closable state) */
    void SetOpen(bool open);

    /** Toggle panel visibility */
    void ToggleOpen() { SetOpen(!m_IsOpen); }

    // ========================================================================
    // Window State Queries
    // ========================================================================

    /** Check if panel window is currently focused */
    bool IsFocused() const { return m_IsFocused; }

    /** Check if panel window is currently hovered */
    bool IsHovered() const { return m_IsHovered; }

    /** Check if panel is docked (not floating) */
    bool IsDocked() const;

    /** Get ImGui window ID (0 if not rendered yet) */
    ImGuiID GetWindowID() const;

    // ========================================================================
    // Panel Behavior Configuration
    // ========================================================================

    /** Set whether panel can be closed by user */
    void SetClosable(bool closable);

    /** Check if panel can be closed */
    bool IsClosable() const;

    /** Set whether panel can be undocked/floated */
    void SetFloatable(bool floatable);

    /** Check if panel can float */
    bool IsFloatable() const;

    /**
     * @brief Set custom icon for panel tab (e.g., ICON_FA_CUBE)
     * Requires FontAwesome or similar icon font loaded
     */
    void SetIcon(const std::string& icon) { m_Icon = icon; }

    /** Get panel icon */
    const std::string& GetIcon() const { return m_Icon; }

    /**
     * @brief Set minimum size for panel window
     * @param width Minimum width (0 = no constraint)
     * @param height Minimum height (0 = no constraint)
     */
    void SetMinSize(float width, float height);

    /** Get minimum width */
    float GetMinWidth() const { return m_MinWidth; }

    /** Get minimum height */
    float GetMinHeight() const { return m_MinHeight; }

protected:
    // ========================================================================
    // Protected Helpers for Derived Classes
    // ========================================================================

    /**
     * @brief Begin ImGui window for this panel
     * @param flags Additional ImGui window flags to apply
     * Call this at the start of OnGuiRender()
     */
    void BeginPanel(ImGuiWindowFlags flags = 0);

    /**
     * @brief End ImGui window for this panel
     * Call this at the end of OnGuiRender()
     */
    void EndPanel();

    /**
     * @brief Show tooltip explaining why panel can't be closed
     * Useful when implementing custom close logic
     */
    void ShowCannotCloseTooltip() const;

    /**
     * @brief Access to editor context for services
     * Use to get selected entity, command history, etc.
     */
    EditorContext& GetContext() { return m_Context; }
    const EditorContext& GetContext() const { return m_Context; }

    /**
     * @brief Get current window content region size
     * Only valid between BeginPanel() and EndPanel()
     */
    ImVec2 GetContentRegionAvail() const;

    /**
     * @brief Mark panel as dirty/needing refresh
     * Some panels may cache data and use this to trigger refresh
     */
    void MarkDirty() { m_IsDirty = true; }

    /** Check if panel is dirty */
    bool IsDirty() const { return m_IsDirty; }

    /** Clear dirty flag */
    void ClearDirty() { m_IsDirty = false; }

private:
    // Core properties
    std::string m_Name;
    std::string m_Icon;
    bool m_IsOpen;

    // Window state (updated by BeginPanel/EndPanel)
    bool m_IsFocused;
    bool m_IsHovered;
    bool m_IsDocked;
    mutable ImGuiID m_WindowID;

    // Behavior flags
    bool m_IsClosable;
    bool m_CanFloat;

    // Size constraints
    float m_MinWidth = 0.0f;
    float m_MinHeight = 0.0f;

    // Dirty flag for refresh logic
    bool m_IsDirty = false;

    // Editor context reference
    EditorContext& m_Context;
};

} // namespace Zgine
