#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"

namespace Engine
{
	class Texture
	{
	public:

		enum class WrapMode
		{
			Repeat,
			MirroredRepeat,
			ClampToEdge
		};

		enum class MinMagFilter
		{
			Nearest,
			Linear
		};

		enum class Format
		{
			// 8 bit components normalized
			R8_UNORM,
			RG8_UNORM,
			RGBA8_UNORM,
			R8_SNORM,
			RG8_SNORM,
			RGBA8_SNORM,

			// 8 bit components int
			R8_UINT,
			RG8_UINT,
			RGBA8_UINT,
			R8_SINT,
			RG8_SINT,
			RGBA8_SINT,

			// 16 bit components normalized
			R16_UNORM,
			RG16_UNORM,
			RGBA16_UNORM,
			R16_SNORM,
			RG16_SNORM,
			RGBA16_SNORM,

			// 16 bit components int
			R16_UINT,
			RG16_UINT,
			RGBA16_UINT,
			R16_SINT,
			RG16_SINT,
			RGBA16_SINT,

			// 16 bit components float
			R16_FLOAT,
			RG16_FLOAT,
			RGBA16_FLOAT,

			// 32 bit components int
			R32_UINT,
			RG32_UINT,
			RGBA32_UINT,
			R32_SINT,
			RG32_SINT,
			RGBA32_SINT,

			// 32 bit components float
			R32_FLOAT,
			RG32_FLOAT,
			RGBA32_FLOAT,

			// depth stencil
			// NOTE : shaders can not sample form stencil channel 
			D16_UNORM,
			D24_UNORM_S8_UINT,
			D32_FLOAT,
			D32_FLOAT_S8_UINT,
		};

		static DXGI_FORMAT GetDXGIFormat(Format format);
		static DXGI_FORMAT GetDXGIBufferFormat(Format format);
		static DXGI_FORMAT GetDXGISRVFormat(Format format);
		static uint32_t GetFormatSize(Format format);
		static bool IsDepthOrStencil(Format format);

	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void LoadFromFile(const fs::path& path) = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	protected:
		Texture2D(Format format) : m_Format(format) {}; // WARNING : only use this if the resource is being created by the inherited class

	public:
		Texture2D(const fs::path& path);
		Texture2D(uint32_t width, uint32_t height, Format format, unsigned char const* data);
		virtual ~Texture2D() override = default;

		virtual uint32_t GetWidth() const override { return m_Width; };
		virtual uint32_t GetHeight() const override { return m_Height; };

		virtual void SetData(void* data, uint32_t size) override;
		virtual void LoadFromFile(const fs::path& path) override;

		virtual bool IsDepthStencilTexture() { return IsDepthOrStencil(m_Format); }

		wrl::ComPtr<ID3D11Texture2D> GetBuffer() { return m_Buffer; }
		wrl::ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_SRV; }

		virtual bool operator==(const Texture& other) const override;

		static Ref<Texture2D> Create(const fs::path& path = "");
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, Format format);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, Format format, unsigned char const* data);

	protected:
		void GenTextureBuffer(void* data, uint32_t numMipMaps = 0);
		void GenSRV();

	protected:
		uint32_t m_Width = 0, m_Height = 0;
		Format m_Format = Texture::Format::RGBA8_UNORM;

		wrl::ComPtr<ID3D11Texture2D> m_Buffer;
		wrl::ComPtr<ID3D11ShaderResourceView> m_SRV;

	};
}