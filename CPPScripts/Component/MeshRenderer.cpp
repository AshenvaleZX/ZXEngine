#include "MeshRenderer.h"
#include "../ZMesh.h"
#include "../RenderAPI.h"
#include "../Material.h"
#include "../ModelUtil.h"
#include "../Animation/AnimationController.h"

namespace ZXEngine
{
    ComponentType MeshRenderer::GetType()
    {
        return ComponentType::MeshRenderer;
    }

    MeshRenderer::MeshRenderer()
    {
        
    }

    MeshRenderer::~MeshRenderer()
    {
        delete mMatetrial;
        delete mShadowCastMaterial;

        if (mRootBone)
            delete mRootBone;

        if (mAnimationController)
            delete mAnimationController;

        for (auto mesh : mMeshes)
            delete mesh;
    }

    ComponentType MeshRenderer::GetInsType()
    {
        return ComponentType::MeshRenderer;
    }

    void MeshRenderer::Draw()
    {
        for (auto mesh : mMeshes)
        {
            RenderAPI::GetInstance()->Draw(mesh->VAO);
        }
    }

    void MeshRenderer::LoadModel(GeometryType type)
    {
        mModelName = ModelUtil::GetGeometryTypeName(type);
        mMeshes.push_back(ModelUtil::GenerateGeometry(type));
        UpdateInternalData();
    }

    void MeshRenderer::LoadModel(const string& path)
    {
        ModelUtil::LoadModel(path, this);
        UpdateInternalData();
    }

    void MeshRenderer::UpdateInternalData()
    {
        for (auto mesh : mMeshes)
		{
            mVerticesNum += mesh->mVertices.size();
            mTrianglesNum += mesh->mIndices.size() / 3;

            if (mesh->mExtremeVertices[0].Position.x > mExtremeVertices[0].Position.x)
                mExtremeVertices[0] = mesh->mExtremeVertices[0];
            if (mesh->mExtremeVertices[1].Position.x < mExtremeVertices[1].Position.x)
                mExtremeVertices[1] = mesh->mExtremeVertices[1];
            if (mesh->mExtremeVertices[2].Position.y > mExtremeVertices[2].Position.y)
                mExtremeVertices[2] = mesh->mExtremeVertices[2];
            if (mesh->mExtremeVertices[3].Position.y < mExtremeVertices[3].Position.y)
                mExtremeVertices[3] = mesh->mExtremeVertices[3];
            if (mesh->mExtremeVertices[4].Position.z > mExtremeVertices[4].Position.z)
                mExtremeVertices[4] = mesh->mExtremeVertices[4];
            if (mesh->mExtremeVertices[5].Position.z < mExtremeVertices[5].Position.z)
                mExtremeVertices[5] = mesh->mExtremeVertices[5];
		}

        mAABBSizeX = mExtremeVertices[0].Position.x - mExtremeVertices[1].Position.x;
        mAABBSizeY = mExtremeVertices[2].Position.y - mExtremeVertices[3].Position.y;
        mAABBSizeZ = mExtremeVertices[4].Position.z - mExtremeVertices[5].Position.z;
    }
}