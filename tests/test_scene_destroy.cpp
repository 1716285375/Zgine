#include <iostream>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>

using namespace Zgine;

int main() {
    World World;

    std::cout << "Initial count: " << World.GetRegistry().storage<entt::entity>().size() << std::endl;

    Entity e1 = World.CreateEntity("Test1");
    std::cout << "After create: " << World.GetRegistry().storage<entt::entity>().size() << std::endl;
    std::cout << "Entity valid: " << (bool)e1 << std::endl;
    std::cout << "Entity handle: " << (uint32_t)static_cast<entt::entity>(e1) << std::endl;

    World.DestroyEntity(e1);
    std::cout << "After destroy: " << World.GetRegistry().storage<entt::entity>().size() << std::endl;
    std::cout << "Entity still valid (bool): " << (bool)e1 << std::endl;

    return 0;
}
