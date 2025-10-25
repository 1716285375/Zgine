#include "sandbox_pch.h"
#include "ECSTestLayer.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Renderer/BatchRenderer2D.h"
#include "Zgine/Renderer/OrthographicCamera.h"
#include "Zgine/ECS/ECSSerialization.h"
#include "Zgine/ImGui/ImGuiWrapper.h"
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>

namespace Sandbox {

ECSTestLayer::ECSTestLayer()
    : ImGuiLayer()
{
    ZG_CORE_INFO("ECS Test Layer created");
}

void ECSTestLayer::OnAttach()
{
    ZG_CORE_INFO("ECS Test Layer attached");
    CreateTestEntities();
}

void ECSTestLayer::OnDetach()
{
    ZG_CORE_INFO("ECS Test Layer detached");
}

void ECSTestLayer::OnUpdate(Zgine::Timestep ts)
{
    if (m_EnableMovement)
    {
        UpdateEntities(ts);
    }
    
    // Always render entities, regardless of movement state
    RenderEntities();
    
    m_LastUpdateTime = ts;
}

void ECSTestLayer::RenderCustomUI()
{
    // ZG_CORE_INFO("ECSTestLayer::RenderCustomUI called - m_ShowECSWindow: {}", m_ShowECSWindow);
    
    if (m_ShowECSWindow)
    {
        if (Zgine::IG::Begin("ECS Test Window", &m_ShowECSWindow))
        {
            // ZG_CORE_INFO("ECS Test Window is open and rendering");
            
            // ECS Statistics
            Zgine::IG::Text("ECS Statistics:");
            Zgine::IG::Separator();
            Zgine::IG::Text("Entity Count: %zu", m_ECSManager.GetEntityCount());
            Zgine::IG::Text("Total Components: %zu", m_ECSManager.GetComponentCount());
            Zgine::IG::Text("Last Update Time: %.3f ms", m_LastUpdateTime * 1000.0f);
            
            // Component Statistics
            if (Zgine::IG::CollapsingHeader("Component Statistics"))
            {
                Zgine::IG::Text("Position: %zu", m_ECSManager.GetComponentCount("Position"));
                Zgine::IG::Text("Velocity: %zu", m_ECSManager.GetComponentCount("Velocity"));
                Zgine::IG::Text("Renderable: %zu", m_ECSManager.GetComponentCount("Renderable"));
                Zgine::IG::Text("Transform: %zu", m_ECSManager.GetComponentCount("Transform"));
                Zgine::IG::Text("Sprite: %zu", m_ECSManager.GetComponentCount("Sprite"));
                Zgine::IG::Text("Animation: %zu", m_ECSManager.GetComponentCount("Animation"));
                Zgine::IG::Text("Physics: %zu", m_ECSManager.GetComponentCount("Physics"));
                Zgine::IG::Text("Health: %zu", m_ECSManager.GetComponentCount("Health"));
                Zgine::IG::Text("Tag: %zu", m_ECSManager.GetComponentCount("Tag"));
            }
            
            Zgine::IG::Separator();
            
            // Controls
            Zgine::IG::Checkbox("Enable Movement", &m_EnableMovement);
            Zgine::IG::SliderFloat("Movement Speed", &m_MovementSpeed, 0.1f, 10.0f);
            
            Zgine::IG::Separator();
            
            // Entity Information
            if (Zgine::IG::CollapsingHeader("Entity Information"))
            {
                for (size_t i = 0; i < m_Entities.size(); ++i)
                {
                    auto& entity = m_Entities[i];
                    if (!entity.IsValid()) continue;
                    
                    std::string entityInfo = "Entity " + std::to_string(i + 1) + " (ID: " + std::to_string(entity.GetID()) + ")";
                    
                    // Legacy Position/Renderable components
                    if (entity.HasComponent<Zgine::ECS::Position>() && entity.HasComponent<Zgine::ECS::Renderable>())
                    {
                        auto& pos = entity.GetComponent<Zgine::ECS::Position>();
                        auto& renderable = entity.GetComponent<Zgine::ECS::Renderable>();
                        entityInfo += " - Pos: (" + std::to_string(pos.position.x) + ", " + std::to_string(pos.position.y) + ", " + std::to_string(pos.position.z) + ")";
                        entityInfo += " - Color: (" + std::to_string(renderable.color.r) + ", " + std::to_string(renderable.color.g) + ", " + std::to_string(renderable.color.b) + ")";
                        entityInfo += " - Scale: " + std::to_string(renderable.scale);
                    }
                    // New Transform/Sprite components
                    else if (entity.HasComponent<Zgine::ECS::Transform>() && entity.HasComponent<Zgine::ECS::Sprite>())
                    {
                        auto& transform = entity.GetComponent<Zgine::ECS::Transform>();
                        auto& sprite = entity.GetComponent<Zgine::ECS::Sprite>();
                        entityInfo += " - Transform: (" + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y) + ", " + std::to_string(transform.position.z) + ")";
                        entityInfo += " - Sprite: (" + std::to_string(sprite.color.r) + ", " + std::to_string(sprite.color.g) + ", " + std::to_string(sprite.color.b) + ")";
                        entityInfo += " - Size: (" + std::to_string(sprite.size.x) + ", " + std::to_string(sprite.size.y) + ")";
                        
                        // Additional component info
                        if (entity.HasComponent<Zgine::ECS::Physics>())
                        {
                            auto& physics = entity.GetComponent<Zgine::ECS::Physics>();
                            entityInfo += " - Physics: Vel(" + std::to_string(physics.velocity.x) + ", " + std::to_string(physics.velocity.y) + ", " + std::to_string(physics.velocity.z) + ")";
                        }
                        if (entity.HasComponent<Zgine::ECS::Animation>())
                        {
                            auto& anim = entity.GetComponent<Zgine::ECS::Animation>();
                            entityInfo += " - Animation: " + std::string(anim.playing ? "Playing" : "Stopped") + " (" + std::to_string(anim.currentTime) + "/" + std::to_string(anim.duration) + ")";
                        }
                        if (entity.HasComponent<Zgine::ECS::Health>())
                        {
                            auto& health = entity.GetComponent<Zgine::ECS::Health>();
                            entityInfo += " - Health: " + std::to_string(health.current) + "/" + std::to_string(health.maximum);
                        }
                        if (entity.HasComponent<Zgine::ECS::Tag>())
                        {
                            auto& tag = entity.GetComponent<Zgine::ECS::Tag>();
                            entityInfo += " - Name: " + tag.name;
                            if (!tag.tags.empty())
                            {
                                entityInfo += " - Tags: ";
                                for (size_t j = 0; j < tag.tags.size(); ++j)
                                {
                                    entityInfo += tag.tags[j];
                                    if (j < tag.tags.size() - 1) entityInfo += ", ";
                                }
                            }
                        }
                    }
                    
                    Zgine::IG::Text("%s", entityInfo.c_str());
                }
            }
            
            Zgine::IG::Separator();
            
            // Serialization Controls
            if (Zgine::IG::CollapsingHeader("Serialization"))
            {
                static char sceneName[256] = "test_scene";
                Zgine::IG::InputText("Scene Name", sceneName, sizeof(sceneName));
                
                Zgine::IG::Separator();
                
                if (Zgine::IG::Button("Save Scene"))
                {
                    std::string filepath = "scenes/" + std::string(sceneName) + ".json";
                    if (Zgine::ECS::ECSSerializer::SaveToFile(m_ECSManager, filepath, Zgine::JSON::JSONBackend::Nlohmann))
                    {
                        ZG_CORE_INFO("Scene saved successfully to: {}", filepath);
                    }
                    else
                    {
                        ZG_CORE_ERROR("Failed to save scene to: {}", filepath);
                    }
                }
                
                Zgine::IG::SameLine();
                if (Zgine::IG::Button("Load Scene"))
                {
                    std::string filepath = "scenes/" + std::string(sceneName) + ".json";
                    if (Zgine::ECS::ECSSerializer::LoadFromFile(m_ECSManager, filepath, Zgine::JSON::JSONBackend::Nlohmann))
                    {
                        ZG_CORE_INFO("Scene loaded successfully from: {}", filepath);
                        // Refresh entity list
                        RefreshEntityList();
                    }
                    else
                    {
                        ZG_CORE_ERROR("Failed to load scene from: {}", filepath);
                    }
                }
                
                Zgine::IG::Separator();
                
            }
            
            Zgine::IG::Separator();
            
            // Actions
            if (Zgine::IG::Button("Reset Entities"))
            {
                CreateTestEntities();
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Add Random Entity"))
            {
                AddRandomEntity();
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Remove Last Entity"))
            {
                RemoveLastEntity();
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Clear All Entities"))
            {
                ClearAllEntities();
            }
        }
        Zgine::IG::End();
    }
}

void ECSTestLayer::OnEvent(Zgine::Event& e)
{
    // Handle events if needed
}

void ECSTestLayer::CreateTestEntities()
{
    // Clear existing entities
    for (auto& entity : m_Entities)
    {
        if (entity.IsValid())
        {
            m_ECSManager.DestroyEntity(entity);
        }
    }
    m_Entities.clear();
    
    // Create Entity 1 - Red moving square (legacy Position/Velocity)
    auto entity1 = m_ECSManager.CreateEntity();
    entity1.AddComponent<Zgine::ECS::Position>(Zgine::ECS::Position{{-5.0f, 0.0f, 0.0f}});
    entity1.AddComponent<Zgine::ECS::Velocity>(Zgine::ECS::Velocity{{1.0f, 0.5f, 0.0f}});
    entity1.AddComponent<Zgine::ECS::Renderable>(Zgine::ECS::Renderable{{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f});
    m_Entities.push_back(entity1);
    
    // Create Entity 2 - Green moving circle (legacy Position/Velocity)
    auto entity2 = m_ECSManager.CreateEntity();
    entity2.AddComponent<Zgine::ECS::Position>(Zgine::ECS::Position{{0.0f, 3.0f, 0.0f}});
    entity2.AddComponent<Zgine::ECS::Velocity>(Zgine::ECS::Velocity{{-0.5f, -1.0f, 0.0f}});
    entity2.AddComponent<Zgine::ECS::Renderable>(Zgine::ECS::Renderable{{0.0f, 1.0f, 0.0f, 1.0f}, 1.5f});
    m_Entities.push_back(entity2);
    
    // Create Entity 3 - Blue stationary triangle (legacy Position/Velocity)
    auto entity3 = m_ECSManager.CreateEntity();
    entity3.AddComponent<Zgine::ECS::Position>(Zgine::ECS::Position{{5.0f, -2.0f, 0.0f}});
    entity3.AddComponent<Zgine::ECS::Velocity>(Zgine::ECS::Velocity{{0.0f, 0.0f, 0.0f}});
    entity3.AddComponent<Zgine::ECS::Renderable>(Zgine::ECS::Renderable{{0.0f, 0.0f, 1.0f, 1.0f}, 0.8f});
    m_Entities.push_back(entity3);
    
    // Create Entity 4 - Orange sprite with Transform
    auto entity4 = m_ECSManager.CreateEntity();
    entity4.AddComponent<Zgine::ECS::Transform>(Zgine::ECS::Transform{{-7.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.2f, 1.2f, 1.0f}});
    entity4.AddComponent<Zgine::ECS::Sprite>(Zgine::ECS::Sprite{{1.0f, 0.5f, 0.0f, 1.0f}, {1.5f, 1.5f}, {0.0f, 0.0f}, {1.0f, 1.0f}, 0, true});
    entity4.AddComponent<Zgine::ECS::Tag>(Zgine::ECS::Tag{"TransformSprite", {"sprite", "transform"}});
    m_Entities.push_back(entity4);
    
    // Create Entity 5 - Purple sprite with Physics
    auto entity5 = m_ECSManager.CreateEntity();
    entity5.AddComponent<Zgine::ECS::Transform>(Zgine::ECS::Transform{{7.0f, 4.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
    entity5.AddComponent<Zgine::ECS::Physics>(Zgine::ECS::Physics{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, 0.2f, 0.8f, false, true});
    entity5.AddComponent<Zgine::ECS::Sprite>(Zgine::ECS::Sprite{{0.5f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, 0, true});
    entity5.AddComponent<Zgine::ECS::Tag>(Zgine::ECS::Tag{"PhysicsSprite", {"physics", "gravity"}});
    m_Entities.push_back(entity5);
    
    // Create Entity 6 - Cyan animated sprite
    auto entity6 = m_ECSManager.CreateEntity();
    entity6.AddComponent<Zgine::ECS::Transform>(Zgine::ECS::Transform{{-3.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
    entity6.AddComponent<Zgine::ECS::Animation>(Zgine::ECS::Animation{0.0f, 3.0f, true, true, 1.0f, Zgine::ECS::Animation::Position});
    entity6.AddComponent<Zgine::ECS::Sprite>(Zgine::ECS::Sprite{{0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, 0, true});
    entity6.AddComponent<Zgine::ECS::Tag>(Zgine::ECS::Tag{"AnimatedSprite", {"animation", "moving"}});
    m_Entities.push_back(entity6);
    
    // Set up animation keyframes for entity6
    auto& anim = entity6.GetComponent<Zgine::ECS::Animation>();
    anim.keyTimes = {0.0f, 1.5f};
    anim.keyValues = {glm::vec4(-3.0f, -3.0f, 0.0f, 1.0f), glm::vec4(3.0f, -3.0f, 0.0f, 1.0f)};
    
    // Create Entity 7 - Yellow sprite with Health
    auto entity7 = m_ECSManager.CreateEntity();
    entity7.AddComponent<Zgine::ECS::Transform>(Zgine::ECS::Transform{{3.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
    entity7.AddComponent<Zgine::ECS::Health>(Zgine::ECS::Health{60.0f, 100.0f, true});
    entity7.AddComponent<Zgine::ECS::Sprite>(Zgine::ECS::Sprite{{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, 0, true});
    entity7.AddComponent<Zgine::ECS::Tag>(Zgine::ECS::Tag{"HealthSprite", {"health", "damageable"}});
    m_Entities.push_back(entity7);
    
    ZG_CORE_INFO("Created {} test entities with various component types", m_Entities.size());
}

void ECSTestLayer::RenderEntities()
{
    // Create a simple orthographic camera for ECS rendering
    Zgine::OrthographicCamera camera(-10.0f, 10.0f, -7.5f, 7.5f);
    
    // Render ECS entities using BatchRenderer2D
    Zgine::BatchRenderer2D::BeginScene(camera);
    
    // ZG_CORE_INFO("Rendering {} entities", m_Entities.size());
    
    // Render all entities in the dynamic list
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        auto& entity = m_Entities[i];
        if (!entity.IsValid()) continue;
        
        // Render entities with legacy Position/Renderable components
        if (entity.HasComponent<Zgine::ECS::Position>() && entity.HasComponent<Zgine::ECS::Renderable>())
        {
            auto& pos = entity.GetComponent<Zgine::ECS::Position>();
            auto& renderable = entity.GetComponent<Zgine::ECS::Renderable>();
            
            // Different rendering based on entity index
            if (i == 0) // First entity - red square
            {
                Zgine::BatchRenderer2D::DrawQuad(
                    glm::vec3(pos.position.x, pos.position.y, pos.position.z),
                    glm::vec2(renderable.scale, renderable.scale),
                    renderable.color
                );
            }
            else if (i == 1) // Second entity - green circle
            {
                Zgine::BatchRenderer2D::DrawCircle(
                    glm::vec3(pos.position.x, pos.position.y, pos.position.z),
                    renderable.scale,
                    renderable.color
                );
            }
            else if (i == 2) // Third entity - blue triangle
            {
                Zgine::BatchRenderer2D::DrawRotatedQuad(
                    glm::vec3(pos.position.x, pos.position.y, pos.position.z),
                    glm::vec2(renderable.scale, renderable.scale),
                    0.0f, // No rotation for now
                    renderable.color
                );
            }
            else if (i >= 3 && i <= 6) // Pre-defined test entities (index 3-6)
            {
                // Render pre-defined entities as circles with different colors
                Zgine::BatchRenderer2D::DrawCircle(
                    glm::vec3(pos.position.x, pos.position.y, pos.position.z),
                    renderable.scale,
                    renderable.color
                );
            }
            else // Random entities (index 7+) - render as colored circles
            {
                ZG_CORE_INFO("Rendering random entity {} at position ({}, {}, {}) with color ({}, {}, {}, {}) and scale {}", 
                             i, pos.position.x, pos.position.y, pos.position.z, 
                             renderable.color.r, renderable.color.g, renderable.color.b, renderable.color.a, renderable.scale);
                Zgine::BatchRenderer2D::DrawCircle(
                    glm::vec3(pos.position.x, pos.position.y, pos.position.z),
                    renderable.scale,
                    renderable.color
                );
            }
        }
        // Render entities with new Transform/Sprite components
        else if (entity.HasComponent<Zgine::ECS::Transform>() && entity.HasComponent<Zgine::ECS::Sprite>())
        {
            auto& transform = entity.GetComponent<Zgine::ECS::Transform>();
            auto& sprite = entity.GetComponent<Zgine::ECS::Sprite>();
            
            if (!sprite.visible) continue;
            
            // Different rendering based on entity index
            if (i == 3) // Entity 4 - Orange sprite with Transform
            {
                Zgine::BatchRenderer2D::DrawQuad(
                    glm::vec3(transform.position.x, transform.position.y, transform.position.z),
                    sprite.size,
                    sprite.color
                );
            }
            else if (i == 4) // Entity 5 - Purple sprite with Physics
            {
                Zgine::BatchRenderer2D::DrawCircle(
                    glm::vec3(transform.position.x, transform.position.y, transform.position.z),
                    sprite.size.x * 0.5f, // Use x component as radius
                    sprite.color
                );
            }
            else if (i == 5) // Entity 6 - Cyan animated sprite
            {
                Zgine::BatchRenderer2D::DrawRotatedQuad(
                    glm::vec3(transform.position.x, transform.position.y, transform.position.z),
                    sprite.size,
                    transform.rotation.z, // Use z rotation
                    sprite.color
                );
            }
            else if (i == 6) // Entity 7 - Yellow sprite with Health
            {
                Zgine::BatchRenderer2D::DrawQuad(
                    glm::vec3(transform.position.x, transform.position.y, transform.position.z),
                    sprite.size,
                    sprite.color
                );
            }
        }
    }
    
    Zgine::BatchRenderer2D::EndScene();
}

void ECSTestLayer::UpdateEntities(float ts)
{
    // Update ECS systems
    m_ECSManager.Update(ts * m_MovementSpeed);
}

void ECSTestLayer::AddRandomEntity()
{
    ZG_CORE_INFO("AddRandomEntity called - Current entity count: {}", m_Entities.size());
    
    auto entity = m_ECSManager.CreateEntity();
    ZG_CORE_INFO("Created entity with ID: {}", entity.GetID());
    
    // Random position within camera bounds
    float x = (float)(rand() % 20 - 10); // -10 to 10
    float y = (float)(rand() % 15 - 7.5f); // -7.5 to 7.5 (camera bounds)
    float z = 0.0f;
    
    // Random velocity
    float vx = (float)(rand() % 4 - 2); // -2 to 2
    float vy = (float)(rand() % 4 - 2); // -2 to 2
    float vz = 0.0f;
    
    // Random color
    float r = (float)(rand() % 100) / 100.0f;
    float g = (float)(rand() % 100) / 100.0f;
    float b = (float)(rand() % 100) / 100.0f;
    float a = 1.0f;
    
    // Random scale
    float scale = 0.5f + (float)(rand() % 100) / 100.0f; // 0.5 to 1.5
    
    entity.AddComponent<Zgine::ECS::Position>(Zgine::ECS::Position{{x, y, z}});
    // 不添加Velocity组件，让随机实体保持静止
    // entity.AddComponent<Zgine::ECS::Velocity>(Zgine::ECS::Velocity{{vx, vy, vz}});
    entity.AddComponent<Zgine::ECS::Renderable>(Zgine::ECS::Renderable{{r, g, b, a}, scale});
    
    m_Entities.push_back(entity);
    
    ZG_CORE_INFO("Added random entity at position ({}, {}, {}) with color ({}, {}, {}, {}) and scale {}", 
                 x, y, z, r, g, b, a, scale);
    ZG_CORE_INFO("Total entities now: {}", m_Entities.size());
}

void ECSTestLayer::RemoveLastEntity()
{
    if (!m_Entities.empty())
    {
        auto& lastEntity = m_Entities.back();
        if (lastEntity.IsValid())
        {
            m_ECSManager.DestroyEntity(lastEntity);
            ZG_CORE_INFO("Removed entity with ID: {}", lastEntity.GetID());
        }
        m_Entities.pop_back();
    }
}

void ECSTestLayer::ClearAllEntities()
{
    // Clear existing entities
    for (auto& entity : m_Entities)
    {
        if (entity.IsValid())
        {
            m_ECSManager.DestroyEntity(entity);
        }
    }
    m_Entities.clear();
    
    ZG_CORE_INFO("Cleared all entities");
}

void ECSTestLayer::RefreshEntityList()
{
    // Clear current entity list
    m_Entities.clear();
    
    // Get all entities from ECS manager
    auto allEntities = m_ECSManager.GetEntitiesWithComponent<Zgine::ECS::Position>();
    for (const auto& entity : allEntities)
    {
        m_Entities.push_back(entity);
    }
    
    ZG_CORE_INFO("Refreshed entity list, found {} entities", m_Entities.size());
}

}
