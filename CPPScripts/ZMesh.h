#pragma once
#include "pubh.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	class Mesh
	{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        // Vertex Array Object
        unsigned int VAO;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
        ~Mesh();
        
        // �ѵ�ǰMesh����ע�ᵽOpenGL״̬��
        void Use();

    private:
        // Vertex Buffer Objects
        unsigned int VBO;
        // Element Buffer Objects
        unsigned int EBO;
    };
}