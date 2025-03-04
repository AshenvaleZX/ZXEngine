#pragma once
#include "pubh.h"
#include "ZMesh.h"

namespace ZXEngine
{
	class StaticMesh : public Mesh
	{
    public:
        StaticMesh(vector<Vertex>&& vertices, vector<uint32_t>&& indices, bool setup = true);
        StaticMesh(const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool setup = true);

        virtual void SetUp() override;

    private:
        bool mIsSetUp = false;
    };
}