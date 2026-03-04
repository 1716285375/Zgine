#pragma once

#include <string>

namespace Zgine {

class Scene;

/**
 * @brief Interface for all game systems
 *
 * Systems process ECS components and manage game logic in a procedural manner.
 * Each system has a priority to control update order and can have fixed/variable update rates.
 */
class ISystem {
public:
    virtual ~ISystem() = default;

    /**
     * @brief Initialize the system
     * Called once when the system is registered or scene starts
     */
    virtual void Initialize() = 0;

    /**
     * @brief Shutdown the system
     * Called once when the system is removed or scene stops
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Update the system with variable delta time
     * @param scene Scene containing entities to process
     * @param deltaTime Time elapsed since last frame
     */
    virtual void Update(Scene* scene, float deltaTime) = 0;

    /**
     * @brief Fixed timestep update (optional)
     * Used for physics and other systems requiring deterministic updates
     * @param scene Scene containing entities to process
     * @param fixedDeltaTime Fixed time step (e.g., 1/60 second)
     */
    virtual void FixedUpdate(Scene* scene, float fixedDeltaTime) {
        (void)scene;
        (void)fixedDeltaTime;
    }

    /**
     * @brief Get system name for debugging
     */
    virtual const char* GetName() const = 0;

    /**
     * @brief Get system priority (lower values execute first)
     *
     * Common priorities:
     * - 0-9: Input systems
     * - 10-19: Physics systems
     * - 20-29: Audio systems
     * - 30-39: Script systems
     * - 40-49: Animation systems
     * - 50+: Rendering systems
     */
    virtual int GetPriority() const { return 100; }

    /**
     * @brief Whether the system is enabled
     */
    virtual bool IsEnabled() const { return m_Enabled; }

    /**
     * @brief Set system enabled state
     */
    virtual void SetEnabled(bool enabled) { m_Enabled = enabled; }

protected:
    bool m_Enabled = true;
};

} // namespace Zgine
