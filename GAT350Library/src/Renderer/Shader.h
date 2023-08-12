#pragma once

#include "Platform/Windows/Win.h"
#include "Core/Core.h"
#include <string>
#include <unordered_map>

namespace Engine
{
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

		struct Sampler 
		{
			BindPointInfo info;
			wrl::ComPtr<ID3D11SamplerState> sampler;
		};

		Shader(const fs::path& path);
		Shader(const std::string& src);

		BindPointInfo GetBindPoint(const std::string& name) { return m_BindPoints[name]; }
		wrl::ComPtr<ID3D11InputLayout> GetInputLayout() { return m_InputLayout; }
		wrl::ComPtr<ID3D11VertexShader> GetVertexShader() { return m_VertexShader; }
		wrl::ComPtr<ID3D11PixelShader> GetPixelShader() { return m_PixelShader; }
		wrl::ComPtr<ID3D11DepthStencilState> GetDepthStencilState() { return m_DepthStencilState; }
		wrl::ComPtr<ID3D11RasterizerState> GetRasterizerState() { return m_RasterizerState; }
		std::vector<Sampler>& GetSamplers() { return m_Samplers; }

		bool operator==(const Shader& other);

		static Ref<Shader> Create(const fs::path& path);
		static Ref<Shader> CreateFromSrc(const std::string& src);

	private:

		void LoadFromSrc(const std::string& src);

		void AddBindPoint(const std::string& name, ShaderType type, uint32_t slot);

	private:
		wrl::ComPtr<ID3D11InputLayout> m_InputLayout;

		wrl::ComPtr<ID3D11VertexShader> m_VertexShader;
		wrl::ComPtr<ID3D11PixelShader> m_PixelShader;

		wrl::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
		wrl::ComPtr<ID3D11RasterizerState> m_RasterizerState;

		std::vector<Sampler> m_Samplers;

		std::unordered_map<std::string, BindPointInfo> m_BindPoints;
	};
	
}