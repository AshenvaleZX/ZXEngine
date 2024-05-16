#pragma once
#include "pubh.h"
#include "PublicStruct.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace ZXEngine
{
	struct Vertex;
	class Mesh;
	class StaticMesh;
	class MeshRenderer;
	class AnimationController;
	class ModelUtil
	{
	public:
		// 使用ASSIMP加载模型文件
		static ModelData* LoadModel(const string& path, bool loadFullAnim = true, bool async = false);
		// 算法生成几何体模型
		static shared_ptr<Mesh> GenerateGeometry(GeometryType type);

		static string GetGeometryTypeName(GeometryType type);

	private:
		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		static void ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData* pModelData, bool async = false);
		static void ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData* pModelData, BoneNode* pBoneNode, bool async = false);
		static shared_ptr<StaticMesh> ProcessMesh(const aiMesh* mesh, bool async = false);
		static AnimationController* ProcessAnimation(const aiScene* pScene);
		static void CountNode(const aiNode* pNode, uint32_t& count);
		static void LoadAnimBriefInfos(const aiScene* pScene, ModelData* pModelData);

		static inline Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& mat);
		static inline Vector3 aiVector3DToVector3(const aiVector3D& vec);
		static inline Quaternion aiQuaternionToQuaternion(const aiQuaternion& quat);

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

	Vector3 ModelUtil::aiVector3DToVector3(const aiVector3D& vec)
	{
		return Vector3(vec.x, vec.y, vec.z);
	}

	Quaternion ModelUtil::aiQuaternionToQuaternion(const aiQuaternion& quat)
	{
		return Quaternion(quat.x, quat.y, quat.z, quat.w);
	}
}