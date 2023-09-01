#pragma once
#include "Component.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	class Mesh;
	class Material;
	class AnimationController;
	class MeshRenderer : public Component
	{
	public:
		static ComponentType GetType();

	public:
		bool mCastShadow = false;
		bool mReceiveShadow = false;

		string mModelName = "";

		Material* mMatetrial = nullptr;
		Material* mShadowCastMaterial = nullptr;

		size_t mVerticesNum = 0;
		size_t mTrianglesNum = 0;
		vector<Mesh*> mMeshes;

		// 模型骨骼根节点
		BoneNode* mRootBone = nullptr;

		// 动画控制器
		AnimationController* mAnimationController = nullptr;

		// 在xyz方向上最远的点，0-5分别对应+x, -x, +y, -y, +z, -z
		array<Vertex, 6> mExtremeVertices;
		// 模型在X轴上的长度
		float mAABBSizeX = 0.0f;
		// 模型在Y轴上的长度
		float mAABBSizeY = 0.0f;
		// 模型在Z轴上的长度
		float mAABBSizeZ = 0.0f;

		MeshRenderer();
		~MeshRenderer();

		virtual ComponentType GetInsType();

		void Draw();
		void LoadModel(GeometryType type);
		void LoadModel(const string& path);

	private:
		void UpdateInternalData();
	};
}