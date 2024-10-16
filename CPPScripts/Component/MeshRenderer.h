#pragma once
#include "Component.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	class Mesh;
	class Material;
	class Animator;
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
		Material* mGBufferMaterial = nullptr;

		Animator* mAnimator = nullptr;

		size_t mVerticesNum = 0;
		size_t mTrianglesNum = 0;
		vector<shared_ptr<Mesh>> mMeshes;

		// ��xyz��������Զ�ĵ㣬0-5�ֱ��Ӧ+x, -x, +y, -y, +z, -z
		array<Vertex, 6> mExtremeVertices;
		// ģ����X���ϵĳ���
		float mAABBSizeX = 0.0f;
		// ģ����Y���ϵĳ���
		float mAABBSizeY = 0.0f;
		// ģ����Z���ϵĳ���
		float mAABBSizeZ = 0.0f;

		MeshRenderer();
		~MeshRenderer();

		virtual ComponentType GetInsType();

		void Draw();
		void DrawShadow();
		void GenerateModel(GeometryType type);
		void SetMeshes(const vector<shared_ptr<Mesh>>& meshes);

	private:
		void UpdateInternalData();
	};
}