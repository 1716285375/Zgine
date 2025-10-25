#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Zgine {

	/**
	 * @brief Shader class for managing GPU shader programs
	 * @details This class handles the creation, compilation, and management
	 *          of vertex and fragment shaders, including uniform uploads
	 */
	class Shader
	{
	public:
		/**
		 * @brief Construct a new Shader object
		 * @param vertexSrc The vertex shader source code
		 * @param fragmentSrc The fragment shader source code
		 * @details Creates and compiles a shader program from the provided source code
		 */
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		
		/**
		 * @brief Create a shader from source code
		 * @param vertexSrc The vertex shader source code
		 * @param fragmentSrc The fragment shader source code
		 * @return Ref<Shader> A reference to the created shader
		 */
		static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
		
		/**
		 * @brief Create a shader from a single source file
		 * @param source The combined shader source code
		 * @return Ref<Shader> A reference to the created shader
		 */
		static Ref<Shader> Create(const std::string& source);
		
		/**
		 * @brief Destroy the Shader object
		 * @details Properly cleans up the shader program resources
		 */
		~Shader();

		/**
		 * @brief Bind the shader program
		 * @details Makes this shader program active for rendering
		 */
		void Bind() const;
		
		/**
		 * @brief Unbind the shader program
		 * @details Deactivates the current shader program
		 */
		void Unbind() const;

		/**
		 * @brief Get the renderer ID of the shader
		 * @return uint32_t The OpenGL shader program ID
		 */
		uint32_t GetRendererID() const { return m_RendererID; }

		/**
		 * @brief Get the size of the shader program
		 * @return size_t The estimated size of the shader program in bytes
		 */
		size_t GetSize() const;

		/**
		 * @brief Upload a float uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The float value to upload
		 */
		void UploadUniformFloat(const std::string& name, float value);
		
		/**
		 * @brief Upload a vec2 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The glm::vec2 value to upload
		 */
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		
		/**
		 * @brief Upload a vec3 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The glm::vec3 value to upload
		 */
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		
		/**
		 * @brief Upload a vec4 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The glm::vec4 value to upload
		 */
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		/**
		 * @brief Upload an int uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The int value to upload
		 */
		void UploadUniformInt(const std::string& name, int value);
		
		/**
		 * @brief Upload an ivec2 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The glm::ivec2 value to upload
		 */
		void UploadUniformInt2(const std::string& name, const glm::ivec2& value);
		
		/**
		 * @brief Upload an ivec3 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The glm::ivec3 value to upload
		 */
		void UploadUniformInt3(const std::string& name, const glm::ivec3& value);
		
		/**
		 * @brief Upload an ivec4 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param value The glm::ivec4 value to upload
		 */
		void UploadUniformInt4(const std::string& name, const glm::ivec4& value);

		/**
		 * @brief Upload an int array uniform to the shader
		 * @param name The name of the uniform variable
		 * @param values Pointer to the int array
		 * @param count Number of elements in the array
		 */
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		/**
		 * @brief Upload a mat3 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param matrix The glm::mat3 matrix to upload
		 */
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		
		/**
		 * @brief Upload a mat4 uniform to the shader
		 * @param name The name of the uniform variable
		 * @param matrix The glm::mat4 matrix to upload
		 */
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		/**
		 * @brief Get the location of a uniform variable
		 * @param name The name of the uniform variable
		 * @return int The OpenGL uniform location
		 * @details Retrieves the location of a uniform variable in the shader program
		 */
		int GetUniformLocation(const std::string& name) const;

	private:
		uint32_t m_RendererID;  ///< OpenGL shader program ID
	};

}