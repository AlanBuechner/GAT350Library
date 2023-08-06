#include "Texture.h"
#include "RendererAPI.h"
#include "stb_image.h"

stbi_uc* T24To32(uint32_t width, uint32_t height, stbi_uc* data)
{
	struct Pixel32 {
		uint8_t r, g, b, a;
	};
	struct Pixel24 {
		uint8_t r, g, b;
	};

	Pixel32* newImage = new Pixel32[width * height];

	for (uint32_t h = 0; h < height; h++)
	{
		for (uint32_t w = 0; w < width; w++)
		{
			Pixel32& np = newImage[(width * h) + w];
			Pixel24& op = ((Pixel24*)data)[(width * h) + w];
			np.r = op.r;
			np.g = op.g;
			np.b = op.b;
			np.a = 1;
		}
	}

	delete[] data;
	return (stbi_uc*)newImage;
}

namespace Engine 
{

	DXGI_FORMAT Texture::GetDXGIFormat(Format format)
	{
		switch (format)
		{
		case Format::R8_UNORM:			return DXGI_FORMAT_R8_UNORM;
		case Format::RG8_UNORM:			return DXGI_FORMAT_R8G8_UNORM;
		case Format::RGBA8_UNORM:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::R8_SNORM:			return DXGI_FORMAT_R8_SNORM;
		case Format::RG8_SNORM:			return DXGI_FORMAT_R8G8_SNORM;
		case Format::RGBA8_SNORM:		return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Format::R8_UINT:			return DXGI_FORMAT_R8_UINT;
		case Format::RG8_UINT:			return DXGI_FORMAT_R8G8_UINT;
		case Format::RGBA8_UINT:		return DXGI_FORMAT_R8G8B8A8_UINT;
		case Format::R8_SINT:			return DXGI_FORMAT_R8_SINT;
		case Format::RG8_SINT:			return DXGI_FORMAT_R8G8_SINT;
		case Format::RGBA8_SINT:		return DXGI_FORMAT_R8G8B8A8_SINT;

		case Format::R16_UNORM:			return DXGI_FORMAT_R16_UNORM;
		case Format::RG16_UNORM:		return DXGI_FORMAT_R16G16_UNORM;
		case Format::RGBA16_UNORM:		return DXGI_FORMAT_R16G16B16A16_UNORM;
		case Format::R16_SNORM:			return DXGI_FORMAT_R16_SNORM;
		case Format::RG16_SNORM:		return DXGI_FORMAT_R16G16_SNORM;
		case Format::RGBA16_SNORM:		return DXGI_FORMAT_R16G16B16A16_SNORM;
		case Format::R16_UINT:			return DXGI_FORMAT_R16_UINT;
		case Format::RG16_UINT:			return DXGI_FORMAT_R16G16_UINT;
		case Format::RGBA16_UINT:		return DXGI_FORMAT_R16G16B16A16_UINT;
		case Format::R16_SINT:			return DXGI_FORMAT_R16_SINT;
		case Format::RG16_SINT:			return DXGI_FORMAT_R16G16_SINT;
		case Format::RGBA16_SINT:		return DXGI_FORMAT_R16G16B16A16_SINT;
		case Format::R16_FLOAT:			return DXGI_FORMAT_R16_FLOAT;
		case Format::RG16_FLOAT:		return DXGI_FORMAT_R16G16_FLOAT;
		case Format::RGBA16_FLOAT:		return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case Format::R32_UINT:			return DXGI_FORMAT_R32_UINT;
		case Format::RG32_UINT:			return DXGI_FORMAT_R32G32_UINT;
		case Format::RGBA32_UINT:		return DXGI_FORMAT_R32G32B32A32_UINT;
		case Format::R32_SINT:			return DXGI_FORMAT_R32_SINT;
		case Format::RG32_SINT:			return DXGI_FORMAT_R32G32_SINT;
		case Format::RGBA32_SINT:		return DXGI_FORMAT_R32G32B32A32_SINT;
		case Format::R32_FLOAT:			return DXGI_FORMAT_R32_FLOAT;
		case Format::RG32_FLOAT:		return DXGI_FORMAT_R32G32_FLOAT;
		case Format::RGBA32_FLOAT:		return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case Format::D16_UNORM:			return DXGI_FORMAT_D16_UNORM;
		case Format::D24_UNORM_S8_UINT:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case Format::D32_FLOAT:			return DXGI_FORMAT_D32_FLOAT;
		case Format::D32_FLOAT_S8_UINT:	return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT Texture::GetDXGIBufferFormat(Format format)
	{
		switch (format)
		{
		case Engine::Texture::Format::D16_UNORM:			return DXGI_FORMAT_R16_TYPELESS;
		case Engine::Texture::Format::D24_UNORM_S8_UINT:	return DXGI_FORMAT_R24G8_TYPELESS;
		case Engine::Texture::Format::D32_FLOAT:			return DXGI_FORMAT_R32_TYPELESS;
		case Engine::Texture::Format::D32_FLOAT_S8_UINT:	return DXGI_FORMAT_R32G8X24_TYPELESS;
		}
		return GetDXGIFormat(format);
	}

	DXGI_FORMAT Texture::GetDXGISRVFormat(Format format)
	{
		switch (format)
		{
		case Engine::Texture::Format::D16_UNORM:			return DXGI_FORMAT_R16_UNORM;
		case Engine::Texture::Format::D24_UNORM_S8_UINT:	return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case Engine::Texture::Format::D32_FLOAT:			return DXGI_FORMAT_R32_FLOAT;
		case Engine::Texture::Format::D32_FLOAT_S8_UINT:	return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		}
		return GetDXGIFormat(format);
	}

	uint32_t Texture::GetFormatSize(Format format)
	{
		switch (format)
		{
		case Format::R8_UNORM:			return 1 * 1;
		case Format::RG8_UNORM:			return 2 * 1;
		case Format::RGBA8_UNORM:		return 4 * 1;
		case Format::R8_SNORM:			return 1 * 1;
		case Format::RG8_SNORM:			return 2 * 1;
		case Format::RGBA8_SNORM:		return 4 * 1;
		case Format::R8_UINT:			return 1 * 1;
		case Format::RG8_UINT:			return 2 * 1;
		case Format::RGBA8_UINT:		return 4 * 1;
		case Format::R8_SINT:			return 1 * 1;
		case Format::RG8_SINT:			return 2 * 1;
		case Format::RGBA8_SINT:		return 4 * 1;

		case Format::R16_UNORM:			return 1 * 2;
		case Format::RG16_UNORM:		return 2 * 2;
		case Format::RGBA16_UNORM:		return 4 * 2;
		case Format::R16_SNORM:			return 1 * 2;
		case Format::RG16_SNORM:		return 2 * 2;
		case Format::RGBA16_SNORM:		return 4 * 2;
		case Format::R16_UINT:			return 1 * 2;
		case Format::RG16_UINT:			return 2 * 2;
		case Format::RGBA16_UINT:		return 4 * 2;
		case Format::R16_SINT:			return 1 * 2;
		case Format::RG16_SINT:			return 2 * 2;
		case Format::RGBA16_SINT:		return 4 * 2;
		case Format::R16_FLOAT:			return 1 * 2;
		case Format::RG16_FLOAT:		return 2 * 2;
		case Format::RGBA16_FLOAT:		return 4 * 2;

		case Format::R32_UINT:			return 1 * 4;
		case Format::RG32_UINT:			return 2 * 4;
		case Format::RGBA32_UINT:		return 4 * 4;
		case Format::R32_SINT:			return 1 * 4;
		case Format::RG32_SINT:			return 2 * 4;
		case Format::RGBA32_SINT:		return 4 * 4;
		case Format::R32_FLOAT:			return 1 * 4;
		case Format::RG32_FLOAT:		return 2 * 4;
		case Format::RGBA32_FLOAT:		return 4 * 4;

		case Format::D16_UNORM:			return 1 * 2;
		case Format::D24_UNORM_S8_UINT:	return 1 * 3 + 1 * 1;
		case Format::D32_FLOAT:			return 1 * 4;
		case Format::D32_FLOAT_S8_UINT:	return 1 * 4 + 1 * 4; // the S8 is actually S8X24
		}
		return 0;
	}

	bool Texture::IsDepthOrStencil(Format format)
	{
		switch (format)
		{
		case Format::D16_UNORM:
		case Format::D24_UNORM_S8_UINT:
		case Format::D32_FLOAT:
		case Format::D32_FLOAT_S8_UINT:
			return true;
		}
		return false;
	}

	Texture2D::Texture2D(const fs::path& path)
	{
		LoadFromFile(path);
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, Format format, unsigned char const* data) :
		m_Width(width), m_Height(height), m_Format(format)
	{
		GenTextureBuffer((void*)data);
		GenSRV();
	}

	void Texture2D::SetData(void* data, uint32_t size)
	{
		// TODO
	}

	void Texture2D::LoadFromFile(const fs::path& path)
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

		if (data == nullptr)
		{
			DBOUT("failed to load image " << path.c_str() << std::endl);
			return;
		}

		if (channels == 3)
		{
			channels = 4;
			data = T24To32(width, height, data);
		}
		m_Width = width; m_Height = height;
		switch (channels)
		{
		case 1: m_Format = Format::R8_UNORM;
		case 2: m_Format = Format::RG8_UNORM;
		case 4: m_Format = Format::RGBA8_UNORM;
		}

		GenTextureBuffer(data);
		GenSRV();

		RendererAPI::Get().GetContext()->GenerateMips(m_SRV.Get());

		stbi_image_free(data);
	}

	bool Texture2D::operator==(const Texture& other) const
	{
		Texture2D& o = (Texture2D&)other;
		return (m_SRV == o.m_SRV);
	}

	void Texture2D::GenTextureBuffer(void* data, uint32_t numMipMaps)
	{
		if (data == nullptr)
			data = new uint8_t[m_Width * m_Height * GetFormatSize(m_Format)]{0};

		RendererAPI& graphics = RendererAPI::Get();

		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = m_Width;
		textureDesc.Height = m_Height;
		textureDesc.MipLevels = numMipMaps;
		textureDesc.ArraySize = 1;
		textureDesc.Format = GetDXGIBufferFormat(m_Format);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		if (IsDepthStencilTexture())
		{
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			textureDesc.MiscFlags = 0;
		}
		else
		{
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		textureDesc.CPUAccessFlags = 0;


		HRESULT hr = graphics.GetDivice()->CreateTexture2D(&textureDesc, nullptr, m_Buffer.GetAddressOf());
		if (FAILED(hr)) {
			DBOUT("failed to create texture");
			return;
		}

		graphics.GetContext()->UpdateSubresource(m_Buffer.Get(), 0, nullptr, (const void*)data, m_Width * GetFormatSize(m_Format), 0);
	}

	void Texture2D::GenSRV()
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.Format = GetDXGISRVFormat(m_Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		HRESULT hr = graphics.GetDivice()->CreateShaderResourceView(m_Buffer.Get(), &srvDesc, m_SRV.GetAddressOf());
		if (FAILED(hr)) {
			DBOUT("failed to create texture resorce");
			return;
		}
	}

	Ref<Texture2D> Texture2D::Create(const fs::path& path)
	{
		return std::make_shared<Texture2D>(path);
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, Format format)
	{
		unsigned char* data = new unsigned char[width*height*4*sizeof(uint8_t)];
		for (uint32_t i = 0; i < width * height * sizeof(uint8_t) * 4; i++)
			data[i] = 0xff;

		Ref<Texture2D> tex = std::make_shared<Texture2D>(width, height, format, data);

		delete[] data;

		return tex;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, Format format, unsigned char const* data)
	{
		return std::make_shared<Texture2D>(width, height, format, data);
	}



}