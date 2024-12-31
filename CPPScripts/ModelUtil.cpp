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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 0.0f;
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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 1.0f;
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
            mesh->mAABBSizeX = 10.0f;
            mesh->mAABBSizeY = 0.0f;
            mesh->mAABBSizeZ = 10.0f;
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
            mesh->mAABBSizeX = 1.0f;
            mesh->mAABBSizeY = 1.0f;
            mesh->mAABBSizeZ = 0.0f;
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
			mesh->mAABBSizeX = 5.0f;
			mesh->mAABBSizeY = 0.0f;
			mesh->mAABBSizeZ = 5.0f;
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

        // ��ASSIMP����ģ���ļ�
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace 
            | aiProcess_FixInfacingNormals | aiProcess_FlipWindingOrder | aiProcess_LimitBoneWeights);
        
        // ����쳣
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::LogError("ASSIMP: %s", importer.GetErrorString());
            return pModelData;
        }

        // ����������
        if (loadFullAnim)
            pModelData->pAnimationController = ProcessAnimation(scene);
        else
            LoadAnimBriefInfos(scene, pModelData);

        // ����ģ�ͺ͹�������
        if (pModelData->pAnimationController)
        {
            pModelData->pRootBoneNode = new BoneNode();
            ProcessNode(scene->mRootNode, scene, pModelData, pModelData->pRootBoneNode, Matrix4::Identity, async);
        }
        // ����ģ������
        else
		{
			ProcessNode(scene->mRootNode, scene, pModelData, async);
		}

        pModelData->isConstructed = true;
        return pModelData;
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData* pModelData, bool async)
    {
        // ����Mesh����
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode��������������ȡaiScene�е�ʵ�ʶ���
            // aiScene�����������ݣ�aiNodeֻ��Ϊ����������֯����(�����¼�ڵ�֮��Ĺ�ϵ)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
            pModelData->pMeshes.push_back(ProcessMesh(mesh, async));
        }

        // �ݹ鴦���ӽڵ�
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            ProcessNode(pNode->mChildren[i], pScene, pModelData, async);
        }
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, ModelData* pModelData, BoneNode* pBoneNode, const Matrix4& parentTrans, bool async)
    {
        // ���ع����ڵ�
        pBoneNode->name = pNode->mName.C_Str();
        pBoneNode->transform = aiMatrix4x4ToMatrix4(pNode->mTransformation);

        Matrix4 nodeTransform = parentTrans * pBoneNode->transform;

        // ����Mesh����
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode��������������ȡaiScene�е�ʵ�ʶ���
            // aiScene�����������ݣ�aiNodeֻ��Ϊ����������֯����(�����¼�ڵ�֮��Ĺ�ϵ)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
            pModelData->pMeshes.push_back(ProcessMesh(mesh, async));
            pModelData->pMeshes.back()->mRootTrans = nodeTransform;
        }

        // �ݹ鴦���ӽڵ�
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            pBoneNode->children.push_back(new BoneNode());
            ProcessNode(pNode->mChildren[i], pScene, pModelData, pBoneNode->children.back(), nodeTransform, async);
        }
    }

    shared_ptr<StaticMesh> ModelUtil::ProcessMesh(const aiMesh* mesh, bool async)
    {
        // ��������
        vector<Vertex> vertices;
        vector<uint32_t> indices;
        array<Vertex, 6> extremeVertices;

        if (mesh->mNumVertices > 0)
            vertices.reserve(mesh->mNumVertices);

        // ��������
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            // ����ת��aiVector3D��Vector3����ʱ����
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
                Vector2 vec;
                // һ�����������԰���8����ͬ���������꣬������ʱĬ��ֻʹ�õ�һ��
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = Vector2(0.0f, 0.0f);
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
        // ��ӹ�����Ϣ
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

                // ��������ID��ӳ��
                if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end())
                {
                    boneNameToIndexMap[boneName] = i;
                }
                else
                {
                    Debug::LogWarning("Duplicate bone name %s", boneName);
                }

                // ��������Ϣ��ӵ�������
                for (unsigned int j = 0; j < bone->mNumWeights; j++)
				{
					uint32_t vertexID = bone->mWeights[j].mVertexId;
					float weight = bone->mWeights[j].mWeight;
					vertices[vertexID].AddBoneData(i, weight);
				}
            }
        }

        // ����Face(������ͼԪ)
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            // �洢��������������
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        auto newMesh = new StaticMesh(std::move(vertices), std::move(indices), !async);
        newMesh->mBonesFinalTransform.resize(boneOffsetMatrices.size());
        newMesh->mBonesOffset = std::move(boneOffsetMatrices);
        newMesh->mBoneNameToIndexMap = std::move(boneNameToIndexMap);

        newMesh->mExtremeVertices = std::move(extremeVertices);
        newMesh->mAABBSizeX = newMesh->mExtremeVertices[0].Position.x - newMesh->mExtremeVertices[1].Position.x;
        newMesh->mAABBSizeY = newMesh->mExtremeVertices[2].Position.y - newMesh->mExtremeVertices[3].Position.y;
        newMesh->mAABBSizeZ = newMesh->mExtremeVertices[4].Position.z - newMesh->mExtremeVertices[5].Position.z;

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