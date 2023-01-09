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
		// ��xyz��������Զ�ĵ㣬0-5�ֱ��Ӧ+x, -x, +y, -y, +z, -z
		Vertex extremeVertices[6];
		// ģ����X���ϵĳ���
		float boundsSizeX = 0.0f;
		// ģ����Y���ϵĳ���
		float boundsSizeY = 0.0f;
		// ģ����Z���ϵĳ���
		float boundsSizeZ = 0.0f;

		MeshRenderer();
		~MeshRenderer();

		virtual ComponentType GetInsType();

		void Draw();
		// ʹ��ASSIMP����ģ���ļ�
		void LoadModel(string const& path);

	private:
		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
		void CheckExtremeVertex(const Vertex& vertex);
	};
}