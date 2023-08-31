#pragma once
#include "pubh.h"
#include "ZMesh.h"

namespace ZXEngine
{
    class DynamicMesh : public Mesh
    {
    public:
        DynamicMesh(uint32_t vertexSize, uint32_t indexSize);

        void UpdateData(const vector<Vertex>& vertices, const vector<uint32_t>& indices);

    private:
        uint32_t vertexSize;
        uint32_t indexSize;
    };
}