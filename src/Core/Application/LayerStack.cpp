#include <Zgine/Core/Application/LayerStack.h>

namespace Zgine {

    Layer::Layer(const std::string& debugName)
        : m_DebugName(debugName)
    {
    }

    LayerStack::~LayerStack()
    {
        // Call OnDetach on all layers, but DO NOT delete them
        // Layers are owned by the caller (e.g., Application holds m_GuiLayer in a smart pointer)
        // The owner is responsible for deletion
        for (Layer* layer : m_Layers)
        {
            if (layer) {
                layer->OnDetach();
            }
        }
        // Clear the vector but don't delete pointers
        m_Layers.clear();
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->OnAttach();
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_Layers.emplace_back(overlay);
        overlay->OnAttach();
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
        if (it != m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            overlay->OnDetach();
            m_Layers.erase(it);
        }
    }

}
