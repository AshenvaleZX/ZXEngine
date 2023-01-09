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
		string modelName = "";
		Material* matetrial = nullptr;
		size_t verticesNum = 0;
		size_t trianglesNum = 0;
		vector<Mesh*> meshes;
		// 在xyz方向上最远的点，0-5分别对应+x, -x, +y, -y, +z, -z
		Vertex extremeVertices[6];
		// 模型在X轴上的长度
		float boundsSizeX = 0.0f;
		// 模型在Y轴上的长度
		float boundsSizeY = 0.0f;
		// 模型在Z轴上的长度
		float boundsSizeZ = 0.0f;

		MeshRenderer();
		~MeshRenderer();

		virtual ComponentType GetInsType();

		void Draw();
		// 使用ASSIMP加载模型文件
		void LoadModel(string const& path);

	private:
		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
		void CheckExtremeVertex(const Vertex& vertex);
	};
}