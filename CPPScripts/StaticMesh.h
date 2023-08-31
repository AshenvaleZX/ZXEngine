#pragma once
#include "pubh.h"
#include "ZMesh.h"

namespace ZXEngine
{
	class StaticMesh : public Mesh
	{
    public:
        StaticMesh(const vector<Vertex>& vertices, const vector<uint32_t>& indices);
    };
}