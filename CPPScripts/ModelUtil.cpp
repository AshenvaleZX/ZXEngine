#include "ModelUtil.h"
#include "PublicStruct.h"
#include "Material.h"
#include "StaticMesh.h"
#include "GeometryGenerator.h"
#include "Component/MeshRenderer.h"
#include "Animation/Animation.h"
#include "Animation/NodeAnimation.h"
#include "Animation/AnimationController.h"

namespace ZXEngine
{
    const unordered_map<GeometryType, string> GeometryTypeName =
    {
        { GeometryType::Box,      "Box"      }, { GeometryType::Sphere, "Sphere" }, { GeometryType::TessellationSphere, "TessellationSphere" },
        { GeometryType::Cylinder, "Cylinder" }, { GeometryType::Plane,  "Plane"  }, { GeometryType::Quad,               "Quad"               }
    };

    Mesh* ModelUtil::GenerateGeometry(GeometryType type)
    {
        StaticMesh* mesh = nullptr;

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

    void ModelUtil::LoadModel(const string& path, MeshRenderer* pMeshRenderer)
    {
        // ��ASSIMP����ģ���ļ�
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals | aiProcess_FlipWindingOrder);
        
        // ����쳣
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::LogError("ASSIMP: %s", importer.GetErrorString());
            return;
        }

        pMeshRenderer->mRootBone = new BoneNode();

        // ����ģ�ͺ͹�������
        ProcessNode(scene->mRootNode, scene, pMeshRenderer, pMeshRenderer->mRootBone);
        Matrix4 globalInverseTransform = Math::Inverse(aiMatrix4x4ToMatrix4(scene->mRootNode->mTransformation));
        for (auto mesh : pMeshRenderer->mMeshes)
		{
			mesh->mRootBoneToWorld = globalInverseTransform;
		}

        // ����������
        ProcessAnimation(scene, pMeshRenderer);
    }

    void ModelUtil::ProcessNode(const aiNode* pNode, const aiScene* pScene, MeshRenderer* pMeshRenderer, BoneNode* pBoneNode)
    {
        // ����Mesh����
        for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            // aiNode��������������ȡaiScene�е�ʵ�ʶ���
            // aiScene�����������ݣ�aiNodeֻ��Ϊ����������֯����(�����¼�ڵ�֮��Ĺ�ϵ)
            aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
            pMeshRenderer->mMeshes.push_back(ProcessMesh(mesh));
        }

        // ���ع�������
        pBoneNode->name = pNode->mName.C_Str();
        pBoneNode->transform = aiMatrix4x4ToMatrix4(pNode->mTransformation);

        // �ݹ鴦���ӽڵ�
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            pBoneNode->children.push_back(new BoneNode());
            ProcessNode(pNode->mChildren[i], pScene, pMeshRenderer, pBoneNode->children.back());
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

        vector<BoneInfo> bones;
        unordered_map<string, uint32_t> boneNameToIndexMap;
        // ��ӹ�����Ϣ
        if (mesh->HasBones())
        {
            for (unsigned int i = 0; i < mesh->mNumBones; i++)
            {
                const aiBone* bone = mesh->mBones[i];

                string boneName(bone->mName.C_Str());

                bones.push_back(BoneInfo(aiMatrix4x4ToMatrix4(bone->mOffsetMatrix)));

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
        newMesh->mBones = std::move(bones);
		newMesh->mBoneNameToIndexMap = std::move(boneNameToIndexMap);
        newMesh->mExtremeVertices = std::move(extremeVertices);
        newMesh->mAABBSizeX = newMesh->mExtremeVertices[0].Position.x - newMesh->mExtremeVertices[1].Position.x;
        newMesh->mAABBSizeY = newMesh->mExtremeVertices[2].Position.y - newMesh->mExtremeVertices[3].Position.y;
        newMesh->mAABBSizeZ = newMesh->mExtremeVertices[4].Position.z - newMesh->mExtremeVertices[5].Position.z;

		return newMesh;
    }

    void ModelUtil::ProcessAnimation(const aiScene* pScene, MeshRenderer* pMeshRenderer)
    {
        if (!pScene->HasAnimations())
			return;

        pMeshRenderer->mAnimationController = new AnimationController();

        for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
		{
			const aiAnimation* pAnimation = pScene->mAnimations[i];

			Animation* pAnim = new Animation();
			pAnim->mName = pAnimation->mName.C_Str();
            pAnim->mDuration = static_cast<float>(pAnimation->mDuration);
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

			pMeshRenderer->mAnimationController->AddAnimation(pAnim);
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