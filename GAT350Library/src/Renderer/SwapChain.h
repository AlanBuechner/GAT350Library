#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"

#include <stdarg.h>

namespace Engine
{
	class SwapChain
	{
	public:

		void Init(void* windowHandle, uint32_t width, uint32_t height);

		void Resize(uint32_t width, uint32_t height);

		void SetVSync(bool val) { m_VSync = val; }

		void SwapBuffers();

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

		wrl::ComPtr<ID3D11RenderTargetView> GetRTV() { return m_RTV; }

		static SwapChain& CreateSwapChain();

	private:
		void CreateDepthBuffer(uint32_t width, uint32_t height);

	private:

		wrl::ComPtr<IDXGISwapChain> m_Swap; // pointer to the swap chain 
		wrl::ComPtr<ID3D11RenderTargetView> m_RTV; // pointer to the render target

		uint32_t m_Width, m_Height;

		bool m_VSync = true;

	};
}