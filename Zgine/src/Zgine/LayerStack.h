#pragma once

#include <Zgine/Core.h>
#include "Layer.h"
#include <vector>

namespace Zgine {

	/**
	 * @brief Layer stack management class for organizing application layers
	 * @details This class manages a stack of layers and overlays, providing
	 *          ordered execution and proper lifecycle management
	 */
	class ZG_API LayerStack
	{
	public:
		/**
		 * @brief Construct a new LayerStack object
		 */
		LayerStack();
		
		/**
		 * @brief Destroy the LayerStack object
		 * @details Safely destroys all layers in the stack
		 */
		~LayerStack();

		/**
		 * @brief Push a layer onto the stack
		 * @param layer Pointer to the layer to add
		 * @details Layers are rendered behind overlays
		 */
		void PushLayer(Layer* layer);
		
		/**
		 * @brief Push an overlay onto the stack
		 * @param overlay Pointer to the overlay to add
		 * @details Overlays are rendered on top of layers
		 */
		void PushOverlay(Layer* overlay);
		
		/**
		 * @brief Pop a layer from the stack
		 * @param layer Pointer to the layer to remove
		 */
		void PopLayer(Layer* layer);
		
		/**
		 * @brief Pop an overlay from the stack
		 * @param overlay Pointer to the overlay to remove
		 */
		void PopOverlay(Layer* overlay);

		/**
		 * @brief Get iterator to the beginning of the layer collection
		 * @return std::vector<Layer*>::iterator Iterator to the first layer
		 */
		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		
		/**
		 * @brief Get iterator to the end of the layer collection
		 * @return std::vector<Layer*>::iterator Iterator past the last layer
		 */
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;           ///< Vector containing all layers and overlays
		unsigned int m_LayerInsertIndex = 0;    ///< Index where new layers should be inserted
	};

}