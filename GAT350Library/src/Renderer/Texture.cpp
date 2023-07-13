#include "Texture.h"
#include "RendererAPI.h"
#include "stb_image.h"

namespace Engine 
{

	DXGI_FORMAT GetFormat(int channels)
	{
		switch (channels)
		{
		case 1:
			return DXGI_FORMAT_R8_UNORM;
		case 2:
			return DXGI_FORMAT_R8G8_UNORM;
		case 3:
		case 4:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		default:
			break;
		}
	}

	D3D11_FILTER GetMinMagFilter(TextureAttribute::MinMagFilter min, TextureAttribute::MinMagFilter mag)
	{
		switch (min)
		{
		case Engine::TextureAttribute::MinMagFilter::Nearest:
			switch (mag)
			{
			case Engine::TextureAttribute::MinMagFilter::Nearest:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			case Engine::TextureAttribute::MinMagFilter::Linear:
				return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			}
		case Engine::TextureAttribute::MinMagFilter::Linear:
			switch (mag)
			{
			case Engine::TextureAttribute::MinMagFilter::Nearest:
				return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case Engine::TextureAttribute::MinMagFilter::Linear:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}
	}

	D3D11_TEXTURE_ADDRESS_MODE GetAddressMode(TextureAttribute::WrapMode mode)
	{
		switch (mode)
		{
		case Engine::TextureAttribute::WrapMode::Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case Engine::TextureAttribute::WrapMode::MirroredRepeat:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case Engine::TextureAttribute::WrapMode::ClampToEdge:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}
	}

	uint32_t ChannelSize(int channels)
	{
		if (channels == 3)
			channels = 4;
		return channels * sizeof(uint8_t);
	}

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

	Texture2D::Texture2D(const std::string& path, Ref<TextureAttribute> attribute) :
		m_Attribute(attribute)
	{
		LoadFromFile(path);
		GenSampler();
	}

	Texture2D::Texture2D(const uint32_t width, const uint32_t height, unsigned char* data, Ref<TextureAttribute> attribute) :
		m_Attribute(attribute)
	{
		GenTextureView((void*)data, width, height, 4);
		GenSampler();
	}

	void Texture2D::SetData(void* data, uint32_t size)
	{

	}

	void Texture2D::LoadFromFile(const std::string& path)
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		m_Width = width; m_Height = height;

		if (data == nullptr)
		{
			DBOUT("failed to load image " << path.c_str() << std::endl);
			return;
		}

		if (channels == 3)
			data = T24To32(width, height, data);

		GenTextureView(data, width, height, channels);

		stbi_image_free(data);

	}

	bool Texture2D::operator==(const Texture& other) const
	{
		Texture2D& o = (Texture2D&)other;
		return (m_pTextureView == o.m_pTextureView) && (m_pSampler == o.m_pSampler);
	}

	void Texture2D::GenTextureView(void* data, uint32_t width, uint32_t height, uint32_t channels)
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = GetFormat(channels);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA sd = { 0 };
		sd.pSysMem = (void*)data;
		sd.SysMemPitch = width * ChannelSize(channels);

		wrl::ComPtr<ID3D11Texture2D> pTexture;
		HRESULT hr = graphics.GetDivice()->CreateTexture2D(&textureDesc, &sd, &pTexture);
		if (FAILED(hr)) {
			DBOUT("failed to create texture");
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		hr = graphics.GetDivice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pTextureView);
		if (FAILED(hr)) {
			DBOUT("failed to create texture resorce");
			return;
		}
	}

	void Texture2D::GenSampler()
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = GetMinMagFilter(m_Attribute->Min, m_Attribute->Mag);
		samplerDesc.AddressU = GetAddressMode(m_Attribute->U);
		samplerDesc.AddressV = GetAddressMode(m_Attribute->V);
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		graphics.GetDivice()->CreateSamplerState(&samplerDesc, &m_pSampler);
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, TextureAttribute& attribute)
	{
		return std::make_shared<Texture2D>(path, std::make_shared<TextureAttribute>(attribute));
	}

	Ref<Texture2D> Texture2D::Create(const uint32_t width, const uint32_t height, TextureAttribute& attribute)
	{
		unsigned char* data = new unsigned char[width*height*4*sizeof(uint8_t)];
		for (uint32_t i = 0; i < width * height * sizeof(uint8_t) * 4; i++)
			data[i] = 0xff;

		Ref<Texture2D> tex = std::make_shared<Texture2D>(width, height, data, std::make_shared<TextureAttribute>(attribute));

		delete[] data;

		return tex;
	}

	Ref<Texture2D> Texture2D::Create(const uint32_t width, const uint32_t height, unsigned char* data, TextureAttribute& attribute)
	{
		return std::make_shared<Texture2D>(width, height, data, std::make_shared<TextureAttribute>(attribute));
	}
}