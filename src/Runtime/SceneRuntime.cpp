#include <Zgine/Runtime/SceneRuntime.h>
#include <Zgine/World/Core/World.h>

#include <utility>

namespace Zgine {

SceneRuntime::~SceneRuntime() {
    Stop();
}

bool SceneRuntime::StartFrom(const World& editWorld) {
    return StartFrom(editWorld, {});
}

bool SceneRuntime::StartFrom(const World& editWorld, const std::function<void(World&)>& configureRuntimeWorld) {
    if (m_Running) {
        return true;
    }

    auto runtimeWorld = editWorld.CloneForRuntime();
    if (!runtimeWorld) {
        return false;
    }

    if (configureRuntimeWorld) {
        configureRuntimeWorld(*runtimeWorld);
    }

    return Start(std::move(runtimeWorld));
}

bool SceneRuntime::Start(std::unique_ptr<World> runtimeWorld) {
    if (m_Running) {
        return true;
    }

    if (!runtimeWorld) {
        return false;
    }

    m_World = std::move(runtimeWorld);
    m_World->GetSystemManager().StartScene(m_World.get());
    m_Running = true;
    return true;
}

void SceneRuntime::Stop() {
    if (!m_World) {
        m_Running = false;
        return;
    }

    if (m_Running) {
        m_World->GetSystemManager().StopScene();
    }

    m_World.reset();
    m_Running = false;
}

void SceneRuntime::Update(float deltaTime) {
    if (m_Running && m_World) {
        m_World->GetSystemManager().UpdateAll(m_World.get(), deltaTime);
    }
}

void SceneRuntime::FixedUpdate(float fixedDeltaTime) {
    if (m_Running && m_World) {
        m_World->GetSystemManager().FixedUpdateAll(m_World.get(), fixedDeltaTime);
    }
}

} // namespace Zgine
