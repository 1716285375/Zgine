#include <iostream>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>

using namespace Zgine;

int main() {
    World World;

    std::cout << "Initial count: " << World.GetEntityCount() << std::endl;

    Entity e1 = World.CreateEntity("Test1");
    std::cout << "After create: " << World.GetEntityCount() << std::endl;
    std::cout << "Entity valid: " << (bool)e1 << std::endl;
    std::cout << "Entity handle: " << e1.GetHandle().GetValue() << std::endl;

    World.DestroyEntity(e1);
    std::cout << "After destroy: " << World.GetEntityCount() << std::endl;
    std::cout << "Entity still valid (bool): " << (bool)e1 << std::endl;

    return 0;
}
