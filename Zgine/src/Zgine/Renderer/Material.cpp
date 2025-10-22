#include "zgpch.h"
#include "Material.h"

#include <cstring>

namespace Zgine {

	Material::Material(const std::shared_ptr<Shader>& shader)
		: m_Name("Unnamed Material"), m_Shader(shader)
	{
	}

	Material::Material(const std::string& name, const std::shared_ptr<Shader>& shader)
		: m_Name(name), m_Shader(shader)
	{
	}

	Material::~Material()
	{
		for (auto& [name, property] : m_Properties)
		{
			CleanupProperty(property);
		}
	}

	void Material::SetFloat(const std::string& name, float value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		float* data = new float(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Float, name, data, sizeof(float));
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::vec2* data = new glm::vec2(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Float2, name, data, sizeof(glm::vec2));
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::vec3* data = new glm::vec3(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Float3, name, data, sizeof(glm::vec3));
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::vec4* data = new glm::vec4(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Float4, name, data, sizeof(glm::vec4));
	}

	void Material::SetInt(const std::string& name, int value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		int* data = new int(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Int, name, data, sizeof(int));
	}

	void Material::SetInt2(const std::string& name, const glm::ivec2& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::ivec2* data = new glm::ivec2(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Int2, name, data, sizeof(glm::ivec2));
	}

	void Material::SetInt3(const std::string& name, const glm::ivec3& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::ivec3* data = new glm::ivec3(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Int3, name, data, sizeof(glm::ivec3));
	}

	void Material::SetInt4(const std::string& name, const glm::ivec4& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::ivec4* data = new glm::ivec4(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Int4, name, data, sizeof(glm::ivec4));
	}

	void Material::SetMat3(const std::string& name, const glm::mat3& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::mat3* data = new glm::mat3(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Mat3, name, data, sizeof(glm::mat3));
	}

	void Material::SetMat4(const std::string& name, const glm::mat4& value)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		glm::mat4* data = new glm::mat4(value);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Mat4, name, data, sizeof(glm::mat4));
	}

	void Material::SetTexture(const std::string& name, const std::shared_ptr<Texture2D>& texture)
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end())
		{
			CleanupProperty(it->second);
		}

		std::shared_ptr<Texture2D>* data = new std::shared_ptr<Texture2D>(texture);
		m_Properties[name] = MaterialProperty(ShaderUniformType::Texture2D, name, data, sizeof(std::shared_ptr<Texture2D>));
	}

	void Material::Bind() const
	{
		m_Shader->Bind();
		UploadUniforms();
	}

	void Material::Unbind() const
	{
		m_Shader->Unbind();
	}

	void Material::UploadUniforms() const
	{
		uint32_t textureSlot = 0;
		for (const auto& [name, property] : m_Properties)
		{
			switch (property.Type)
			{
				case ShaderUniformType::Float:
					m_Shader->UploadUniformFloat(property.Name, *(float*)property.Data);
					break;
				case ShaderUniformType::Float2:
					m_Shader->UploadUniformFloat2(property.Name, *(glm::vec2*)property.Data);
					break;
				case ShaderUniformType::Float3:
					m_Shader->UploadUniformFloat3(property.Name, *(glm::vec3*)property.Data);
					break;
				case ShaderUniformType::Float4:
					m_Shader->UploadUniformFloat4(property.Name, *(glm::vec4*)property.Data);
					break;
				case ShaderUniformType::Int:
					m_Shader->UploadUniformInt(property.Name, *(int*)property.Data);
					break;
				case ShaderUniformType::Int2:
					m_Shader->UploadUniformInt2(property.Name, *(glm::ivec2*)property.Data);
					break;
				case ShaderUniformType::Int3:
					m_Shader->UploadUniformInt3(property.Name, *(glm::ivec3*)property.Data);
					break;
				case ShaderUniformType::Int4:
					m_Shader->UploadUniformInt4(property.Name, *(glm::ivec4*)property.Data);
					break;
				case ShaderUniformType::Mat3:
					m_Shader->UploadUniformMat3(property.Name, *(glm::mat3*)property.Data);
					break;
				case ShaderUniformType::Mat4:
					m_Shader->UploadUniformMat4(property.Name, *(glm::mat4*)property.Data);
					break;
				case ShaderUniformType::Texture2D:
				{
					std::shared_ptr<Texture2D>* texture = (std::shared_ptr<Texture2D>*)property.Data;
					if (*texture)
					{
						(*texture)->Bind(textureSlot);
						m_Shader->UploadUniformInt(property.Name, textureSlot);
						textureSlot++;
					}
					break;
				}
			}
		}
	}

	void Material::CleanupProperty(MaterialProperty& property)
	{
		if (property.Type == ShaderUniformType::Texture2D)
		{
			delete (std::shared_ptr<Texture2D>*)property.Data;
		}
		else
		{
			delete[] (char*)property.Data;
		}
	}

}
