#pragma once

#include <Zgine/Core.h>
#include <Zgine/Events/Event.h>
#include <string>

namespace Zgine {

	/**
	 * @brief Base layer class for the engine's layer system
	 * @details This abstract base class defines the interface for all layers
	 *          in the engine's layer stack architecture
	 */
	class ZG_API Layer
	{
	public:
		/**
		 * @brief Construct a new Layer object
		 * @param name The debug name for this layer
		 */
		Layer(const std::string& name = "Layer");
		
		/**
		 * @brief Destroy the Layer object
		 */
		virtual ~Layer() {};

		/**
		 * @brief Called when the layer is attached to the layer stack
		 * @details Override this method to perform initialization tasks
		 *          when the layer is added to the application
		 */
		virtual void OnAttach() {};
		
		/**
		 * @brief Called when the layer is detached from the layer stack
		 * @details Override this method to perform cleanup tasks
		 *          when the layer is removed from the application
		 */
		virtual void OnDetach() {};
		
		/**
		 * @brief Called every frame to update the layer
		 * @param ts The time step since the last update
		 * @details Override this method to implement per-frame logic
		 */
		virtual void OnUpdate(Timestep ts) {};
		
		/**
		 * @brief Called every frame to render ImGui elements
		 * @details Override this method to implement ImGui rendering
		 *          for debugging and UI elements
		 */
		virtual void OnImGuiRender() {};
		
		/**
		 * @brief Called when an event occurs
		 * @param event The event that occurred
		 * @details Override this method to handle events like input,
		 *          window events, etc.
		 */
		virtual void OnEvent(Event& event) {};

		/**
		 * @brief Get the debug name of this layer
		 * @return const std::string& The debug name
		 */
		inline const std::string& GetName() const { return m_DebugName; };

	private:
		std::string m_DebugName;  ///< Debug name for this layer
	};

}

