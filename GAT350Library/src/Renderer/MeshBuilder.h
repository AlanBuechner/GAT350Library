#pragma once
#include "Core/Core.h"
#include "Mesh.h"

namespace Engine
{
	class MeshBuilder
	{
	public:
		MeshBuilder() = default;
		MeshBuilder(const std::vector<Mesh::Vertex>& verts, const std::vector<uint32_t>& indices);

		Ref<Mesh> Build();
		Ref<Mesh> BuildWithTransform(glm::mat4 transform);

		static Ref<MeshBuilder> Create();
		static Ref<MeshBuilder> Create(const std::vector<Mesh::Vertex>& verts, const std::vector<uint32_t>& indices);

	public:
		std::vector<Mesh::Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}