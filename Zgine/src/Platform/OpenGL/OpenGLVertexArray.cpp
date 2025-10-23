#include "zgpch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace Zgine {

	/**
	 * @brief Convert shader data type to OpenGL base type
	 * @param type The shader data type
	 * @return GLenum The corresponding OpenGL type
	 * @details Converts Zgine shader data types to OpenGL types for vertex attributes
	 */
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Zgine::ShaderDataType::Float:          return GL_FLOAT;
		case Zgine::ShaderDataType::Float2:         return GL_FLOAT;
		case Zgine::ShaderDataType::Float3:         return GL_FLOAT;
		case Zgine::ShaderDataType::Float4:         return GL_FLOAT;
		case Zgine::ShaderDataType::Mat3:           return GL_FLOAT;
		case Zgine::ShaderDataType::Mat4:           return GL_FLOAT;
		case Zgine::ShaderDataType::Int:            return GL_INT;
		case Zgine::ShaderDataType::Int2:           return GL_INT;
		case Zgine::ShaderDataType::Int3:           return GL_INT;
		case Zgine::ShaderDataType::Int4:           return GL_INT;
		case Zgine::ShaderDataType::Bool:           return GL_BOOL;
		}

		ZG_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	/**
	 * @brief Construct a new OpenGLVertexArray object
	 * @details Creates an OpenGL vertex array object
	 */
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	/**
	 * @brief Destroy the OpenGLVertexArray object
	 * @details Properly cleans up OpenGL vertex array resources
	 */
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	/**
	 * @brief Bind the vertex array
	 * @details Makes this OpenGL vertex array active for rendering
	 */
	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
		
		// Re-enable vertex attributes for this VAO
		// This ensures that vertex attributes are properly enabled when the VAO is bound
		for (int i = 0; i < 4; i++)
		{
			glEnableVertexAttribArray(i);
		}
	}

	/**
	 * @brief Unbind the vertex array
	 * @details Deactivates this OpenGL vertex array
	 */
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	/**
	 * @brief Add a vertex buffer to the vertex array
	 * @param vertexBuffer Reference-counted pointer to the vertex buffer
	 * @details Adds a vertex buffer to this OpenGL vertex array
	 */
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		ZG_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		const auto& layout = vertexBuffer->GetLayout();
		
		// Clear any existing vertex attribute state for this VAO
		for (int i = 0; i < 32; i++)
		{
			glDisableVertexAttribArray(i);
		}
		
		// Use traditional glVertexAttribPointer for compatibility
		// This ensures consistent behavior across all OpenGL versions
		for (const auto& element : layout)
		{
			// Get the location from the shader by name
			uint32_t location = GetAttributeLocation(element.Name, layout);
			
			// Use traditional vertex attribute pointer
			glVertexAttribPointer(
				location,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			glEnableVertexAttribArray(location);
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	/**
	 * @brief Set the index buffer for the vertex array
	 * @param indexBuffer Reference-counted pointer to the index buffer
	 * @details Sets the index buffer for this OpenGL vertex array
	 */
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		
		// Use traditional method for compatibility
		// Bind the index buffer while VAO is bound
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	/**
	 * @brief Get the attribute location based on attribute name and layout
	 * @param name The name of the attribute
	 * @param layout The vertex buffer layout to determine renderer type
	 * @return uint32_t The location index
	 * @details Maps attribute names to their layout locations based on renderer type
	 */
	uint32_t OpenGLVertexArray::GetAttributeLocation(const std::string& name, const BufferLayout& layout)
	{
		// Unified attribute layout for all renderers
		// This ensures consistent layout locations across all shaders
		
		// Standard attribute locations (same for all renderers):
		// location 0: a_Position (vec3)
		// location 1: a_Color (vec4) 
		// location 2: a_TexCoord (vec2)
		// location 3: a_TexIndex (float)
		// location 4: a_Normal (vec3) - only used by BatchRenderer3D
		
		if (name == "a_Position") return 0;
		if (name == "a_Color") return 1;
		if (name == "a_TexCoord") return 2;
		if (name == "a_TexIndex") return 3;
		if (name == "a_Normal") return 4;
		
		// Default fallback (should not happen with proper shaders)
		ZG_CORE_WARN("Unknown attribute name: {}", name);
		return 0;
	}

}