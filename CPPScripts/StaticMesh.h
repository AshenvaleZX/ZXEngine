#pragma once
#include "pubh.h"
#include "ZMesh.h"

namespace ZXEngine
{
	class StaticMesh : public Mesh
	{
    public:
        StaticMesh(vector<Vertex> vertices, vector<unsigned int> indices);
        
        virtual void Use();
    };
}