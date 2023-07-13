#include "RendererAPI.h"

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

Engine::RendererAPI* Engine::RendererAPI::s_Instance;

namespace Engine
{
	void RendererAPI::Init()
	{
		if (s_Instance == nullptr)
			s_Instance = new RendererAPI();
		else
			DBOUT("renderer api allready created!");
	}

	RendererAPI::RendererAPI()
	{
		D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr, 0, nullptr, 0,
			D3D11_SDK_VERSION,
			&pDevice,
			nullptr,
			&pContext
		);

		IDXGIDevice* dxgiDevice = 0;
		pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

		IDXGIAdapter* dxgiAdapter = 0;
		dxgiDevice->GetAdapter(&dxgiAdapter);

		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);

		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

}