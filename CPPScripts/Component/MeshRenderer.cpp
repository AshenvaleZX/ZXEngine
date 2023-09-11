#include "MeshRenderer.h"
#include "Animator.h"
#include "../ZMesh.h"
#include "../RenderAPI.h"
#include "../Material.h"
#include "../ModelUtil.h"

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

    void MeshRenderer::GenerateModel(GeometryType type)
    {
        mModelName = ModelUtil::GetGeometryTypeName(type);
        mMeshes.push_back(ModelUtil::GenerateGeometry(type));
        UpdateInternalData();
    }

    void MeshRenderer::SetMeshes(const vector<Mesh*>& meshes)
    {
		mMeshes = meshes;
		UpdateInternalData();
    }

    void MeshRenderer::UpdateAnimation()
    {
        if (mAnimator)
            mAnimator->Update(mMeshes);
    }

    void MeshRenderer::UpdateBoneTransformsForRender()
    {
        if (mAnimator)
        {
            for (auto mesh : mMeshes)
                mMatetrial->SetMatrix("_BoneMatrices", mesh->mBonesFinalTransform.data(), static_cast<uint32_t>(mesh->mBonesFinalTransform.size()));
        }
    }

    void MeshRenderer::UpdateBoneTransformsForShadow()
    {
        if (mAnimator)
        {
            for (auto mesh : mMeshes)
                mShadowCastMaterial->SetMatrix("_BoneMatrices", mesh->mBonesFinalTransform.data(), static_cast<uint32_t>(mesh->mBonesFinalTransform.size()));
        }
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