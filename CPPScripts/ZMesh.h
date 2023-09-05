#pragma once
#include "pubh.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	class Mesh
	{
	public:
		uint32_t VAO;

		vector<Vertex> mVertices;
		vector<uint32_t> mIndices;

		// 骨骼数据数组
		vector<BoneInfo> mBones;
		// 骨骼名字到数组索引的映射
		unordered_map<string, uint32_t> mBoneNameToIndexMap;
		// 骨骼根节点到世界坐标系的变换矩阵
		Matrix4 mRootBoneToWorld;

		// 在xyz方向上最远的点，0-5分别对应+x, -x, +y, -y, +z, -z
		array<Vertex, 6> mExtremeVertices;
		// 模型在X轴上的长度
		float mAABBSizeX = 0.0f;
		// 模型在Y轴上的长度
		float mAABBSizeY = 0.0f;
		// 模型在Z轴上的长度
		float mAABBSizeZ = 0.0f;

		~Mesh();
	};
}