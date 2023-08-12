#include "ShaderCompiler.h"
#include <string.h>
#include <fstream>

namespace Engine
{

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

	ShaderCompiler::SamplerInfo::WrapMode GetWrapMode(const std::string& str)
	{
		if (str == "repeat")
			return ShaderCompiler::SamplerInfo::WrapMode::Repeat;
		else if (str == "repeatMiror")
			return ShaderCompiler::SamplerInfo::WrapMode::MirroredRepeat;
		else if (str == "clamp")
			return ShaderCompiler::SamplerInfo::WrapMode::Clamp;
		return ShaderCompiler::SamplerInfo::WrapMode::Clamp;
	}

	ShaderCompiler::SamplerInfo::MinMagFilter GetFilter(const std::string& str)
	{
		if (str == "point")
			return ShaderCompiler::SamplerInfo::MinMagFilter::Point;
		else if (str == "linear")
			return ShaderCompiler::SamplerInfo::MinMagFilter::Linear;
		else if (str == "anisotropic")
			return ShaderCompiler::SamplerInfo::MinMagFilter::Anisotropic;
		return ShaderCompiler::SamplerInfo::MinMagFilter::Linear;
	}

	ShaderCompiler::CompiledShader ShaderCompiler::Compile(const std::string& src)
	{
		CompiledShader shader;

		std::stringstream srcStream(src);

		struct SectionInfo 
		{
			std::stringstream src;
			std::unordered_map<std::string, SamplerInfo> samplers;
		};

		std::unordered_map<std::string, SectionInfo> ss;
		SectionInfo* currSection = &ss["config"];

		std::string line;
		while (getline(srcStream, line))
		{
			std::vector<std::string> tokens = Tokenize(line);

			if(tokens.empty())
				continue;

			if (tokens[0] == "#section")
				currSection = &ss[tokens[1]];
			else if (tokens[0] == "StaticSampler")
			{
				// 0			 1	  2 3			 4 5 6 7 8 9   10 11  12 13
				// StaticSampler name = StaticSampler( U , V , Min ,  Mag )  ;

				std::string& name = tokens[1];
				std::string& u = tokens[5];
				std::string& v = tokens[7];
				std::string& min = tokens[9];
				std::string& mag = tokens[11];

				SamplerInfo info;
				info.U = GetWrapMode(u);
				info.V = GetWrapMode(v);

				info.Min = GetFilter(min);
				info.Mag = GetFilter(mag);

				info.bindInfo.name = name;

				currSection->samplers[name] = info;

				currSection->src << "SamplerState " << name << ";\n";
			}
			else
				currSection->src << line << '\n';
		}

		std::string commonSrc = ss["common"].src.str();
		for (auto& sectionSource : ss)
		{
			if (sectionSource.first != "config" && sectionSource.first != "common")
			{
				std::string sectionSrc = commonSrc + sectionSource.second.src.str();
				Shader::ShaderType type;
				if (sectionSource.first == "vertex")
					type = Shader::Vertex;
				else if (sectionSource.first == "pixel")
					type = Shader::Pixel;
				else
					continue;

				for (auto& sampler : sectionSource.second.samplers)
				{
					sampler.second.bindInfo.type = type;
					shader.samplers.push_back(sampler.second);
				}

				CompileShaderCode(sectionSrc, shader, type);

			}
		}

		return shader;
	}

	void ShaderCompiler::CompileShaderCode(const std::string& src, CompiledShader& shader, Shader::ShaderType type)
	{
		const char* target = nullptr;
		switch (type)
		{
		case Engine::Shader::Vertex:
			target = "vs_5_0";
			break;
		case Engine::Shader::Pixel:
			target = "ps_5_0";
			break;
		default:
			return;
		}
		uint32_t flags = 0;
#ifdef DEBUG
		flags |= D3DCOMPILE_DEBUG;
#elif defined(RELEASE)
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		ID3DBlob* blob;
		ID3DBlob* errors;
		HRESULT hr = D3DCompile(src.c_str(), src.size() * sizeof(char), nullptr, nullptr, nullptr, "main", target, flags, 0, &blob, &errors);
		if (FAILED(hr))
		{
			DBOUT((char*)errors->GetBufferPointer());
			DBOUT("failed to compile shaders");
			return;
		}

		switch (type)
		{
		case Engine::Shader::Vertex:
			shader.vertexShader = blob;
			break;
		case Engine::Shader::Pixel:
			shader.pixelShader = blob;
			break;
		}

		// reflection

		ID3D11ShaderReflection* pReflector = nullptr;
		hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);

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
					shader.bindings.push_back({cbDesc.Name, type, cbIndex});
			}
		}

		for (uint32_t srvIndex = 0; srvIndex < reflectDesc.BoundResources; srvIndex++)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			pReflector->GetResourceBindingDesc(srvIndex, &bindDesc);
			switch (bindDesc.Type)
			{
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
				shader.bindings.push_back({ bindDesc.Name, type, bindDesc.BindPoint });
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
				for (auto& sampler : shader.samplers)
				{
					if (sampler.bindInfo.type == type && sampler.bindInfo.name == bindDesc.Name)
					{
						sampler.bindInfo.point = bindDesc.BindPoint;
						break;
					}
				}
				break;
			}
		}

		if (type == Shader::Vertex)
		{
			uint32_t numInputParams = reflectDesc.InputParameters;

			for (uint32_t i = 0; i < numInputParams; i++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC ps;
				pReflector->GetInputParameterDesc(i, &ps);
				shader.inputSigniture.push_back({ ps.SemanticName, ps.SemanticIndex, GetFormatFromDesc(ps) });
			}
		}
	}

	std::vector<std::string> ShaderCompiler::Tokenize(const std::string& line)
	{
		const char delimiters[] = { ' ', '	', '\r', '\n' };
		const char reservedTokens[] = { "={};\"()," };

		std::vector<std::string> tokens;

		bool lastCharReserved = false;
		std::string token;
		for (const char& c : line)
		{
			bool endToken = false;
			// check if it is a delimiter
			bool delimiter = false;
			for (uint32_t i = 0; i < _countof(delimiters); i++)
			{
				if (c == delimiters[i])
				{
					endToken = true;
					delimiter = true;
					break;
				}
			}

			// check if reserved token
			bool reserved = false;
			for (uint32_t i = 0; i < _countof(reservedTokens); i++)
			{
				if (c == reservedTokens[i])
				{
					reserved = true;
					break;
				}
			}

			// combine char with last
			if (reserved && lastCharReserved && !endToken)
			{
				endToken = true;
				//if (token.back() == '=')
			}

			if (lastCharReserved != reserved)
				endToken = true;

			if (endToken)
			{
				if (!token.empty())
				{
					tokens.push_back(token);
					token.clear();
				}
				if (!delimiter)
					token.push_back(c);
				lastCharReserved = reserved;
			}
			else
				token.push_back(c);
		}

		if (!token.empty())
			tokens.push_back(token); // add the last token

		return tokens;
	}

}

