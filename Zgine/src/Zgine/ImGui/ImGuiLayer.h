#pragma once

#include "Zgine/Layer.h"
#include "Zgine/Events/KeyEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include "Zgine/Events/ApplicationEvent.h"

namespace Zgine {

	/**
	 * @brief ImGui layer for debugging and UI rendering
	 * @details This layer handles ImGui initialization, rendering,
	 *          and event processing for debugging interfaces
	 */
	class ZG_API ImGuiLayer : public Layer
	{
	public:
		/**
		 * @brief Construct a new ImGuiLayer object
		 * @details Initializes the ImGui layer with default settings
		 */
		ImGuiLayer();
		
		/**
		 * @brief Destroy the ImGuiLayer object
		 * @details Properly cleans up ImGui resources
		 */
		~ImGuiLayer();
		
		/**
		 * @brief Called when the layer is attached to the layer stack
		 * @details Initializes ImGui and sets up the rendering context
		 */
		virtual void OnAttach() override;
		
		/**
		 * @brief Called when the layer is detached from the layer stack
		 * @details Shuts down ImGui and cleans up resources
		 */
		virtual void OnDetach() override;
		
		/**
		 * @brief Called every frame to render ImGui elements
		 * @details Renders ImGui debug interface and UI elements
		 */
		virtual void OnImGuiRender() override;
		
		/**
		 * @brief Called when an event occurs
		 * @param e The event that occurred
		 * @details Handles ImGui events and input processing
		 */
		virtual void OnEvent(Event& e) override;

		/**
		 * @brief Begin ImGui frame
		 * @details Starts a new ImGui frame for rendering
		 */
		void Begin();
		
		/**
		 * @brief End ImGui frame
		 * @details Ends the current ImGui frame and renders it
		 */
		void End();

	private:
		float m_Time = 0.0f;  ///< Time tracking for ImGui rendering
	};

}


