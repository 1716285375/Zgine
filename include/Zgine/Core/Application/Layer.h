#pragma once

#include <Zgine/Core/Foundation/Macro.h>
#include <Zgine/Core/Foundation/Prerequisites.h>
#include <Zgine/Core/Events/Event.h>
#include <Zgine/Core/Time/Timestep.h>
#include <string>

namespace Zgine
{

    class Layer
    {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnFixedUpdate(Timestep ts) { ZGINE_UNUSED(ts); }
        virtual void OnUpdate(Timestep ts) { ZGINE_UNUSED(ts); }
        virtual void OnGuiRender() {}
        virtual void OnEvent(Event &event) { ZGINE_UNUSED(event); }

        const std::string &GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };

}
