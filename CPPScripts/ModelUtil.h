#pragma once
#include "pubh.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace ZXEngine
{
	struct Vertex;
	class Mesh;
	class StaticMesh;
	class ModelUtil
	{
	public:
		// ʹ��ASSIMP����ģ���ļ�
		static vector<Mesh*> LoadModel(string const& path);
		// �㷨���ɼ�����ģ��
		static Mesh* GenerateGeometry(GeometryType type);

		static string GetGeometryTypeName(GeometryType type);

	private:
		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		static void ProcessNode(const aiNode* node, const aiScene* scene, vector<Mesh*>& meshes);
		static StaticMesh* ProcessMesh(const aiMesh* mesh, const aiScene* scene);
		static void CheckExtremeVertex(const Vertex& vertex, array<Vertex, 6>& extremeVertices);
	};
}