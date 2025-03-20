#pragma once
#include "pubh.h"
#include "ZMesh.h"

namespace ZXEngine
{
	class StaticMesh : public Mesh
	{
    public:
        StaticMesh(vector<Vertex>&& vertices, vector<uint32_t>&& indices, bool setup = true, bool skinned = false);
        StaticMesh(const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool setup = true, bool skinned = false);

        virtual void SetUp() override;

    private:
        bool mSkinned = false;
        bool mIsSetUp = false;
    };
}