#include "RendererCommand.h"
#include "RendererAPI.h"
#include "Core/Core.h"
#include "Platform/Windows/Win.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Shader.h"
#include "SwapChain.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "FrameBuffer.h"
#include "MeshBuilder.h"

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")

Engine::Ref<Engine::Shader> Engine::RendererCommand::s_BlitShader;
Engine::Ref<Engine::Mesh> Engine::RendererCommand::s_ScreenMesh;

namespace Engine
{
	void RendererCommand::Init()
	{
		Time::Init(); // just to make things easier for the students
		RendererAPI::Init();

		s_BlitShader = Shader::CreateFromSrc(R"(
			#section config

			#section common
			struct VS_Input
			{
				float4 position : POSITION;
				float3 normal : NORMAL;
				float3 tangent : TANGENT;
				float2 uv : UV;
			};
			
			struct VS_Output
			{
				float4 position : SV_POSITION;
				float2 uv : TEXTCOORD;
			};
			
			typedef VS_Output PS_Input;
			
			#section vertex
			
			VS_Output main(VS_Input input)
			{
				VS_Output output;

				output.position = input.position;
				output.uv = input.uv;
			
				return output;
			}
			
			#section pixel
			
			struct PS_Output
			{
				float4 color : SV_TARGET0;
			};
			
			Texture2D<float4> tex : register(t0);
			
			StaticSampler textureSampler = StaticSampler(repeat, repeat, linear, linear);
			
			PS_Output main(PS_Input input)
			{
				PS_Output output;
			
				output.color = tex.Sample(textureSampler, input.uv);
				//output.color = float4(input.uv, 1, 1);
			
				return output;
			}
		)");

		MeshBuilder mb;
		mb.m_Vertices.push_back({ {-1,-1,1,1}, {0,0,0}, {0,0,0}, {0,1} });
		mb.m_Vertices.push_back({ { 1,-1,1,1}, {0,0,0}, {0,0,0}, {1,1} });
		mb.m_Vertices.push_back({ {-1, 1,1,1}, {0,0,0}, {0,0,0}, {0,0} });
		mb.m_Vertices.push_back({ { 1, 1,1,1}, {0,0,0}, {0,0,0}, {1,0} });

		mb.m_Indices.push_back(2);
		mb.m_Indices.push_back(1);
		mb.m_Indices.push_back(3);
		mb.m_Indices.push_back(2);
		mb.m_Indices.push_back(0);
		mb.m_Indices.push_back(1);

		s_ScreenMesh = mb.Build();
	}

	void RendererCommand::SetSwapChain(SwapChain& swapChain)
	{
		RendererAPI& graphics = RendererAPI::Get();

		graphics.GetContext()->OMSetRenderTargets(1u, swapChain.GetRTV().GetAddressOf(), nullptr);
		SetViewPort(swapChain.GetWidth(), swapChain.GetHeight(), 0, 0);
	}

	void RendererCommand::ClearSwapChain(SwapChain& swapChain, glm::vec3 color)
	{
		RendererAPI& graphics = RendererAPI::Get();

		graphics.GetContext()->ClearRenderTargetView(swapChain.GetRTV().Get(), (float*)&color);
		SetViewPort(swapChain.GetWidth(), swapChain.GetHeight(), 0, 0);
	}

	void RendererCommand::SetRenderTerget(Ref<RenderTarget> renderTarget)
	{
		RendererAPI& graphics = RendererAPI::Get();

		if(renderTarget->IsDepthStencilTexture())
			graphics.GetContext()->OMSetRenderTargets(1u, nullptr, renderTarget->GetDSV().Get());
		else
			graphics.GetContext()->OMSetRenderTargets(1u, renderTarget->GetRTV().GetAddressOf(), nullptr);
		SetViewPort(renderTarget->GetWidth(), renderTarget->GetHeight(), 0, 0);
	}

	void RendererCommand::ClearRenderTarget(Ref<RenderTarget> renderTarget, glm::vec4 color)
	{
		RendererAPI& graphics = RendererAPI::Get();

		if (renderTarget->IsDepthStencilTexture())
			graphics.GetContext()->ClearDepthStencilView(renderTarget->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, color.r, (uint8_t)color.g);
		else
			graphics.GetContext()->ClearRenderTargetView(renderTarget->GetRTV().Get(), (float*)&color);
	}

	void RendererCommand::SetFrameBuffer(Ref<FrameBuffer> frameBuffer)
	{
		RendererAPI& graphics = RendererAPI::Get();

		ID3D11DepthStencilView* dsv = nullptr;
		std::vector<ID3D11RenderTargetView*> rtvs;
		rtvs.reserve(frameBuffer->GetRenderTargets().size());

		if (frameBuffer->HasDepthBuffer())
			dsv = frameBuffer->GetDepthBuffer()->GetDSV().Get();

		for (Ref<RenderTarget> renderTarget : frameBuffer->GetRenderTargets())
			rtvs.push_back(renderTarget->GetRTV().Get());

		graphics.GetContext()->OMSetRenderTargets((uint32_t)rtvs.size(), rtvs.data(), dsv);
	}

	void RendererCommand::SetViewPort(int width, int height, int x, int y)
	{
		RendererAPI& graphics = RendererAPI::Get();
		D3D11_VIEWPORT vp = { 0 };
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = (float)x;
		vp.TopLeftY = (float)y;
		graphics.GetContext()->RSSetViewports(1u, &vp);
	}

	void RendererCommand::SetVertexBuffer(Ref<VertexBuffer> vb)
	{
		RendererAPI& graphics = RendererAPI::Get();

		const UINT stride = vb->GetStride();
		const UINT offset = 0u;
		graphics.GetContext()->IASetVertexBuffers(0u, 1u, vb->GetBuffer().GetAddressOf(), &stride, &offset);
	}

	void RendererCommand::SetIndexBuffer(Ref<IndexBuffer> ib)
	{
		RendererAPI& graphics = RendererAPI::Get();
		graphics.GetContext()->IASetIndexBuffer(ib->GetBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	void RendererCommand::SetMesh(Ref<Mesh> mesh)
	{
		SetVertexBuffer(mesh->GetVertexBuffer());
		SetIndexBuffer(mesh->GetIndexBuffer());
	}

	void RendererCommand::SetShader(Ref<Shader> shader)
	{
		RendererAPI& graphics = RendererAPI::Get();
		graphics.GetContext()->IASetInputLayout(shader->GetInputLayout().Get());
		graphics.GetContext()->VSSetShader(shader->GetVertexShader().Get(), nullptr, 0u);
		graphics.GetContext()->PSSetShader(shader->GetPixelShader().Get(), nullptr, 0u);
		graphics.GetContext()->RSSetState(shader->GetRasterizerState().Get());
		graphics.GetContext()->OMSetDepthStencilState(shader->GetDepthStencilState().Get(), 0);

		for (Shader::Sampler& sampler : shader->GetSamplers())
		{
			switch (sampler.info.type)
			{
			case Shader::Vertex:
				graphics.GetContext()->VSSetSamplers(sampler.info.pixelSlot, 1, sampler.sampler.GetAddressOf());
				break;
			case Shader::Pixel:
				graphics.GetContext()->PSSetSamplers(sampler.info.pixelSlot, 1, sampler.sampler.GetAddressOf());
				break;
			}
		}
	}

	void RendererCommand::SetConstantBuffer(Shader::BindPointInfo bp, Ref<ConstantBuffer> cb)
	{
		RendererAPI& graphics = RendererAPI::Get();

		if ((uint32_t)bp.type & (uint32_t)Shader::ShaderType::Vertex)
			graphics.GetContext()->VSSetConstantBuffers(bp.vertexSlot, 1u, cb->GetBuffer().GetAddressOf());
		if ((uint32_t)bp.type & (uint32_t)Shader::ShaderType::Pixel)
			graphics.GetContext()->PSSetConstantBuffers(bp.pixelSlot, 1u, cb->GetBuffer().GetAddressOf());
	}

	void RendererCommand::SetStructruedBuffer(Shader::BindPointInfo bp, Ref<StructuredBuffer> sb)
	{
		RendererAPI& graphics = RendererAPI::Get();

		if ((uint32_t)bp.type & (uint32_t)Shader::ShaderType::Vertex)
			graphics.GetContext()->VSSetShaderResources(bp.vertexSlot, 1u, sb->GetSRV().GetAddressOf());
		if ((uint32_t)bp.type & (uint32_t)Shader::ShaderType::Pixel)
			graphics.GetContext()->VSSetShaderResources(bp.pixelSlot, 1u, sb->GetSRV().GetAddressOf());
	}

	void RendererCommand::SetTexture(Shader::BindPointInfo bp, Ref<Texture2D> texture)
	{
		RendererAPI& graphics = RendererAPI::Get();
		if ((uint32_t)bp.type & (uint32_t)Shader::ShaderType::Vertex)
		{
			graphics.GetContext()->VSSetShaderResources(bp.vertexSlot, 1u, texture->GetSRV().GetAddressOf());
		}
		if ((uint32_t)bp.type & (uint32_t)Shader::ShaderType::Pixel)
		{
			graphics.GetContext()->PSSetShaderResources(bp.pixelSlot, 1u, texture->GetSRV().GetAddressOf());
		}
	}

	void RendererCommand::BlitToSwapChain(SwapChain& swapChain, Ref<RenderTarget> renderTarget)
	{
		RendererAPI& graphics = RendererAPI::Get();
		SetSwapChain(swapChain);
		SetShader(s_BlitShader);
		SetTexture(s_BlitShader->GetBindPoint("tex"), renderTarget);
		DrawMesh(s_ScreenMesh);
	}

	void RendererCommand::DrawIndexed(uint32_t count)
	{
		RendererAPI& graphics = RendererAPI::Get();
		graphics.GetContext()->DrawIndexed(count, 0u, 0u);
	}

	void RendererCommand::DrawMesh(Ref<Mesh> mesh)
	{
		SetMesh(mesh);
		DrawIndexed(mesh->GetIndexBuffer()->GetCount());
	}

}