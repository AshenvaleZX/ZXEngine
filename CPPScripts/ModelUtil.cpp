#include "ModelUtil.h"
#include "PublicStruct.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Resources.h"
#include "GeometryGenerator.h"
#include "Animation/Animation.h"
#include "Animation/NodeAnimation.h"
#include "Animation/AnimationController.h"

namespace ZXEngine
{
    const unordered_map<GeometryType, string> GeometryTypeName =
    {
        { GeometryType::Box,                "Box"                }, { GeometryType::Sphere,   "Sphere"   },
        { GeometryType::TessellationSphere, "TessellationSphere" }, { GeometryType::Cylinder, "Cylinder" },
        { GeometryType::Plane,              "Plane"              }, { GeometryType::Quad,     "Quad"     },
        { GeometryType::DynamicPlane,       "DynamicPlane"       }, { GeometryType::Cone,     "Cone"     },
        { GeometryType::Circle,             "Circle"             }
    };

    shared_ptr<Mesh> ModelUtil::GenerateGeometry(GeometryType type)
    {
        Mesh* mesh = nullptr;

        if (type == GeometryType::Box)
        {
            mesh = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 1.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3(-0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f,  0.5f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, -0.5f) }
            };
        }
        else if (type == GeometryType::Sphere)
        {
            mesh = GeometryGenerator::CreateSphere(0.5f, 40, 20);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 1.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3(-0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f,  0.5f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, -0.5f) }
            };
        }
        else if (type == GeometryType::TessellationSphere)
        {
            mesh = GeometryGenerator::CreateSphereTessellation(0.5f, 4);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 1.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3(-0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f,  0.5f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, -0.5f) }
            };
        }
        else if (type == GeometryType::Circle)
        {
            mesh = GeometryGenerator::CreateCircle(0.5f, 40);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 0.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f,  0.0f, 0.0f) },
                Vertex{ .Position = Vector3(-0.5f,  0.0f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, 0.0f) }
            };
        }
        else if (type == GeometryType::Cone)
        {
            mesh = GeometryGenerator::CreateCone(0.5f, 1.0f, 40);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 1.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3(-0.5f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f,  0.5f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f, -0.5f) }
            };
        }
        else if (type == GeometryType::Cylinder)
        {
            mesh = GeometryGenerator::CreateCylinder(0.5f, 0.5f, 1.0f, 40, 1);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 1.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3(-0.5f,  0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f,  0.5f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, -0.5f) }
            };
        }
        else if (type == GeometryType::Plane)
        {
            mesh = GeometryGenerator::CreatePlane(10.0f, 10.0f, 11, 11);
            mesh->mAABBSize.x = 10.0f;
            mesh->mAABBSize.y = 0.0f;
            mesh->mAABBSize.z = 10.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 5.0f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3(-5.0f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f,  5.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f, -5.0f) }
            };
        }
        else if (type == GeometryType::Quad)
        {
            mesh = GeometryGenerator::CreateQuad(1.0f, 1.0f);
            mesh->mAABBSize.x = 1.0f;
            mesh->mAABBSize.y = 1.0f;
            mesh->mAABBSize.z = 0.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 0.5f,  0.0f, 0.0f) },
                Vertex{ .Position = Vector3(-0.5f,  0.0f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.5f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f, -0.5f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, 0.0f) },
                Vertex{ .Position = Vector3( 0.0f,  0.0f, 0.0f) }
            };
        }
        else if (type == GeometryType::DynamicPlane)
        {
			mesh = GeometryGenerator::CreateDynamicPlane(5.0f, 5.0f, 11, 11);
			mesh->mAABBSize.x = 5.0f;
			mesh->mAABBSize.y = 0.0f;
			mesh->mAABBSize.z = 5.0f;
            mesh->mExtremeVertices =
            {
                Vertex{ .Position = Vector3( 2.5f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3(-2.5f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f,  0.0f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f,  2.5f) },
                Vertex{ .Position = Vector3( 0.0f, 0.0f, -2.5f) }
            };
        }
        else
        {
            Debug::LogError("Try to generate invalid geometry type!");
        }

        return shared_ptr<Mesh>(mesh);
    }

    string ModelUtil::GetGeometryTypeName(GeometryType type)
    {
        return GeometryTypeName.at(type);
    }

    ModelData* ModelUtil::LoadModel(const string& path, bool loadFullAnim, bool async)
    {
        ModelData* pModelData = new ModelData();

        vector<unsigned char> buffer;
        Resources::LoadBinaryFile(buffer, path);

        // 用ASSIMP加载模型文件
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(),
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals
            | aiProcess_FlipWindingOrder | aiProcess_LimitBoneWeights, Resources::GetAssetExtension(path).c_str());
        
        // 检查异常
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::LogError("ASSIMP: %s", importer.GetErrorString());
            return pModelData;
        }

        // 处理动画数据
        if (loadFullAnim)
            pModelData->pAnimationController = ProcessAnimation(scene);
        else
            LoadAnimBriefInfos(scene, pModelData);

        // 处理模型和骨骼数据
        if (pModelData->pAnimationController)
        {
            pModelData->pRootBoneNode = new BoneNode();
            ProcessNode(scene->mRootNode, scene, pModelData, pModelData->pRootBoneNode, Matrix4::Identity, async);
        }
        // 处理模型数据
        else
		{
			ProcessNode(scene->mRootNode, scene, pModelData, async);
		}

        pModelData->isConstructed = true;
        return pModelData;
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData* pModelData, bool async)
    {
        // 处理Mesh数据
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode仅包含索引来获取aiScene中的实际对象
            // aiScene包含所有数据，aiNode只是为了让数据组织起来(比如记录节点之间的关系)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
            pModelData->pMeshes.push_back(ProcessMesh(mesh, async, false));
        }

        // 递归处理子节点
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            ProcessNode(pNode->mChildren[i], pScene, pModelData, async);
        }
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData* pModelData, BoneNode* pBoneNode, const Matrix4& parentTrans, bool async)
    {
        // 加载骨骼节点
        pBoneNode->name = pNode->mName.C_Str();
        pBoneNode->transform = aiMatrix4x4ToMatrix4(pNode->mTransformation);

        Matrix4 nodeTransform = parentTrans * pBoneNode->transform;

        // 处理Mesh数据
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode仅包含索引来获取aiScene中的实际对象
            // aiScene包含所有数据，aiNode只是为了让数据组织起来(比如记录节点之间的关系)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];

            auto pMesh = ProcessMesh(mesh, async, true);
            pMesh->mRootTrans = nodeTransform;

#ifdef ZX_COMPUTE_SHADER_SUPPORT
            pMesh->mVertexSSBO = RenderAPI::GetInstance()->CreateShaderStorageBuffer(
                pMesh->mVertices.data(), pMesh->mVertices.size() * sizeof(Vertex), GPUBufferType::Static);

            pMesh->mBoneTransformSSBO = RenderAPI::GetInstance()->CreateShaderStorageBuffer(
                pMesh->mBonesFinalTransform.data(), pMesh->mBonesFinalTransform.size() * sizeof(Matrix4), GPUBufferType::DynamicCPUWriteGPURead);
#endif

            pModelData->pMeshes.push_back(pMesh);
        }

        // 递归处理子节点
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            pBoneNode->children.push_back(new BoneNode());
            ProcessNode(pNode->mChildren[i], pScene, pModelData, pBoneNode->children.back(), nodeTransform, async);
        }
    }

    shared_ptr<StaticMesh> ModelUtil::ProcessMesh(const aiMesh* mesh, bool async, bool skinned)
    {
        // 顶点数据
        vector<Vertex> vertices;
        vector<uint32_t> indices;
        array<Vertex, 6> extremeVertices;

        if (mesh->mNumVertices > 0)
            vertices.reserve(mesh->mNumVertices);

        // 遍历顶点
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            // 用于转换aiVector3D到Vector3的临时变量
            Vector3 vector;
            
            // Position
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            // Normal
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            
            // Texture Coords
            if (mesh->mTextureCoords[0])
            {
                // 一个顶点最多可以包含8个不同的纹理坐标，但是暂时默认只使用第一个
                vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vertex.TexCoords = Vector4::Zero;
            }

            // Tangent & Bitangent
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
            if (mesh->mNumBones > MAX_BONE_NUM)
            {
                Debug::LogWarning("Too many bones in mesh %s, num: %s", mesh->mName.C_Str(), mesh->mNumBones);
            }

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

            for (auto& vertex : vertices)
            {
                vertex.NormalizeWeights();
            }
        }

        // 遍历Face(三角形图元)
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            // 存储这个面的所有索引
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        auto newMesh = new StaticMesh(std::move(vertices), std::move(indices), !async, skinned);
        newMesh->mBonesFinalTransform.resize(boneOffsetMatrices.size());
        newMesh->mBonesOffset = std::move(boneOffsetMatrices);
        newMesh->mBoneNameToIndexMap = std::move(boneNameToIndexMap);

        newMesh->mExtremeVertices = std::move(extremeVertices);
        newMesh->mAABBSize.x = newMesh->mExtremeVertices[0].Position.x - newMesh->mExtremeVertices[1].Position.x;
        newMesh->mAABBSize.y = newMesh->mExtremeVertices[2].Position.y - newMesh->mExtremeVertices[3].Position.y;
        newMesh->mAABBSize.z = newMesh->mExtremeVertices[4].Position.z - newMesh->mExtremeVertices[5].Position.z;
        newMesh->mAABBCenter.x = (newMesh->mExtremeVertices[0].Position.x + newMesh->mExtremeVertices[1].Position.x) / 2.0f;
        newMesh->mAABBCenter.y = (newMesh->mExtremeVertices[2].Position.y + newMesh->mExtremeVertices[3].Position.y) / 2.0f;
        newMesh->mAABBCenter.z = (newMesh->mExtremeVertices[4].Position.z + newMesh->mExtremeVertices[5].Position.z) / 2.0f;

        return shared_ptr<StaticMesh>(newMesh);
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

    void ModelUtil::LoadAnimBriefInfos(const aiScene* pScene, ModelData* pModelData)
    {
        if (!pScene->HasAnimations())
            return;

        CountNode(pScene->mRootNode, pModelData->boneNum);

        for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
        {
            const aiAnimation* pAnimation = pScene->mAnimations[i];

            AnimBriefInfo briefInfo;
            briefInfo.name = pAnimation->mName.C_Str();
            briefInfo.duration = static_cast<float>(pAnimation->mDuration / pAnimation->mTicksPerSecond);

            pModelData->animBriefInfos.push_back(std::move(briefInfo));
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