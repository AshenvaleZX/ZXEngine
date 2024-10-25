#pragma once
#include "pubh.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	// һ��Mesh֧�ֵ�����������
	constexpr uint32_t MAX_BONE_NUM = 64;

	class Mesh
	{
	public:
		uint32_t VAO;

		vector<Vertex> mVertices;
		vector<uint32_t> mIndices;

		// �Ӱ�����Mesh���ݵĽڵ㵽ģ�͸��ڵ�ı任����(����ģ�Ͱ������νṹʱ��Ч)
		Matrix4 mRootTrans;
		// ����ƫ��������
		vector<Matrix4> mBonesOffset;
		// �������ձ任��������(�����򣬻�ֱ�Ӵ����Դ湩Shaderʹ��)
		vector<Matrix4> mBonesFinalTransform;
		// �������ֵ�����������ӳ��
		unordered_map<string, uint32_t> mBoneNameToIndexMap;

		// ��xyz��������Զ�ĵ㣬0-5�ֱ��Ӧ+x, -x, +y, -y, +z, -z
		array<Vertex, 6> mExtremeVertices;
		// ģ����X���ϵĳ���
		float mAABBSizeX = 0.0f;
		// ģ����Y���ϵĳ���
		float mAABBSizeY = 0.0f;
		// ģ����Z���ϵĳ���
		float mAABBSizeZ = 0.0f;

		~Mesh();

		virtual void SetUp() {};
	};
}