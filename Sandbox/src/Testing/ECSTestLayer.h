#pragma once

#include "Zgine/ImGui/ImGuiLayer.h"
#include "Zgine/Timestep.h"
#include "Zgine/Events/Event.h"
#include "Zgine/ECS/ECS.h"

namespace Sandbox {

class ECSTestLayer : public Zgine::ImGuiLayer
{
public:
    ECSTestLayer();
    virtual ~ECSTestLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Zgine::Timestep ts) override;
    virtual void OnEvent(Zgine::Event& e) override;

protected:
    virtual void RenderCustomUI() override;

private:
    void CreateTestEntities();
    void RenderEntities();
    void UpdateEntities(float ts);
    void AddRandomEntity();
    void RemoveLastEntity();
    void ClearAllEntities();
    void RefreshEntityList();
    
    // ECS management
    Zgine::ECS::ECSManager m_ECSManager;

private:
    
    // Dynamic entity list
    std::vector<Zgine::ECS::Entity> m_Entities;
    
    // UI state
    bool m_ShowECSWindow = true;
    bool m_EnableMovement = true;
    float m_MovementSpeed = 2.0f;
    
    // Statistics
    float m_LastUpdateTime = 0.0f;
};

}
