#include "ZMesh.h"
#include "PhysZ/PhysZ.h"

namespace ZXEngine
{
	Mesh::~Mesh()
	{
		RenderAPI::GetInstance()->DeleteMesh(VAO);
	}

	bool Mesh::Intersect(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit) const
	{
		return PhysZ::IntersectionDetector::Detect(ray, *this, hit);
	}

	bool Mesh::IntersectAABB(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit) const
	{
		PhysZ::Ray correctionRay = PhysZ::Ray(ray.mOrigin - mAABBCenter, ray.mDirection);
		return PhysZ::IntersectionDetector::Detect(correctionRay, mAABBSize / 2.0f, hit);
	}
}