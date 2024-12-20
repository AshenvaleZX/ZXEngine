#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct Vertex;
	class StaticMesh;
	class DynamicMesh;
	class GeometryGenerator
	{
	public:
		static StaticMesh* CreateBox(float xLength, float yLength, float zLength);
		static StaticMesh* CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount);
		static StaticMesh* CreateSphereTessellation(float radius, uint32_t subdivisionNum);
		static StaticMesh* CreateCircle(float radius, uint32_t sliceCount);
		static StaticMesh* CreateCone(float radius, float height, uint32_t sliceCount);
		static StaticMesh* CreateCylinder(float topRadius, float bottomRadius, float height, uint32_t sliceCount, uint32_t stackCount);
		static StaticMesh* CreatePlane(float xLength, float zLength, uint32_t xSplit, uint32_t zSplit);
		static StaticMesh* CreateQuad(float xLength, float yLength);
		// ������Ļ�ľ���(λ��NDC)
		static StaticMesh* CreateScreenQuad();
		// �泯�ڲ��ĵ�λ������
		static StaticMesh* CreateSimpleInwardBox();

		static DynamicMesh* CreateDynamicPlane(float xLength, float zLength, uint32_t xSplit, uint32_t zSplit);

	private:
		static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
		static void Subdivide(vector<Vertex>& vertices, vector<uint32_t>& indices);
		static void CreatePlaneVertices(float xLength, float zLength, uint32_t xSplit, uint32_t zSplit, vector<Vertex>& vertices, vector<uint32_t>& indices);
	};
}