#include "Mesh.h"

namespace Engine
{

	Mesh::Mesh(Vertex* vertices, uint32_t vertCount, uint32_t* indeces, uint32_t indexCount)
	{
		SetData(vertices, vertCount, indeces, indexCount);
	}

	void Mesh::UpdateVertexBuffer(Vertex* vertices, uint32_t count)
	{
		vb->SetData(vertices, count * sizeof(Vertex));
	}

	void Mesh::UpdateIndexBuffer(uint32_t* indeces, uint32_t count)
	{
		ib->SetData(indeces, count);
	}

	void Mesh::SetData(Vertex* vertices, uint32_t vertCount, uint32_t* indeces, uint32_t indexCount)
	{
		vb = VertexBuffer::Create(vertices, sizeof(Vertex), vertCount);
		ib = IndexBuffer::Create(indeces, indexCount);
	}

	Ref<Mesh> Mesh::Create(Vertex* vertices, uint32_t vertCount, uint32_t* indeces, uint32_t indexCount)
	{
		return std::make_shared<Mesh>(vertices, vertCount, indeces, indexCount);
	}
}