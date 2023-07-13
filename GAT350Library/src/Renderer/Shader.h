#pragma once

#include "Platform/Windows/Win.h"
#include "Core/Core.h"
#include <string>
#include <unordered_map>

namespace Engine
{
	struct ShaderSource
	{
		std::wstring VetexShader;
		std::wstring PixelShader;
	};

	class Shader
	{
	public:
		enum ShaderType
		{
			Vertex = BIT(0),
			Pixel = BIT(1)
		};

		struct BindPointInfo
		{
			ShaderType type;
			uint32_t vertexSlot;
			uint32_t pixelSlot;
		};

		Shader(const ShaderSource& src);

		void GenInputLayoutFromReflection();

		BindPointInfo GetBindPoint(const std::string& name) { return m_BindPoints[name]; }
		wrl::ComPtr<ID3D11InputLayout> GetInputLayout() { return m_pInputLayout; }
		wrl::ComPtr<ID3D11VertexShader> GetVertexShader() { return m_pVertexShader; }
		wrl::ComPtr<ID3D11PixelShader> GetPixelShader() { return m_pPixelShader; }
		wrl::ComPtr<ID3D11DepthStencilState> GetDepthStencilState() { return m_DepthStencilState; }
		wrl::ComPtr<ID3D11RasterizerState> GetRasterizerState() { return m_RasterizerState; }

		bool operator==(const Shader& other);

		static Ref<Shader> Create(const ShaderSource& src);

	private:

		void LoadShader(const std::wstring& file, ShaderType type);
		void GenBuffers(wrl::ComPtr<ID3DBlob> pBlob, ShaderType type);
		void AddBindPoint(const std::string& name, ShaderType type, uint32_t slot);

		wrl::ComPtr<ID3DBlob> ReadBlob(const std::wstring& fileName);

	private:

		std::wstring m_VertexShaderFile;
		wrl::ComPtr<ID3D11InputLayout> m_pInputLayout;

		wrl::ComPtr<ID3D11VertexShader> m_pVertexShader;
		wrl::ComPtr<ID3D11PixelShader> m_pPixelShader;

		wrl::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
		wrl::ComPtr<ID3D11RasterizerState> m_RasterizerState;

		std::unordered_map<std::string, BindPointInfo> m_BindPoints;
	};
	
}