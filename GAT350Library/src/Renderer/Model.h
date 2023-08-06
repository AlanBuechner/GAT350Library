#pragma once
#include "Core/Core.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include <vector>
#include <unordered_map>

struct aiScene;
struct aiNode;

namespace Engine
{
	class Model
	{
	public:
		struct Node
		{
			Ref<Mesh> m_Mesh;
			Ref<Material> m_Material;
		};

		Model(const fs::path& path);

		size_t GetNumberOfNodes() const { return m_Nodes.size(); }
		Node& GetNode(uint32_t i) { return m_Nodes[i]; }

		const std::vector<Node> GetNodes() { return m_Nodes; }

		void LoadFromFile(const fs::path& path);

		static Ref<Model> Create(const fs::path& path);

	private:

		void LoadNodeData(const aiScene* model, aiNode* node, std::vector<MeshBuilder>& meshBuilders, std::vector<Ref<Material>>& materials, glm::mat4 transform);

	private:
		std::vector<Node> m_Nodes;
	};
}