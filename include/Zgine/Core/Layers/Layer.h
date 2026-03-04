#pragma once

#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Core/Events/Event.h>
#include <Zgine/Core/Time/Timestep.h>
#include <string>

namespace Zgine {

    class Layer {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) { (void)ts; }
        virtual void OnGuiRender() {}
        virtual void OnEvent(Event& event) { (void)event; }

        const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };

}
