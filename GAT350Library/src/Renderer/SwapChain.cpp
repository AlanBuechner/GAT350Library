#include "SwapChain.h"
#include "RendererAPI.h"

namespace Engine
{

	void SwapChain::Init(void* windowHandle, uint32_t width, uint32_t height)
	{
		RendererAPI& graphics = RendererAPI::Get();

		m_Width = width;
		m_Height = height;

		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format to store the data
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // unspecified no scaling required 
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // unspecified 
		sd.SampleDesc.Count = 1; // no anti aliasing
		sd.SampleDesc.Quality = 0; // no anti aliasing
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1; // double buffering
		sd.OutputWindow = (HWND)windowHandle; // window to output to
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		graphics.GetFactory()->CreateSwapChain((IUnknown*)graphics.GetDivice().Get(), &sd, &m_Swap);

		m_Swap->GetBuffer(0, __uuidof(ID3D11Resource), &m_BackBuffer);

		graphics.GetDivice()->CreateRenderTargetView(m_BackBuffer.Get(), nullptr, m_RTV.GetAddressOf());
	}

	void SwapChain::Resize(uint32_t width, uint32_t height)
	{
		if (m_Swap && m_RTV)
		{
			RendererAPI& graphics = RendererAPI::Get();

			m_Width = width, m_Height = height;

			DXGI_MODE_DESC md = { 0 };
			md.Width = width;
			md.Height = height;
			md.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format to store the data
			md.RefreshRate.Numerator = 0;
			md.RefreshRate.Denominator = 0;
			md.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // unspecified no scaling required 
			md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // unspecified 

			// unbind the render target
			graphics.GetContext()->OMSetRenderTargets(0, 0, 0);
			// release reference to the render target
			m_RTV->Release();

			// flush the context
			graphics.GetContext()->Flush();

			// resize the target and swap chain buffers
			m_Swap->ResizeTarget(&md);
			m_Swap->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			// recreate the render target
			m_Swap->GetBuffer(0, __uuidof(ID3D11Resource), &m_BackBuffer);

			graphics.GetDivice()->CreateRenderTargetView(m_BackBuffer.Get(), nullptr, m_RTV.GetAddressOf());
		}
	}

	void SwapChain::SwapBuffers()
	{
		m_Swap->Present((m_VSync ? 1u : 0u), 0u);
	}

	SwapChain& SwapChain::CreateSwapChain()
	{
		
		return *new SwapChain();
	}

}