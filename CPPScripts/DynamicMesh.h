#pragma once
#include "pubh.h"
#include "RenderAPI.h"

namespace ZXEngine
{
    class DynamicMesh
    {
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        // Vertex Array Object
        unsigned int VAO;

        DynamicMesh(unsigned int vertexSize, unsigned int indexSize);

        // 把当前Mesh数据注册到OpenGL状态机
        void Use();
        void UpdateData(vector<Vertex> vertices, vector<unsigned int> indices);

    private:
        // Vertex Buffer Objects
        unsigned int VBO;
        // Element Buffer Objects
        unsigned int EBO;

        unsigned int vertexSize;
        unsigned int indexSize;
    };
}