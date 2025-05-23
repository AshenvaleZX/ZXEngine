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

        if (mShadowCastMaterial)
            delete mShadowCastMaterial;
        if (mGBufferMaterial)
			delete mGBufferMaterial;
        for (auto material : mNonGSCubeShadowCastMaterials)
            delete material;
    }

    ComponentType MeshRenderer::GetInsType()
    {
        return ComponentType::MeshRenderer;
    }

    void MeshRenderer::Draw()
    {
        for (auto& mesh : mMeshes)
        {
#ifndef ZX_COMPUTE_ANIMATION
            if (mAnimator)
            {
                mMatetrial->SetMatrix("_BoneMatrices", mesh->mBonesFinalTransform.data(), static_cast<uint32_t>(mesh->mBonesFinalTransform.size()));
            }
#endif
            RenderAPI::GetInstance()->Draw(mesh->VAO);
        }
    }

    void MeshRenderer::DrawShadow(uint32_t face)
    {
        for (auto& mesh : mMeshes)
        {
#ifndef ZX_COMPUTE_ANIMATION
            if (mAnimator)
            {
                if (face == UINT32_MAX)
                    mShadowCastMaterial->SetMatrix("_BoneMatrices", mesh->mBonesFinalTransform.data(), static_cast<uint32_t>(mesh->mBonesFinalTransform.size()));
                else
                    mNonGSCubeShadowCastMaterials[face]->SetMatrix("_BoneMatrices", mesh->mBonesFinalTransform.data(), static_cast<uint32_t>(mesh->mBonesFinalTransform.size()));
            }
#endif
            RenderAPI::GetInstance()->Draw(mesh->VAO);
        }
    }

    void MeshRenderer::GenerateModel(GeometryType type)
    {
        mModelName = ModelUtil::GetGeometryTypeName(type);
        mMeshes.push_back(ModelUtil::GenerateGeometry(type));
        UpdateInternalData();
    }

    void MeshRenderer::SetMeshes(const vector<shared_ptr<Mesh>>& meshes)
    {
		mMeshes = meshes;
		UpdateInternalData();
    }

    void MeshRenderer::UpdateInternalData()
    {
        for (auto& mesh : mMeshes)
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