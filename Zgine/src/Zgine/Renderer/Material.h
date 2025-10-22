#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#include "Zgine/Renderer/Shader.h"
#include "Zgine/Renderer/Texture.h"

namespace Zgine {

	enum class ShaderUniformType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Texture2D
	};

	struct MaterialProperty
	{
		ShaderUniformType Type;
		std::string Name;
		void* Data;
		size_t Size;

		MaterialProperty() = default;
		MaterialProperty(ShaderUniformType type, const std::string& name, void* data, size_t size)
			: Type(type), Name(name), Data(data), Size(size) {}
	};

	class Material
	{
	public:
		Material(const std::shared_ptr<Shader>& shader);
		Material(const std::string& name, const std::shared_ptr<Shader>& shader);
		virtual ~Material();

		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);

		void SetInt(const std::string& name, int value);
		void SetInt2(const std::string& name, const glm::ivec2& value);
		void SetInt3(const std::string& name, const glm::ivec3& value);
		void SetInt4(const std::string& name, const glm::ivec4& value);

		void SetMat3(const std::string& name, const glm::mat3& value);
		void SetMat4(const std::string& name, const glm::mat4& value);

		void SetTexture(const std::string& name, const std::shared_ptr<Texture2D>& texture);

		void Bind() const;
		void Unbind() const;

		const std::string& GetName() const { return m_Name; }
		const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }

		template<typename T>
		T GetValue(const std::string& name) const;

	private:
		void UploadUniforms() const;
		void CleanupProperty(MaterialProperty& property);

	private:
		std::string m_Name;
		std::shared_ptr<Shader> m_Shader;
		std::unordered_map<std::string, MaterialProperty> m_Properties;
	};

}
