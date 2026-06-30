#pragma once

#include <string>

namespace Zgine {

class World;

/**
 * @brief Interface for all game systems
 *
 * Systems process ECS components and manage game logic in a procedural manner.
 * Each system has a priority to control update order and can have fixed/variable update rates.
 */
class ISystem {
public:
    virtual ~ISystem() = default;

    /*
        Purpose : Initialize the system; called once when registered or World starts.
    */
    virtual void Initialize() = 0;

    /*
        Purpose : Shutdown and clean up the system; called once when removed or World stops.
    */
    virtual void Shutdown() = 0;

    /*
        Purpose : Bind the system to a runtime scene. This is separate from
                  Initialize() so resource lifetime and Play Mode lifetime stay clear.
    */
    virtual void OnSceneStart(World* World) { (void)World; }

    /*
        Purpose : Release scene-bound runtime state without destroying engine resources.
    */
    virtual void OnSceneStop() {}

    /*
        Purpose : Variable-timestep update — process entities each frame.
        Args    : World — World containing entities; deltaTime — seconds since last frame.
    */
    virtual void Update(World* World, float deltaTime) = 0;

    /**
     * @brief Fixed timestep update (optional)
     * Used for physics and other systems requiring deterministic updates
     * @param World World containing entities to process
     * @param fixedDeltaTime Fixed time step (e.g., 1/60 second)
     */
    virtual void FixedUpdate(World* World, float fixedDeltaTime) {
        (void)World;
        (void)fixedDeltaTime;
    }

    /*
        Purpose : Get the system's human-readable name for debugging.
        Return  : Null-terminated string (stable lifetime).
    */
    [[nodiscard]] virtual const char* GetName() const = 0;

    /*
        Purpose : Get update priority; lower values execute first.
        Notes   : 0–9 Input, 10–19 Physics, 20–29 Audio, 30–39 Script, 50+ Rendering.
        Return  : Priority integer.
    */
    [[nodiscard]] virtual int GetPriority() const { return 100; }

    /*
        Purpose : Check whether the system is currently active.
    */
    [[nodiscard]] virtual bool IsEnabled() const { return m_Enabled; }

    /**
     * @brief Set system enabled state
     */
    virtual void SetEnabled(bool enabled) { m_Enabled = enabled; }

protected:
    bool m_Enabled = true;
};

} // namespace Zgine
