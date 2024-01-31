#include "ModelUtil.h"
#include "PublicStruct.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "GeometryGenerator.h"
#include "Animation/Animation.h"
#include "Animation/NodeAnimation.h"
#include "Animation/AnimationController.h"

namespace ZXEngine
{
    const unordered_map<GeometryType, string> GeometryTypeName =
    {
        { GeometryType::Box,      "Box"      }, { GeometryType::Sphere, "Sphere" }, { GeometryType::TessellationSphere, "TessellationSphere" },
        { GeometryType::Cylinder, "Cylinder" }, { GeometryType::Plane,  "Plane"  }, { GeometryType::Quad,               "Quad"               },
		{ GeometryType::DynamicPlane, "DynamicPlane" }
    };

    Mesh* ModelUtil::GenerateGeometry(GeometryType type)
    {
        Mesh* mesh = nullptr;

        if (type == GeometryType::Box)
        {
            mesh = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f);
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
        }
        else if (type == GeometryType::Sphere)
        {
            mesh = GeometryGenerator::CreateSphere(0.5f, 40, 20);
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
        }
        else if (type == GeometryType::TessellationSphere)
        {
            mesh = GeometryGenerator::CreateSphereTessellation(0.5f, 4);
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
        }
        else if (type == GeometryType::Cylinder)
        {
            mesh = GeometryGenerator::CreateCylinder(0.5f, 0.5f, 2.0f, 40, 20);
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 2.0f;
            mesh->mAABBSizeZ = 1.0f;
        }
        else if (type == GeometryType::Plane)
        {
            mesh = GeometryGenerator::CreatePlane(10.0f, 10.0f, 11, 11);
            mesh->mAABBSizeX = 10.0f;
            mesh->mAABBSizeY = 0.0f;
            mesh->mAABBSizeZ = 10.0f;
        }
        else if (type == GeometryType::Quad)
        {
            mesh = GeometryGenerator::CreateQuad(1.0f, 1.0f);
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 0.0f;
        }
        else if (type == GeometryType::DynamicPlane)
        {
			mesh = GeometryGenerator::CreateDynamicPlane(6.0f, 6.0f, 11, 11);
			mesh->mAABBSizeX = 6.0f;
			mesh->mAABBSizeY = 0.0f;
			mesh->mAABBSizeZ = 6.0f;
        }
        else
        {
            Debug::LogError("Try to generate invalid geometry type!");
        }

        return mesh;
    }

    string ModelUtil::GetGeometryTypeName(GeometryType type)
    {
        return GeometryTypeName.at(type);
    }

    ModelData ModelUtil::LoadModel(const string& path, bool loadFullAnim)
    {
        ModelData modelData;

        // 用ASSIMP加载模型文件
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace 
            | aiProcess_FixInfacingNormals | aiProcess_FlipWindingOrder | aiProcess_LimitBoneWeights);
        
        // 检查异常
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::LogError("ASSIMP: %s", importer.GetErrorString());
            return modelData;
        }

        // 处理动画数据
        if (loadFullAnim)
            modelData.pAnimationController = ProcessAnimation(scene);
        else
            LoadAnimBriefInfos(scene, modelData);

        // 处理模型和骨骼数据
        if (modelData.pAnimationController)
        {
            modelData.pRootBoneNode = new BoneNode();
            ProcessNode(scene->mRootNode, scene, modelData, modelData.pRootBoneNode);
        }
        // 处理模型数据
        else
		{
			ProcessNode(scene->mRootNode, scene, modelData);
		}

        return modelData;
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData& modelData)
    {
        // 处理Mesh数据
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode仅包含索引来获取aiScene中的实际对象
            // aiScene包含所有数据，aiNode只是为了让数据组织起来(比如记录节点之间的关系)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
            modelData.pMeshes.push_back(ProcessMesh(mesh));
        }

        // 递归处理子节点
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            ProcessNode(pNode->mChildren[i], pScene, modelData);
        }
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData& modelData, BoneNode* pBoneNode)
    {
        // 处理Mesh数据
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode仅包含索引来获取aiScene中的实际对象
            // aiScene包含所有数据，aiNode只是为了让数据组织起来(比如记录节点之间的关系)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
            modelData.pMeshes.push_back(ProcessMesh(mesh));
        }

        // 加载骨骼数据
        pBoneNode->name = pNode->mName.C_Str();
        pBoneNode->transform = aiMatrix4x4ToMatrix4(pNode->mTransformation);

        // 递归处理子节点
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            pBoneNode->children.push_back(new BoneNode());
            ProcessNode(pNode->mChildren[i], pScene, modelData, pBoneNode->children.back());
        }
    }

    StaticMesh* ModelUtil::ProcessMesh(const aiMesh* mesh)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<uint32_t> indices;
        array<Vertex, 6> extremeVertices;

        // Walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            Vector3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to Vector3 class so we transfer the data to this placeholder Vector3 first.
            
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                Vector2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = Vector2(0.0f, 0.0f);
            }

            // tangent and bitangent
            if (mesh->HasTangentsAndBitangents())
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            }
            else
            {
                vertex.Tangent = Vector3(0.0f, 0.0f, 0.0f);
            }

            CheckExtremeVertex(vertex, extremeVertices);
            vertices.push_back(vertex);
        }

        vector<Matrix4> boneOffsetMatrices;
        unordered_map<string, uint32_t> boneNameToIndexMap;
        // 添加骨骼信息
        if (mesh->HasBones())
        {
            for (unsigned int i = 0; i < mesh->mNumBones; i++)
            {
                const aiBone* bone = mesh->mBones[i];

                string boneName(bone->mName.C_Str());

                boneOffsetMatrices.push_back(aiMatrix4x4ToMatrix4(bone->mOffsetMatrix));

                // 骨骼名到ID的映射
                if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end())
                {
                    boneNameToIndexMap[boneName] = i;
                }
                else
                {
                    Debug::LogWarning("Duplicate bone name %s", boneName);
                }

                // 将骨骼信息添加到顶点中
                for (unsigned int j = 0; j < bone->mNumWeights; j++)
				{
					uint32_t vertexID = bone->mWeights[j].mVertexId;
					float weight = bone->mWeights[j].mWeight;
					vertices[vertexID].AddBoneData(i, weight);
				}
            }
        }

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        auto newMesh = new StaticMesh(vertices, indices);
        newMesh->mBonesFinalTransform.resize(boneOffsetMatrices.size());
        newMesh->mBonesOffset = std::move(boneOffsetMatrices);
        newMesh->mBoneNameToIndexMap = std::move(boneNameToIndexMap);

        newMesh->mExtremeVertices = std::move(extremeVertices);
        newMesh->mAABBSizeX = newMesh->mExtremeVertices[0].Position.x - newMesh->mExtremeVertices[1].Position.x;
        newMesh->mAABBSizeY = newMesh->mExtremeVertices[2].Position.y - newMesh->mExtremeVertices[3].Position.y;
        newMesh->mAABBSizeZ = newMesh->mExtremeVertices[4].Position.z - newMesh->mExtremeVertices[5].Position.z;

		return newMesh;
    }

    AnimationController* ModelUtil::ProcessAnimation(const aiScene* pScene)
    {
        if (!pScene->HasAnimations())
			return nullptr;

        AnimationController* animationController = new AnimationController();

        for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
		{
			const aiAnimation* pAnimation = pScene->mAnimations[i];

			Animation* pAnim = new Animation();
			pAnim->mName = pAnimation->mName.C_Str();
            pAnim->mFullTick = static_cast<float>(pAnimation->mDuration);
            pAnim->mDuration = static_cast<float>(pAnimation->mDuration / pAnimation->mTicksPerSecond);
            pAnim->mTicksPerSecond = static_cast<float>(pAnimation->mTicksPerSecond);

			for (unsigned int j = 0; j < pAnimation->mNumChannels; j++)
			{
				const aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];

				string nodeName(pNodeAnim->mNodeName.C_Str());

                NodeAnimation* pNode = new NodeAnimation();
				pNode->mName = nodeName;

                for (unsigned int k = 0; k < pNodeAnim->mNumScalingKeys; k++)
                {
                    const aiVectorKey& aiKey = pNodeAnim->mScalingKeys[k];
                    pNode->mKeyScales.push_back(KeyVector3(static_cast<float>(aiKey.mTime), aiVector3DToVector3(aiKey.mValue)));
                }
                pNode->mKeyScaleNum = pNode->mKeyScales.size();

				for (unsigned int k = 0; k < pNodeAnim->mNumPositionKeys; k++)
				{
					const aiVectorKey& aiKey = pNodeAnim->mPositionKeys[k];
					pNode->mKeyPositions.push_back(KeyVector3(static_cast<float>(aiKey.mTime), aiVector3DToVector3(aiKey.mValue)));
				}
                pNode->mKeyPositionNum = pNode->mKeyPositions.size();

				for (unsigned int k = 0; k < pNodeAnim->mNumRotationKeys; k++)
				{
					const aiQuatKey& aiKey = pNodeAnim->mRotationKeys[k];
					pNode->mKeyRotations.push_back(KeyQuaternion(static_cast<float>(aiKey.mTime), aiQuaternionToQuaternion(aiKey.mValue)));
				}
                pNode->mKeyRotationNum = pNode->mKeyRotations.size();

                pAnim->AddNodeAnimation(pNode);
			}

            animationController->Add(pAnim);
		}

		return animationController;
    }

    void ModelUtil::CountNode(const aiNode* pNode, uint32_t& count)
    {
        count++;

        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            CountNode(pNode->mChildren[i], count);
        }
    }

    void ModelUtil::LoadAnimBriefInfos(const aiScene* pScene, ModelData& modelData)
    {
        if (!pScene->HasAnimations())
            return;

        CountNode(pScene->mRootNode, modelData.boneNum);

        for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
        {
            const aiAnimation* pAnimation = pScene->mAnimations[i];

            AnimBriefInfo briefInfo;
            briefInfo.name = pAnimation->mName.C_Str();
            briefInfo.duration = static_cast<float>(pAnimation->mDuration / pAnimation->mTicksPerSecond);

            modelData.animBriefInfos.push_back(std::move(briefInfo));
        }
    }

    void ModelUtil::CheckExtremeVertex(const Vertex& vertex, array<Vertex, 6>& extremeVertices)
    {
        if (vertex.Position.x > extremeVertices[0].Position.x)
            extremeVertices[0] = vertex;
        else if (vertex.Position.x < extremeVertices[1].Position.x)
            extremeVertices[1] = vertex;

        if (vertex.Position.y > extremeVertices[2].Position.y)
            extremeVertices[2] = vertex;
        else if (vertex.Position.y < extremeVertices[3].Position.y)
            extremeVertices[3] = vertex;

        if (vertex.Position.z > extremeVertices[4].Position.z)
            extremeVertices[4] = vertex;
        else if (vertex.Position.z < extremeVertices[5].Position.z)
            extremeVertices[5] = vertex;
    }
}