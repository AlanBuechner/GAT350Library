#include "RenderTarget.h"
#include "RendererAPI.h"

namespace Engine
{


	RenderTarget::RenderTarget(uint32_t width, uint32_t height, Format format) :
		Texture2D(format)
	{
		Resize(width, height);
	}

	void RenderTarget::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		GenTextureBuffer(nullptr, D3D11_BIND_RENDER_TARGET);
		GenRTVDSV();
		GenSRV();
	}

	void RenderTarget::GenRTVDSV()
	{
		RendererAPI& graphics = RendererAPI::Get();

		if (Texture::IsDepthOrStencil(m_Format))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = Texture::GetDXGIFormat(m_Format);
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			HRESULT hr = graphics.GetDivice()->CreateDepthStencilView(m_Buffer.Get(), &dsvDesc, m_DSV.GetAddressOf());
			if (FAILED(hr)) {
				DBOUT(TranslateError(hr));
				return;
			}
		}
		else
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = Texture::GetDXGIFormat(m_Format);
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			HRESULT hr = graphics.GetDivice()->CreateRenderTargetView(m_Buffer.Get(), &rtvDesc, m_RTV.GetAddressOf());
			if (FAILED(hr)) {
				DBOUT(TranslateError(hr));
				return;
			}
		}
	}

	Ref<RenderTarget> RenderTarget::Create(uint32_t width, uint32_t height, Format format)
	{
		return std::make_shared<RenderTarget>(width, height, format);
	}

}

