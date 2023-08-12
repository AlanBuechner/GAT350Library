#pragma once
#include "Core/Core.h"
#include "Shader.h"

namespace Engine
{
	class SwapChain;
	class VertexBuffer;
	class IndexBuffer;
	class Mesh;
	class ConstantBuffer;
	class StructuredBuffer;
	class Texture2D;
	class RenderTarget;
	class FrameBuffer;
}

namespace Engine
{
	class RendererCommand
	{

	public:
		static void Init();

		static void SetSwapChain(SwapChain& swapChain);
		static void ClearSwapChain(SwapChain& swapChain, glm::vec3 color);
		static void SetRenderTerget(Ref<RenderTarget> renderTarget);
		static void ClearRenderTarget(Ref<RenderTarget> renderTarget, glm::vec4 color);
		static void SetFrameBuffer(Ref<FrameBuffer> frameBuffer);
		static void SetViewPort(int width, int height, int x = 0, int y = 0);
		static void SetVertexBuffer(Ref<VertexBuffer> vb);
		static void SetIndexBuffer(Ref<IndexBuffer> ib);
		static void SetMesh(Ref<Mesh> mesh);
		static void SetShader(Ref<Shader> shader);
		static void SetConstantBuffer(Shader::BindPointInfo bp, Ref<ConstantBuffer> cb);
		static void SetStructruedBuffer(Shader::BindPointInfo bp, Ref<StructuredBuffer> sb);
		static void SetTexture(Shader::BindPointInfo bp, Ref<Texture2D> texture);

		static void BlitToSwapChain(SwapChain& swapChain, Ref<RenderTarget> renderTarget);

		static void DrawIndexed(uint32_t count);
		static void DrawMesh(Ref<Mesh> mesh);

	private:
		static Ref<Shader> s_BlitShader;
		static Ref<Mesh> s_ScreenMesh;

	};
}