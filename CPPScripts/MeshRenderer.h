#pragma once
#include "Component.h"
#include "ZMesh.h"
#include "Material.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace ZXEngine
{
	class MeshRenderer : public Component
	{
	public:
		static ComponentType GetType();

	public:
		bool castShadow = false;
		bool receiveShadow = false;
		Material* matetrial = nullptr;
		vector<Mesh*> meshes;

		MeshRenderer() {};
		~MeshRenderer() {};

		virtual ComponentType GetInsType();

		// 使用ASSIMP加载模型文件
		void LoadModel(string const& path);

	private:
		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}