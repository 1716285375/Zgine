#pragma once

#include <cstdint>

namespace Zgine {

	/**
	 * @brief Enumeration of shader data types
	 * @details Defines all supported data types for vertex attributes
	 */
	enum class ShaderDataType
	{
		None = 0,    ///< No data type
		Float,       ///< Single float
		Float2,      ///< Two floats (vec2)
		Float3,      ///< Three floats (vec3)
		Float4,      ///< Four floats (vec4)
		Mat3,        ///< 3x3 matrix
		Mat4,        ///< 4x4 matrix
		Int,         ///< Single integer
		Int2,        ///< Two integers (ivec2)
		Int3,        ///< Three integers (ivec3)
		Int4,        ///< Four integers (ivec4)
		Bool         ///< Boolean value
	};

	/**
	 * @brief Get the size in bytes of a shader data type
	 * @param type The shader data type
	 * @return uint32_t The size in bytes
	 * @details Returns the memory size required for the specified data type
	 */
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:            return 4;
			case ShaderDataType::Float2:           return 4 * 2;
			case ShaderDataType::Float3:           return 4 * 3;
			case ShaderDataType::Float4:           return 4 * 4;
			case ShaderDataType::Mat3:             return 4 * 3 * 3;
			case ShaderDataType::Mat4:             return 4 * 4 * 4;
			case ShaderDataType::Int:              return 4;
			case ShaderDataType::Int2:             return 4 * 2;
			case ShaderDataType::Int3:             return 4 * 3;
			case ShaderDataType::Int4:             return 4 * 4;
			case ShaderDataType::Bool:             return 1;
		}

		ZG_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	/**
	 * @brief Buffer element structure for vertex attributes
	 * @details Represents a single vertex attribute in a buffer layout
	 */
	class BufferElement
	{
	public:
		std::string Name;        ///< Name of the attribute
		ShaderDataType Type;    ///< Data type of the attribute
		uint32_t Size;          ///< Size in bytes
		uint32_t Offset;        ///< Offset from start of vertex
		bool Normalized;        ///< Whether the data should be normalized

		/**
		 * @brief Construct a new BufferElement object
		 * @param type The shader data type
		 * @param name The name of the attribute
		 * @param normalized Whether the data should be normalized (default: false)
		 * @details Creates a buffer element with the specified type and name
		 */
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
			// BufferElement constructor implementation
		}

		/**
		 * @brief Get the number of components in this data type
		 * @return uint32_t The number of components
		 * @details Returns how many individual values this data type contains
		 */
		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:           return 1;
				case ShaderDataType::Float2:          return 2;
				case ShaderDataType::Float3:          return 3;
				case ShaderDataType::Float4:          return 4;
				case ShaderDataType::Mat3:            return 3 * 3;
				case ShaderDataType::Mat4:            return 4 * 4;
				case ShaderDataType::Int:             return 1;
				case ShaderDataType::Int2:            return 2;
				case ShaderDataType::Int3:            return 3;
				case ShaderDataType::Int4:            return 4;
				case ShaderDataType::Bool:            return 1;
			}

			ZG_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};
	
	/**
	 * @brief Buffer layout class for defining vertex attribute layouts
	 * @details This class manages the layout of vertex attributes in a buffer
	 */
	class BufferLayout
	{
	public:
		/**
		 * @brief Construct a new BufferLayout object
		 * @details Creates an empty buffer layout
		 */
		BufferLayout() = default;
		
		/**
		 * @brief Construct a new BufferLayout object with elements
		 * @param elements Initializer list of buffer elements
		 * @details Creates a buffer layout with the specified elements
		 */
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		/**
		 * @brief Get the stride of the buffer layout
		 * @return uint32_t The stride in bytes
		 * @details Returns the total size of one vertex in the buffer
		 */
		inline uint32_t GetStride() const { return m_Stride; }
		
		/**
		 * @brief Get the elements of the buffer layout
		 * @return const std::vector<BufferElement>& Reference to the elements
		 * @details Returns all buffer elements in this layout
		 */
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		/**
		 * @brief Get iterator to the beginning of elements
		 * @return std::vector<BufferElement>::iterator Iterator to first element
		 */
		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		
		/**
		 * @brief Get iterator to the end of elements
		 * @return std::vector<BufferElement>::iterator Iterator past last element
		 */
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		/**
		 * @brief Get const iterator to the beginning of elements
		 * @return std::vector<BufferElement>::const_iterator Const iterator to first element
		 */
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		
		/**
		 * @brief Get const iterator to the end of elements
		 * @return std::vector<BufferElement>::const_iterator Const iterator past last element
		 */
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		/**
		 * @brief Calculate offsets and stride for all elements
		 * @details Computes the offset for each element and the total stride
		 */
		void CalculateOffsetAndStride();
		
	private:
		std::vector<BufferElement> m_Elements;  ///< Vector of buffer elements
		uint32_t m_Stride = 0;                 ///< Total stride in bytes
	};

	/**
	 * @brief Abstract base class for vertex buffers
	 * @details This class defines the interface for vertex buffer implementations
	 */
	class VertexBuffer
	{
	public:
		/**
		 * @brief Destroy the VertexBuffer object
		 */
		virtual ~VertexBuffer() {}

		/**
		 * @brief Bind the vertex buffer
		 * @details Makes this vertex buffer active for rendering
		 */
		virtual void Bind() const = 0;
		
		/**
		 * @brief Unbind the vertex buffer
		 * @details Deactivates this vertex buffer
		 */
		virtual void Unbind() const = 0;

		/**
		 * @brief Get the buffer layout
		 * @return const BufferLayout& Reference to the buffer layout
		 * @details Returns the layout of vertex attributes in this buffer
		 */
		virtual const BufferLayout& GetLayout() const = 0;
		
		/**
		 * @brief Set the buffer layout
		 * @param layout The buffer layout to set
		 * @details Sets the layout of vertex attributes for this buffer
		 */
		virtual void SetLayout(const BufferLayout& layout) = 0;

		/**
		 * @brief Set vertex data
		 * @param data Pointer to the vertex data
		 * @param size Size of the data in bytes
		 * @details Uploads vertex data to the buffer
		 */
		virtual void SetData(const void* data, uint32_t size) = 0;

		/**
		 * @brief Create a new vertex buffer
		 * @param vertices Pointer to vertex data
		 * @param size Size of the data in bytes
		 * @return VertexBuffer* Pointer to the created vertex buffer
		 * @details Creates a platform-specific vertex buffer implementation
		 */
		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	/**
	 * @brief Abstract base class for index buffers
	 * @details This class defines the interface for index buffer implementations
	 */
	class IndexBuffer
	{
	public:
		/**
		 * @brief Destroy the IndexBuffer object
		 */
		virtual ~IndexBuffer() {}

		/**
		 * @brief Bind the index buffer
		 * @details Makes this index buffer active for rendering
		 */
		virtual void Bind() const = 0;
		
		/**
		 * @brief Unbind the index buffer
		 * @details Deactivates this index buffer
		 */
		virtual void Unbind() const = 0;

		/**
		 * @brief Get the number of indices
		 * @return uint32_t The number of indices
		 * @details Returns the count of indices in this buffer
		 */
		virtual uint32_t GetCount() const = 0;

		/**
		 * @brief Create a new index buffer
		 * @param indices Pointer to index data
		 * @param count Number of indices
		 * @return IndexBuffer* Pointer to the created index buffer
		 * @details Creates a platform-specific index buffer implementation
		 */
		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};

}