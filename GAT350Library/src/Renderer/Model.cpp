#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/scene.h>

namespace Engine
{

	glm::vec2 GetUVCoords(aiMesh* mesh, uint32_t index)
	{
		if (mesh->mTextureCoords[0] == nullptr)
			return { 0.0f, 0.0f };
		const auto coords = mesh->mTextureCoords[0][index];
		return { coords.x, coords.y };
	}

	glm::vec4 GetColor(aiMesh* mesh, uint32_t index)
	{
		if (mesh->mColors[0] == nullptr)
			return { 1.0f, 1.0f, 1.0f, 1.0f };
		const auto color = mesh->mColors[0][index];
		return { color.r, color.g, color.b, color.a };
	}

	Model::Model(const fs::path& path)
	{
		LoadFromFile(path);
	}

	void Model::LoadFromFile(const fs::path& path)
	{
		m_Nodes.clear();
		fs::path folder = path.parent_path();

		// load mesh data from the file
		Assimp::Importer imp;
		auto model = imp.ReadFile(path.string(), 0
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_GenNormals 
			| aiProcess_CalcTangentSpace
//			| aiProcess_FlipWindingOrder 
			| aiProcess_FlipUVs
		);

		if (model == nullptr)
		{
			DBOUT(L"faild to load model \"" + path.wstring() + L"\"\n");
			return;
		}

		// Generate all materials
		std::vector<Ref<Material>> materials;
		materials.reserve(model->mNumMaterials);
		for (uint32_t i = 0; i < model->mNumMaterials; i++)
		{
			Ref<Material> mat = std::make_shared<Material>();
			auto& material = *model->mMaterials[i];

			aiString fileName;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &fileName) == aiReturn_SUCCESS)
				mat->m_Diffuse = Texture2D::Create(folder / fileName.C_Str());

			if (material.GetTexture(aiTextureType_NORMALS, 0, &fileName) == aiReturn_SUCCESS)
				mat->m_Normal = Texture2D::Create(folder / fileName.C_Str());

			if (material.GetTexture(aiTextureType_METALNESS, 0, &fileName) == aiReturn_SUCCESS)
				mat->m_Metal = Texture2D::Create(folder / fileName.C_Str());

			if (material.GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &fileName) == aiReturn_SUCCESS)
				mat->m_Roughness = Texture2D::Create(folder / fileName.C_Str());

			if (material.GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &fileName) == aiReturn_SUCCESS)
				mat->m_AO = Texture2D::Create(folder / fileName.C_Str());

			materials.push_back(mat);
		}

		// Generate all the meshes
		std::vector<MeshBuilder> meshBuilders;
		meshBuilders.reserve(model->mNumMeshes);
		for (uint32_t i = 0; i < model->mNumMeshes; i++)
		{
			std::vector<Mesh::Vertex> vertices;
			std::vector<uint32_t> indices;

			const auto mesh = model->mMeshes[i]; // get the mesh

			// resize the vertices array and load the data
			vertices.reserve(mesh->mNumVertices);
			for (uint32_t j = 0; j < mesh->mNumVertices; j++)
			{
				vertices.push_back({
					{ mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f },
					{ mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z },
					{ mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z },
					GetUVCoords(mesh, j)
				});
			}

			// resize the index array and load the data
			indices.reserve(mesh->mNumFaces * 3);
			for (uint32_t j = 0; j < mesh->mNumFaces; j++)
			{
				const auto& face = mesh->mFaces[j];
				for (uint32_t k = 0; k < 3; k++)
					indices.push_back(face.mIndices[k]);
			}

			meshBuilders.push_back(
				MeshBuilder(vertices, indices)
			);
		}

		// Generate Node tree
		LoadNodeData(model, model->mRootNode, meshBuilders, materials, glm::identity<glm::mat4>());
	}


	void Model::LoadNodeData(const aiScene* model, aiNode* node, std::vector<MeshBuilder>& meshBuilders, std::vector<Ref<Material>>& materials, glm::mat4 transform)
	{

		transform = *reinterpret_cast<glm::mat4*>(&node->mTransformation) * transform;

		// load meshes
		m_Nodes.reserve(node->mNumMeshes);
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t meshIndex = node->mMeshes[i];
			m_Nodes.push_back({
					meshBuilders[meshIndex].BuildWithTransform(transform),
					materials[model->mMeshes[meshIndex]->mMaterialIndex]
				});
		}

		// load child nodes
		for (uint32_t i = 0; i < node->mNumChildren; i++)
			LoadNodeData(model, node->mChildren[i], meshBuilders, materials, transform);

	}

	Ref<Model> Model::Create(const fs::path& path)
	{
		return std::make_shared<Model>(path);
	}
}

