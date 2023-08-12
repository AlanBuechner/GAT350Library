#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"
#include "Shader.h"

namespace Engine
{
	class ShaderCompiler
	{
	public:

		enum class CullMode
		{
			None,
			Front,
			Back
		};

		enum class DepthTestFunc
		{
			Never,
			Equal,
			Not_Equal,
			Less,
			Less_Equal,
			Greater,
			Greater_Equal,
			Always
		};

		struct BindingInfo
		{
			std::string name;
			Shader::ShaderType type;
			uint32_t point;
		};


		struct SamplerInfo
		{
			enum class WrapMode
			{
				Repeat,
				MirroredRepeat,
				Clamp
			};

			enum class MinMagFilter
			{
				Point,
				Linear,
				Anisotropic
			};

			WrapMode U = WrapMode::Repeat, V = WrapMode::Repeat;

			MinMagFilter Min = MinMagFilter::Linear, Mag = MinMagFilter::Linear;
			BindingInfo bindInfo;
		};

		struct ShaderConfig
		{
			CullMode cullMode = CullMode::Back;
			DepthTestFunc depthTestFunc = DepthTestFunc::Less;
		};

		struct InputElement
		{
			std::string semanticName;
			uint32_t semanticIndex;
			DXGI_FORMAT format;
		};

		struct CompiledShader
		{
			ShaderConfig config;
			std::vector<BindingInfo> bindings;
			std::vector<SamplerInfo> samplers;
			std::vector<InputElement> inputSigniture;
			wrl::ComPtr<ID3DBlob> vertexShader;
			wrl::ComPtr<ID3DBlob> pixelShader;
		};

		static CompiledShader Compile(const std::string& src);

	private:

		static void CompileShaderCode(const std::string& src, CompiledShader& shader, Shader::ShaderType type);

		static std::vector<std::string> Tokenize(const std::string& line);

	};
}