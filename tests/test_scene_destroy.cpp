#include <iostream>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Core/Entity.h>

using namespace Zgine;

int main() {
    Scene scene;

    std::cout << "Initial count: " << scene.GetRegistry().storage<entt::entity>().size() << std::endl;

    Entity e1 = scene.CreateEntity("Test1");
    std::cout << "After create: " << scene.GetRegistry().storage<entt::entity>().size() << std::endl;
    std::cout << "Entity valid: " << (bool)e1 << std::endl;
    std::cout << "Entity handle: " << (uint32_t)static_cast<entt::entity>(e1) << std::endl;

    scene.DestroyEntity(e1);
    std::cout << "After destroy: " << scene.GetRegistry().storage<entt::entity>().size() << std::endl;
    std::cout << "Entity still valid (bool): " << (bool)e1 << std::endl;

    return 0;
}
