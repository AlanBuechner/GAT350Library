#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"
#include "TextureAttribute.h"

namespace Engine
{
	class Texture
	{

	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void LoadFromFile(const std::string& path) = 0;
		virtual Ref<TextureAttribute> GetAttributes() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:

		Texture2D(const std::string& path, Ref<TextureAttribute> attribute);
		Texture2D(const uint32_t width, const uint32_t height, unsigned char* data, Ref<TextureAttribute> attribute);
		virtual ~Texture2D() override = default;

		virtual uint32_t GetWidth() const override { return m_Width; };
		virtual uint32_t GetHeight() const override { return m_Height; };
		virtual uint32_t GetRendererID() const override { return 0; };

		virtual void SetData(void* data, uint32_t size) override;
		virtual void LoadFromFile(const std::string& path) override;
		virtual Ref<TextureAttribute> GetAttributes() const override { return m_Attribute; };

		wrl::ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_pTextureView; }
		wrl::ComPtr<ID3D11SamplerState> GetSampler() { return m_pSampler; }

		virtual bool operator==(const Texture& other) const override;

		static Ref<Texture2D> Create(const std::string& path = "", TextureAttribute& attribute = *TextureAttribute::Default);
		static Ref<Texture2D> Create(const uint32_t width, const uint32_t height, TextureAttribute& attribute = *TextureAttribute::Default);
		static Ref<Texture2D> Create(const uint32_t width, const uint32_t height, unsigned char* data, TextureAttribute& attribute = *TextureAttribute::Default);

	private:
		void GenTextureView(void* data, uint32_t width, uint32_t height, uint32_t channels);
		void GenSampler();



	private:
		uint32_t m_Width = 0, m_Height = 0;
		Ref<TextureAttribute> m_Attribute;

		wrl::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		wrl::ComPtr<ID3D11SamplerState> m_pSampler;
	};
}