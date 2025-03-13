#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh::StaticMesh(vector<Vertex>&& vertices, vector<uint32_t>&& indices, bool setup, bool skinned)
	{
		mVertices = std::move(vertices);
		mIndices = std::move(indices);
		mSkinned = skinned;

		if (setup)
		{
			RenderAPI::GetInstance()->SetUpStaticMesh(VAO, mVertices, mIndices, skinned);
			mIsSetUp = true;
		}
	}

	StaticMesh::StaticMesh(const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool setup, bool skinned)
	{
		mVertices = vertices;
		mIndices = indices;
		mSkinned = skinned;

		if (setup)
		{
			RenderAPI::GetInstance()->SetUpStaticMesh(VAO, vertices, indices, skinned);
			mIsSetUp = true;
		}
	}

	void StaticMesh::SetUp()
	{
		if (mIsSetUp)
			return;

		RenderAPI::GetInstance()->SetUpStaticMesh(VAO, mVertices, mIndices, mSkinned);
		mIsSetUp = true;
	}
}