#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"

#include "Texture.h"

namespace Engine
{
	class RenderTarget : Texture2D
	{
	public:
		RenderTarget(uint32_t width, uint32_t height, Format format);

		void Resize(uint32_t width, uint32_t height);

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual bool IsDepthStencilTexture() override { return Texture2D::IsDepthStencilTexture(); }

		wrl::ComPtr<ID3D11RenderTargetView> GetRTV() { return m_RTV; }
		wrl::ComPtr<ID3D11DepthStencilView> GetDSV() { return m_DSV; }

		static Ref<RenderTarget> Create(uint32_t width, uint32_t height, Format format);

	private:

		void GenRTVDSV();

	private:
		wrl::ComPtr<ID3D11RenderTargetView> m_RTV;
		wrl::ComPtr<ID3D11DepthStencilView> m_DSV;
	};
}