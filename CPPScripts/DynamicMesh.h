#pragma once
#include "pubh.h"
#include "ZMesh.h"

namespace ZXEngine
{
    class DynamicMesh : public Mesh
    {
    public:
        DynamicMesh(unsigned int vertexSize, unsigned int indexSize);

        void UpdateData(vector<Vertex> vertices, vector<unsigned int> indices);

    private:
        unsigned int vertexSize;
        unsigned int indexSize;
    };
}