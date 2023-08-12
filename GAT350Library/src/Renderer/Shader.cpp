#include "Shader.h"
#include "RendererAPI.h"
#include "Buffer.h"
#include "ShaderCompiler.h"

namespace Engine
{

	D3D11_FILTER GetMinMagFilter(ShaderCompiler::SamplerInfo::MinMagFilter min, ShaderCompiler::SamplerInfo::MinMagFilter mag)
	{
		if (min == ShaderCompiler::SamplerInfo::MinMagFilter::Anisotropic || mag == ShaderCompiler::SamplerInfo::MinMagFilter::Anisotropic)
			return D3D11_FILTER_ANISOTROPIC;

		switch (min)
		{
		case ShaderCompiler::SamplerInfo::MinMagFilter::Point:
			switch (mag)
			{
			case ShaderCompiler::SamplerInfo::MinMagFilter::Point:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			case ShaderCompiler::SamplerInfo::MinMagFilter::Linear:
				return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			}
		case ShaderCompiler::SamplerInfo::MinMagFilter::Linear:
			switch (mag)
			{
			case ShaderCompiler::SamplerInfo::MinMagFilter::Point:
				return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case ShaderCompiler::SamplerInfo::MinMagFilter::Linear:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}
	}

	D3D11_TEXTURE_ADDRESS_MODE GetAddressMode(ShaderCompiler::SamplerInfo::WrapMode mode)
	{
		switch (mode)
		{
		case ShaderCompiler::SamplerInfo::WrapMode::Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case ShaderCompiler::SamplerInfo::WrapMode::MirroredRepeat:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case ShaderCompiler::SamplerInfo::WrapMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}
	}

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

	Shader::Shader(const fs::path& path)
	{

		std::ifstream file(path);
		file.seekg(0, std::ios::end);
		size_t size = file.tellg();
		std::string src(size, ' ');
		file.seekg(0);
		file.read(&src[0], size);
		LoadFromSrc(src);
	}

	Shader::Shader(const std::string& src)
	{
		LoadFromSrc(src);
	}

	bool Shader::operator==(const Shader& other)
	{
		return m_VertexShader == other.m_VertexShader && m_PixelShader == other.m_PixelShader;
	}

	void Shader::LoadFromSrc(const std::string& src)
	{
		RendererAPI& graphics = RendererAPI::Get();
		ShaderCompiler::CompiledShader compiledShader = ShaderCompiler::Compile(src);

		graphics.GetDivice()->CreateVertexShader(compiledShader.vertexShader->GetBufferPointer(), compiledShader.vertexShader->GetBufferSize(), nullptr, &m_VertexShader);
		graphics.GetDivice()->CreatePixelShader(compiledShader.pixelShader->GetBufferPointer(), compiledShader.pixelShader->GetBufferSize(), nullptr, &m_PixelShader);

		for (ShaderCompiler::BindingInfo binding : compiledShader.bindings)
			AddBindPoint(binding.name, binding.type, binding.point);

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied(compiledShader.inputSigniture.size());
		for (uint32_t i = 0; i < compiledShader.inputSigniture.size(); i++)
		{
			ied[i] = {
				compiledShader.inputSigniture[i].semanticName.c_str(),
				compiledShader.inputSigniture[i].semanticIndex,
				compiledShader.inputSigniture[i].format,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			};
		}

		HRESULT hr = graphics.GetDivice()->CreateInputLayout(ied.data(), (uint32_t)ied.size(), compiledShader.vertexShader->GetBufferPointer(), compiledShader.vertexShader->GetBufferSize(), &m_InputLayout);
		if (FAILED(hr)) {
			DBOUT("failed to create layout from reflection" << std::endl);
			return;
		}

		D3D11_DEPTH_STENCIL_DESC dsstate_desc = {};
		dsstate_desc.DepthEnable = true;
		dsstate_desc.StencilEnable = false;
		switch (compiledShader.config.depthTestFunc)
		{
		case ShaderCompiler::DepthTestFunc::Never:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_NEVER;
			break;
		case ShaderCompiler::DepthTestFunc::Equal:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
			break;
		case ShaderCompiler::DepthTestFunc::Not_Equal:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
			break;
		case ShaderCompiler::DepthTestFunc::Less:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_LESS;
			break;
		case ShaderCompiler::DepthTestFunc::Less_Equal:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			break;
		case ShaderCompiler::DepthTestFunc::Greater:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_GREATER;
			break;
		case ShaderCompiler::DepthTestFunc::Greater_Equal:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			break;
		case ShaderCompiler::DepthTestFunc::Always:
			dsstate_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			break;
		}
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
		switch (compiledShader.config.cullMode)
		{
		case ShaderCompiler::CullMode::None:
			rasterizer_desc.CullMode = D3D11_CULL_NONE;
			break;
		case ShaderCompiler::CullMode::Front:
			rasterizer_desc.CullMode = D3D11_CULL_FRONT;
			break;
		case ShaderCompiler::CullMode::Back:
			rasterizer_desc.CullMode = D3D11_CULL_BACK;
			break;

		}
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.DepthClipEnable = false;
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.MultisampleEnable = false;
		rasterizer_desc.ScissorEnable = false;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		graphics.GetDivice()->CreateRasterizerState(&rasterizer_desc, m_RasterizerState.GetAddressOf());

		// create samplers
		for (ShaderCompiler::SamplerInfo info : compiledShader.samplers)
		{
			wrl::ComPtr<ID3D11SamplerState> sampler;
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = GetMinMagFilter(info.Min, info.Mag);
			samplerDesc.AddressU = GetAddressMode(info.U);
			samplerDesc.AddressV = GetAddressMode(info.V);
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			hr = graphics.GetDivice()->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
			if (FAILED(hr)) {
				DBOUT("failed to create sampler" << std::endl);
				continue;
			}

			Sampler s;
			s.sampler = sampler;
			s.info.type = info.bindInfo.type;
			s.info.pixelSlot = info.bindInfo.point;
			s.info.vertexSlot = info.bindInfo.point;
			m_Samplers.push_back(s);
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

	Ref<Shader> Shader::Create(const fs::path& path)
	{
		return std::make_shared<Shader>(path);
	}

	Ref<Shader> Shader::CreateFromSrc(const std::string& src)
	{
		return std::make_shared<Shader>(src);
	}

}