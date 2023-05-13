#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct Vertex;
	class StaticMesh;
	class GeometryGenerator
	{
	public:
		static StaticMesh* CreateBox(float xLength, float yLength, float zLength);
		static StaticMesh* CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount);
		static StaticMesh* CreateSphereTessellation(float radius, uint32_t subdivisionNum);

	private:
		static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
		static void Subdivide(vector<Vertex>& vertices, vector<uint32_t>& indices);
	};
}