#pragma once
#include "Core/Core.h"
#include "Platform/Windows/Win.h"

namespace Engine
{
	class RendererAPI
	{
	private:
		static RendererAPI* s_Instance;

	public:
		static void Init();
		static inline RendererAPI& Get() { return *s_Instance; }

		RendererAPI();

		inline wrl::ComPtr<ID3D11Device> GetDivice() { return pDevice; }
		inline wrl::ComPtr<ID3D11DeviceContext> GetContext() { return pContext; }
		inline wrl::ComPtr<IDXGIFactory> GetFactory() { return pDXGIFactory; }

	private:
		wrl::ComPtr<ID3D11Device> pDevice;
		wrl::ComPtr<ID3D11DeviceContext> pContext;
		wrl::ComPtr<IDXGIFactory> pDXGIFactory;
		
	};
}