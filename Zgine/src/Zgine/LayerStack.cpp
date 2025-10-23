#include "zgpch.h"
#include "LayerStack.h"

namespace Zgine {

	/**
	 * @brief Construct a new LayerStack object
	 */
	LayerStack::LayerStack()
	{
		// LayerStack constructor implementation
	}

	/**
	 * @brief Destroy the LayerStack object
	 * @details Safely destroys all layers in the stack
	 */
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

	/**
	 * @brief Push a layer onto the stack
	 * @param layer Pointer to the layer to add
	 * @details Layers are rendered behind overlays
	 */
	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		++m_LayerInsertIndex;
	}

	/**
	 * @brief Push an overlay onto the stack
	 * @param overlay Pointer to the overlay to add
	 * @details Overlays are rendered on top of layers
	 */
	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	/**
	 * @brief Pop a layer from the stack
	 * @param layer Pointer to the layer to remove
	 */
	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end()) {
			m_Layers.erase(it);
			--m_LayerInsertIndex;
		}
	}

	/**
	 * @brief Pop an overlay from the stack
	 * @param overlay Pointer to the overlay to remove
	 */
	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end()) {
			m_Layers.erase(it);
		}
	}

}