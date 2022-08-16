#pragma once
#include "pubh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace ZXEngine
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

	class Mesh
	{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        // Vertex Array Object
        unsigned int VAO;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

    private:
        // Vertex Buffer Objects
        unsigned int VBO;
        // Element Buffer Objects
        unsigned int EBO;

        void SetupMesh();
    };
}