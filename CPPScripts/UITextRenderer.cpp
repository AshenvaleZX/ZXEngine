#include "UITextRenderer.h"
#include "ZShader.h"
#include "TextCharactersManager.h"
#include "DynamicMesh.h"
#include "Transform.h"

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
        TextCharactersManager::GetInstance()->BeginRender();
        TextCharactersManager::GetInstance()->SetColor(color);

        float tmpX = this->GetTransform()->GetPosition().x;
        float tmpY = this->GetTransform()->GetPosition().y;
        float scale = this->GetTransform()->GetScale().x;
        // 遍历字符串的每个字符
        string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = TextCharactersManager::GetInstance()->Characters[*c];

            // 计算字符位置和大小
            float xpos = tmpX + ch.Bearing[0] * scale;
            float ypos = tmpY - (ch.Size[1] - ch.Bearing[1]) * scale;
            float w = ch.Size[0] * scale;
            float h = ch.Size[1] * scale;
            
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
            
            // 设置字形(glyph)纹理
            TextCharactersManager::GetInstance()->SetTexture(ch.TextureID);
            TextCharactersManager::GetInstance()->UpdateCharacterMesh(vertices, GlyphIndices);
            TextCharactersManager::GetInstance()->DrawCharacter();

            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            tmpX += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
}