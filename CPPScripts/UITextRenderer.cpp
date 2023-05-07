#include "UITextRenderer.h"
#include "ZShader.h"
#include "TextCharactersManager.h"
#include "DynamicMesh.h"
#include "Transform.h"
#include "GlobalData.h"
#include "Material.h"

namespace ZXEngine
{
    ComponentType UITextRenderer::GetType()
    {
        return ComponentType::UITextRenderer;
    }

    UITextRenderer::UITextRenderer()
    {
        text = "";
        color = Vector4(1, 1, 1, 1);
    }

    ComponentType UITextRenderer::GetInsType()
    {
        return ComponentType::UITextRenderer;
    }

    void UITextRenderer::Render()
    {
        // 如果文本发生了变化，重新生成渲染数据
        if (dirty)
        {
            for (auto material : textMaterials)
                delete material;
            textMaterials.clear();
            GenerateRenderData();
        }

        auto renderAPI = RenderAPI::GetInstance();
        for (size_t i = 0; i < length; i++)
        {
            textMaterials[i]->Use();
            renderAPI->Draw(textMeshes[i]->VAO);
        }
    }

    void UITextRenderer::SetContent(const string& text)
    {
        dirty = true;
        this->text = text;
    }

    void UITextRenderer::GenerateRenderData()
    {
        float tmpX = 0, tmpY = 0;
        Matrix4 mat_M = GetTransform()->GetModelMatrix();
        Matrix4 mat_P = Math::Orthographic(-static_cast<float>(GlobalData::srcWidth) / 2.0f, static_cast<float>(GlobalData::srcWidth) / 2.0f, -static_cast<float>(GlobalData::srcHeight) / 2.0f, static_cast<float>(GlobalData::srcHeight) / 2.0f);

        auto characterMgr = TextCharactersManager::GetInstance();

        length = 0;
        // 遍历字符串的每个字符
        string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = characterMgr->Characters[*c];

            // 如果遇到空格等无实际图像的字符，直接后移一段距离空出来，不做实际渲染
            if (ch.TextureID == UINT32_MAX)
            {
                tmpX += (ch.Advance >> 6) * size;
                continue;
            }

            // 计算字符位置和大小
            float xpos = tmpX + ch.Bearing[0] * size;
            float ypos = tmpY - (ch.Size[1] - ch.Bearing[1]) * size;
            float w = ch.Size[0] * size;
            float h = ch.Size[1] * size;

            // 设置字符顶点数据
            Vector3 points[4] =
            {
                Vector3(xpos,     ypos,     0),
                Vector3(xpos,     ypos + h, 0),
                Vector3(xpos + w, ypos,     0),
                Vector3(xpos + w, ypos + h, 0),
            };
            vector<Vertex> vertices;
            for (unsigned int i = 0; i < 4; i++)
            {
                Vertex vertex;
                vertex.Position = points[i];
                vertex.Normal = Vector3(1);
                vertex.Tangent = Vector3(1);
                vertex.Bitangent = Vector3(1);
                vertex.TexCoords = GlyphCoords[i];
                vertices.push_back(vertex);
            }

            // 更新字符Mesh
            if (length >= textMeshes.size())
            {
                auto charMesh = new DynamicMesh(4, 6);
                charMesh->UpdateData(vertices, GlyphIndices);
                textMeshes.push_back(charMesh);
            }
            else
            {
                textMeshes[length]->UpdateData(vertices, GlyphIndices);
            }

            // 字符材质
            auto charMaterial = new Material(characterMgr->textShader);
            charMaterial->Use();
            charMaterial->SetVector("_TextColor", color, true);
            charMaterial->SetTexture("_Text", ch.TextureID, 0, true);
            charMaterial->SetMatrix("ENGINE_Model", mat_M, true);
            charMaterial->SetMatrix("ENGINE_Projection", mat_P, true);
            textMaterials.push_back(charMaterial);

            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            tmpX += (ch.Advance >> 6) * size; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

            length++;
        }

        dirty = false;
    }
}