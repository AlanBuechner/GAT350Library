#pragma once
#include "Core/Core.h"
#include "Shader.h"
#include "Mesh.h"

namespace Engine
{
	class SwapChain;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class StructuredBuffer;
	class Texture2D;
}

namespace Engine
{
	class RendererCommand
	{

	public:
		static void Init();

		static void SetSwapChain(SwapChain& swapChain);
		static void SetViewPort(int width, int height, int x = 0, int y = 0);
		static void SetVertexBuffer(Ref<VertexBuffer> vb);
		static void SetIndexBuffer(Ref<IndexBuffer> ib);
		static void SetMesh(Ref<Mesh::SubMesh> mesh);
		static void SetShader(Ref<Shader> shader);
		static void SetConstantBuffer(Shader::BindPointInfo bp, Ref<ConstantBuffer> cb);
		static void SetStructruedBuffer(Shader::BindPointInfo bp, Ref<StructuredBuffer> sb);
		static void SetTexture(Shader::BindPointInfo bp, Ref<Texture2D> texture);

		static void ClearToColor(glm::vec4 color);

		static void DrawIndexed(uint32_t count);
		static void DrawMesh(Ref<Mesh::SubMesh> mesh);

	};
}