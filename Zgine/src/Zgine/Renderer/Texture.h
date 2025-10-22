#pragma once

#include <string>
#include <memory>
#include <cstdint>

namespace Zgine {

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual const std::string& GetPath() const = 0;

		static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<Texture> Create(const std::string& path);
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(const std::string& path);
		virtual ~Texture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual const std::string& GetPath() const override { return m_Path; }

		static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<Texture2D> Create(const std::string& path);

		bool operator==(const Texture2D& other) const
		{
			return m_RendererID == other.m_RendererID;
		}

	protected:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		bool m_IsLoaded = false;
	};

}
