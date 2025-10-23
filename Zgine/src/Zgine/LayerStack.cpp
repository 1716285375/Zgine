#include "zgpch.h"
#include "LayerStack.h"


namespace Zgine {
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		// Safely delete layers, checking for null pointers
		for (Layer* layer : m_Layers) {
			if (layer) {
				try {
					layer->OnDetach();
					delete layer;
				}
				catch (...) {
					// Ignore exceptions during cleanup
				}
			}
		}
		m_Layers.clear();
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		++m_LayerInsertIndex;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end()) {
			m_Layers.erase(it);
			--m_LayerInsertIndex;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end()) {
			m_Layers.erase(it);
		}
	}

}