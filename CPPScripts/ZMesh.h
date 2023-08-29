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

		// ��xyz��������Զ�ĵ㣬0-5�ֱ��Ӧ+x, -x, +y, -y, +z, -z
		array<Vertex, 6> mExtremeVertices;
		// ģ����X���ϵĳ���
		float mAABBSizeX = 0.0f;
		// ģ����Y���ϵĳ���
		float mAABBSizeY = 0.0f;
		// ģ����Z���ϵĳ���
		float mAABBSizeZ = 0.0f;

		~Mesh();
	};
}