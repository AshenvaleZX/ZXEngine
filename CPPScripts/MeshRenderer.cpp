#include "MeshRenderer.h"

namespace ZXEngine
{
    ComponentType MeshRenderer::GetType()
    {
        return ComponentType::T_MeshRenderer;
    }

    MeshRenderer::MeshRenderer()
    {
        for (unsigned int i = 0; i < 6; i++)
        {
            extremeVertices[i] = Vertex();
            extremeVertices[i].Position = Vector3(0.0f);
        }
    }

    MeshRenderer::~MeshRenderer()
    {
        delete matetrial;
        for (auto mesh : meshes)
            delete mesh;
    }

    ComponentType MeshRenderer::GetInsType()
    {
        return ComponentType::T_MeshRenderer;
    }

    float MeshRenderer::GetModelSizeX()
    {
        return extremeVertices[0].Position.x - extremeVertices[1].Position.x;
    }

    float MeshRenderer::GetModelSizeY()
    {
        return extremeVertices[2].Position.y - extremeVertices[3].Position.y;
    }

    float MeshRenderer::GetModelSizeZ()
    {
        return extremeVertices[4].Position.z - extremeVertices[5].Position.z;
    }

	void MeshRenderer::LoadModel(string const& path)
	{
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        ProcessNode(scene->mRootNode, scene);
	}

    void MeshRenderer::ProcessNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh* MeshRenderer::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;

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

            // tangent
            if (mesh->mTangents == nullptr)
            {
                vertex.Tangent = Vector3(0.0f, 0.0f, 0.0f);
            }
            else
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            }

            // bitangent
            if (mesh->mBitangents == nullptr)
            {
                vertex.Bitangent = Vector3(0.0f, 0.0f, 0.0f);
            }
            else
            {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            CheckExtremeVertex(vertex);
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return new Mesh(vertices, indices);
    }

    void MeshRenderer::CheckExtremeVertex(const Vertex& vertex)
    {
        if (vertex.Position.x > extremeVertices[0].Position.x)
            extremeVertices[0] = vertex;
        else if (vertex.Position.x < extremeVertices[1].Position.x)
            extremeVertices[1] = vertex;
        else if (vertex.Position.y > extremeVertices[1].Position.y)
            extremeVertices[2] = vertex;
        else if (vertex.Position.y < extremeVertices[1].Position.y)
            extremeVertices[3] = vertex;
        else if (vertex.Position.z > extremeVertices[1].Position.z)
            extremeVertices[4] = vertex;
        else if (vertex.Position.z < extremeVertices[1].Position.z)
            extremeVertices[5] = vertex;
    }
}