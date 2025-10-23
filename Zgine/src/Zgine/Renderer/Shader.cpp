#include "zgpch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Zgine {

	/**
	 * @brief Construct a new Shader object
	 * @param vertexSrc The vertex shader source code
	 * @param fragmentSrc The fragment shader source code
	 * @details Creates and compiles a shader program from the provided source code
	 */
	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_RendererID(0)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar *source = (const GLchar *)vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		//Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(vertexShader);

			ZG_CORE_ERROR("{0}", infoLog.data());
			ZG_CORE_ASSERT(false, "Vertex shader compilation failure!");

			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = (const GLchar*)fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			ZG_CORE_ERROR("{0}", infoLog.data());
			ZG_CORE_ASSERT(false, "Fragment shader compilation failure!");

			return;
		}

		// When vertex shader and fragment shader are successfully compiled.
		// Now time link them together into a program
		// Get a program object
		m_RendererID = glCreateProgram();
		GLuint program = m_RendererID;

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			ZG_CORE_ERROR("{0}", infoLog.data());
			ZG_CORE_ASSERT(false, "Shader link failure!");

			return;
		}
		
		// Always detach shaders after a successful link
		glDetachShader(program, fragmentShader);
		glDetachShader(program, vertexShader);
	}

	/**
	 * @brief Destroy the Shader object
	 * @details Properly cleans up the shader program resources
	 */
	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	/**
	 * @brief Bind the shader program
	 * @details Makes this shader program active for rendering
	 */
	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	
	/**
	 * @brief Unbind the shader program
	 * @details Deactivates the current shader program
	 */
	void Shader::Unbind() const
	{
		glUseProgram(0);
	}
	
	/**
	 * @brief Upload a float uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The float value to upload
	 */
	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	/**
	 * @brief Upload a vec2 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The glm::vec2 value to upload
	 */
	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	/**
	 * @brief Upload a vec3 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The glm::vec3 value to upload
	 */
	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	/**
	 * @brief Upload a vec4 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The glm::vec4 value to upload
	 */
	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	/**
	 * @brief Upload an int uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The int value to upload
	 */
	void Shader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	/**
	 * @brief Upload an ivec2 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The glm::ivec2 value to upload
	 */
	void Shader::UploadUniformInt2(const std::string& name, const glm::ivec2& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2i(location, value.x, value.y);
	}

	/**
	 * @brief Upload an ivec3 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The glm::ivec3 value to upload
	 */
	void Shader::UploadUniformInt3(const std::string& name, const glm::ivec3& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3i(location, value.x, value.y, value.z);
	}

	/**
	 * @brief Upload an ivec4 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param value The glm::ivec4 value to upload
	 */
	void Shader::UploadUniformInt4(const std::string& name, const glm::ivec4& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4i(location, value.x, value.y, value.z, value.w);
	}

	/**
	 * @brief Upload an int array uniform to the shader
	 * @param name The name of the uniform variable
	 * @param values Pointer to the int array
	 * @param count Number of elements in the array
	 */
	void Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	/**
	 * @brief Upload a mat3 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param matrix The glm::mat3 matrix to upload
	 */
	void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	/**
	 * @brief Upload a mat4 uniform to the shader
	 * @param name The name of the uniform variable
	 * @param matrix The glm::mat4 matrix to upload
	 */
	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	/**
	 * @brief Get the location of a uniform variable
	 * @param name The name of the uniform variable
	 * @return int The OpenGL uniform location
	 * @details Retrieves the location of a uniform variable in the shader program
	 */
	int Shader::GetUniformLocation(const std::string& name) const
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1)
		{
			ZG_CORE_WARN("Uniform '{0}' not found!", name);
		}
		return location;
	}

}