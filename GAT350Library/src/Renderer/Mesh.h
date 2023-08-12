#pragma once
#include "Core/Core.h"
#include "Buffer.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <vector>


namespace Engine
{

	class Mesh
	{
	public:
		struct Vertex {
			glm::vec4 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec2 UV;
		};

		Mesh(Vertex* vertices, uint32_t vertCount, uint32_t* indeces, uint32_t indexCount);

		void UpdateVertexBuffer(Vertex* vertices, uint32_t count);
		void UpdateIndexBuffer(uint32_t* indeces, uint32_t count);

		void SetData(Vertex* vertices, uint32_t vertCount, uint32_t* indeces, uint32_t indexCount);

		Ref<VertexBuffer> GetVertexBuffer() { return vb; };
		Ref<IndexBuffer> GetIndexBuffer() { return ib; };

		static Ref<Mesh> Create(Vertex* vertices, uint32_t vertCount, uint32_t* indeces, uint32_t indexCount);


	private:
		Ref<VertexBuffer> vb;
		Ref<IndexBuffer> ib;
		
		static const std::string& s_TexturesFolder;
	};
}