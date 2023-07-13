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

		graphics.GetFactory()->CreateSwapChain((IUnknown*)graphics.GetDivice().Get(), &sd, &pSwap);

		wrl::ComPtr<ID3D11Resource> pBackBuffer;
		pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);

		graphics.GetDivice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

		CreateDepthBuffer(width, height);
	}

	void SwapChain::Resize(uint32_t width, uint32_t height)
	{
		if (pSwap && pTarget && pDSV)
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
			pTarget->Release();

			// flush the context
			graphics.GetContext()->Flush();

			// resize the target and swap chain buffers
			pSwap->ResizeTarget(&md);
			pSwap->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			// recreate the render target
			wrl::ComPtr<ID3D11Resource> pBackBuffer;
			pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);

			graphics.GetDivice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.GetAddressOf());

			// recreate the depth buffer
			CreateDepthBuffer(width, height);
		}
	}

	void SwapChain::ClearColor(float r, float g, float b, float a /*= 1.0f*/)
	{
		RendererAPI& graphics = RendererAPI::Get();
		const float color[] = { r,g,b,a };
		graphics.GetContext()->ClearRenderTargetView(pTarget.Get(), color);
		graphics.GetContext()->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void SwapChain::SwapBuffers()
	{
		pSwap->Present((m_VSync ? 1u : 0u), 0u);
	}

	SwapChain& SwapChain::CreateSwapChain()
	{
		
		return *new SwapChain();
	}

	void SwapChain::CreateDepthBuffer(uint32_t width, uint32_t height)
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		wrl::ComPtr<ID3D11DepthStencilState> pDSState;
		HRESULT hr = graphics.GetDivice()->CreateDepthStencilState(&dsDesc, &pDSState);
		if (FAILED(hr)) {
			DBOUT("failed to create depth stencil state" << std::endl);
		}

		graphics.GetContext()->OMSetDepthStencilState(pDSState.Get(), 1u);

		wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		graphics.GetDivice()->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;

		graphics.GetDivice()->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV);
	}

}