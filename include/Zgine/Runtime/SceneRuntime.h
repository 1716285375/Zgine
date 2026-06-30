#pragma once

#include <functional>
#include <memory>

namespace Zgine {

class World;

class SceneRuntime {
public:
    SceneRuntime() = default;
    ~SceneRuntime();

    SceneRuntime(const SceneRuntime&) = delete;
    SceneRuntime& operator=(const SceneRuntime&) = delete;

    bool StartFrom(const World& editWorld);
    bool StartFrom(const World& editWorld, const std::function<void(World&)>& configureRuntimeWorld);
    bool Start(std::unique_ptr<World> runtimeWorld);
    void Stop();

    void Update(float deltaTime);
    void FixedUpdate(float fixedDeltaTime);

    [[nodiscard]] bool IsRunning() const noexcept { return m_Running; }
    [[nodiscard]] World* GetWorld() noexcept { return m_World.get(); }
    [[nodiscard]] const World* GetWorld() const noexcept { return m_World.get(); }

private:
    std::unique_ptr<World> m_World;
    bool m_Running = false;
};

} // namespace Zgine
