#pragma once

namespace Zgine {

	/**
	 * @brief Abstract base class for graphics context implementations
	 * @details This class defines the interface for graphics context management,
	 *          providing platform-independent graphics context operations
	 */
	class GraphicsContext
	{
	public:
		/**
		 * @brief Initialize the graphics context
		 * @details Sets up the graphics context and prepares it for rendering
		 */
		virtual void Init() = 0;
		
		/**
		 * @brief Swap the front and back buffers
		 * @details Swaps the rendering buffers to display the rendered frame
		 */
		virtual void SwapBuffers() = 0;
	};

}