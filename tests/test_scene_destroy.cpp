#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>

using namespace Zgine;

int main() {
    Log::Init();

    World World;

    ZGINE_CORE_INFO("Initial count: {}", World.GetEntityCount());

    Entity e1 = World.CreateEntity("Test1");
    ZGINE_CORE_INFO("After create: {}", World.GetEntityCount());
    ZGINE_CORE_INFO("Entity valid: {}", static_cast<bool>(e1));
    ZGINE_CORE_INFO("Entity handle: {}", e1.GetHandle().GetValue());

    World.DestroyEntity(e1);
    ZGINE_CORE_INFO("After destroy: {}", World.GetEntityCount());
    ZGINE_CORE_INFO("Entity still valid (bool): {}", static_cast<bool>(e1));

    Log::Shutdown();
    return 0;
}
