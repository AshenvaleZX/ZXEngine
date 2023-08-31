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
		// 使用ASSIMP加载模型文件
		static vector<Mesh*> LoadModel(const string& path);
		// 算法生成几何体模型
		static Mesh* GenerateGeometry(GeometryType type);

		static string GetGeometryTypeName(GeometryType type);

	private:
		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		static void ProcessNode(const aiNode* node, const aiScene* scene, vector<Mesh*>& meshes);
		static StaticMesh* ProcessMesh(const aiMesh* mesh);
		static inline Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& mat);

		static void CheckExtremeVertex(const Vertex& vertex, array<Vertex, 6>& extremeVertices);
	};

	Matrix4 ModelUtil::aiMatrix4x4ToMatrix4(const aiMatrix4x4& mat)
	{
		return Matrix4(
			mat.a1, mat.a2, mat.a3, mat.a4,
			mat.b1, mat.b2, mat.b3, mat.b4,
			mat.c1, mat.c2, mat.c3, mat.c4,
			mat.d1, mat.d2, mat.d3, mat.d4
		);
	}
}