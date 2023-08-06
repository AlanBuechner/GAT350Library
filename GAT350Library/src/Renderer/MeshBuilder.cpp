#include "MeshBuilder.h"

namespace Engine
{

	MeshBuilder::MeshBuilder(const std::vector<Mesh::Vertex>& verts, const std::vector<uint32_t>& indices) :
		m_Vertices(verts), m_Indices(indices)
	{}

	Ref<Mesh> MeshBuilder::Build()
	{
		return Mesh::Create(m_Vertices.data(), (uint32_t)m_Vertices.size(), m_Indices.data(), (uint32_t)m_Indices.size());
	}

	Ref<Mesh> MeshBuilder::BuildWithTransform(glm::mat4 transform)
	{
		std::vector<Mesh::Vertex> verts = m_Vertices;
		for (uint32_t i = 0; i < verts.size(); i++)
		{
			verts[i].Position = transform * verts[i].Position;
			verts[i].Normal = (glm::mat3)transform * verts[i].Normal;
		}

		return Mesh::Create(verts.data(), (uint32_t)verts.size(), m_Indices.data(), (uint32_t)m_Indices.size());
	}

	Ref<MeshBuilder> MeshBuilder::Create()
	{
		return std::make_shared<MeshBuilder>();
	}

	Ref<MeshBuilder> MeshBuilder::Create(const std::vector<Mesh::Vertex>& verts, const std::vector<uint32_t>& indices)
	{
		return std::make_shared<MeshBuilder>(verts, indices);
	}

}

