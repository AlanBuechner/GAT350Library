#include "Shader.h"
#include "RendererAPI.h"
#include "Buffer.h"

namespace Engine
{
	DXGI_FORMAT ShaderDataTypeToDXGIFormat(ShaderDataType& type)
	{
		switch (type)
		{
		case ShaderDataType::Float:			return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:		return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:		return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:		return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case ShaderDataType::Int:			return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:			return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:			return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:			return DXGI_FORMAT_R32G32B32A32_SINT;
		default:
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT GetFormatFromDesc(D3D11_SIGNATURE_PARAMETER_DESC& desc)
	{
		if (desc.Mask == 1)
		{
			if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32_UINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32_SINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32_FLOAT;
		}
		else if (desc.Mask <= 3)
		{
			if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32_UINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32_SINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (desc.Mask <= 7)
		{
			if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32_UINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32_SINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (desc.Mask <= 15)
		{
			if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32A32_UINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32A32_SINT;
			else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	Shader::Shader(const ShaderSource& src) :
		m_VertexShaderFile(src.VetexShader)
	{
		LoadShader(src.VetexShader, ShaderType::Vertex);
		LoadShader(src.PixelShader, ShaderType::Pixel);

		GenInputLayoutFromReflection();

		RendererAPI& graphics = RendererAPI::Get();

		D3D11_DEPTH_STENCIL_DESC dsstate_desc = {};
		dsstate_desc.DepthEnable = true;
		dsstate_desc.StencilEnable = false;
		dsstate_desc.DepthFunc = D3D11_COMPARISON_LESS;
		dsstate_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsstate_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsstate_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsstate_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsstate_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsstate_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsstate_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		graphics.GetDivice()->CreateDepthStencilState(&dsstate_desc, m_DepthStencilState.GetAddressOf());

		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.AntialiasedLineEnable = false;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.DepthClipEnable = false;
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.MultisampleEnable = false;
		rasterizer_desc.ScissorEnable = false;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		graphics.GetDivice()->CreateRasterizerState(&rasterizer_desc, m_RasterizerState.GetAddressOf());
	}

	void Shader::GenInputLayoutFromReflection()
	{
		RendererAPI& graphics = RendererAPI::Get();

		wrl::ComPtr<ID3DBlob> pBlob = ReadBlob(m_VertexShaderFile);

		// reflect on the shader
		ID3D11ShaderReflection* pReflector = nullptr;
		HRESULT hr = D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);

		// check if the reflection failed
		if (FAILED(hr)) {
			DBOUT("shader reflection failed" << std::endl);
			return;
		}

		// get the descriptor for the shader
		D3D11_SHADER_DESC reflectDesc;
		pReflector->GetDesc(&reflectDesc);

		uint32_t numInputParams = reflectDesc.InputParameters;
		D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[numInputParams];

		for (uint32_t i = 0; i < numInputParams; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC ps;
			pReflector->GetInputParameterDesc(i, &ps);

			ied[i] = { ps.SemanticName, ps.SemanticIndex, GetFormatFromDesc(ps), 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}

		hr = graphics.GetDivice()->CreateInputLayout(ied, numInputParams, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pInputLayout);

		if (FAILED(hr)) {
			DBOUT("failed to create layout from reflection" << std::endl);
		}

		delete[] ied;
	}

	bool Shader::operator==(const Shader& other)
	{
		return m_pVertexShader == other.m_pVertexShader && m_pPixelShader == other.m_pPixelShader;
	}

	void Shader::LoadShader(const std::wstring& file, ShaderType type)
	{
		RendererAPI& graphics = RendererAPI::Get();

		wrl::ComPtr<ID3DBlob> pBlob = ReadBlob(file);

		switch (type)
		{
		case Engine::Shader::ShaderType::Vertex:
			graphics.GetDivice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVertexShader);
			break;
		case Engine::Shader::ShaderType::Pixel:
			graphics.GetDivice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader);
			break;
		default:
			break;
		}

		GenBuffers(pBlob, type);
	}

	void Shader::GenBuffers(wrl::ComPtr<ID3DBlob> pBlob, ShaderType type)
	{
		ID3D11ShaderReflection* pReflector = nullptr;
		HRESULT hr = D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);

		// check if the reflection failed
		if (FAILED(hr)) {
			DBOUT("shader reflection failed" << std::endl);
			return;
		}

		// get the descriptor for the shader
		D3D11_SHADER_DESC reflectDesc;
		pReflector->GetDesc(&reflectDesc);

		// iterate over all the constant buffers
		for (uint32_t cbIndex = 0; cbIndex < reflectDesc.ConstantBuffers; cbIndex++)
		{
			// get the constant buffer
			ID3D11ShaderReflectionConstantBuffer* cb = pReflector->GetConstantBufferByIndex(cbIndex);
			if (cb)
			{
				// get the descriptor of the constant buffer
				D3D11_SHADER_BUFFER_DESC cbDesc;
				cb->GetDesc(&cbDesc);
				if (cbDesc.Type == D3D11_CT_CBUFFER)
					AddBindPoint(cbDesc.Name, type, cbIndex);
			}
		}

		// iterate over all BoundResources
		for (uint32_t srvIndex = 0; srvIndex < reflectDesc.BoundResources; srvIndex++)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			pReflector->GetResourceBindingDesc(srvIndex, &bindDesc);
			if (bindDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED)
				AddBindPoint(bindDesc.Name, type, bindDesc.BindPoint);
		}
	}

	void Shader::AddBindPoint(const std::string& name, ShaderType type, uint32_t slot)
	{
		BindPointInfo info = {};
		if (m_BindPoints.find(name) != m_BindPoints.end())
			info = m_BindPoints.at(name);
		info.type = (ShaderType)((uint32_t)info.type | (uint32_t)type);
		switch (type)
		{
		case ShaderType::Vertex:
			info.vertexSlot = slot;
			break;
		case ShaderType::Pixel:
			info.pixelSlot = slot;
			break;
		}

		// create a new constant buffer object and added it to the map
		m_BindPoints[name] = info;
	}

	wrl::ComPtr<ID3DBlob> Shader::ReadBlob(const std::wstring& fileName)
	{
		wrl::ComPtr<ID3DBlob> pBlob;
		HRESULT hr = D3DReadFileToBlob((L"ShaderBin/" + fileName).c_str(), &pBlob);

		if (FAILED(hr))
			DBOUT("failed to read from file \"ShaderBin/" << fileName << "\"" << std::endl);

		return pBlob;
	}

	Ref<Shader> Shader::Create(const ShaderSource& src)
	{
		return std::make_shared<Shader>(src);
	}

}