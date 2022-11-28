#include "TextRenderer.h"
#include "ZShader.h"
#include "TextCharactersManager.h"
#include "DynamicMesh.h"

namespace ZXEngine
{
    TextRenderer::TextRenderer()
    {
        text = "";
        pos = vec2(0, 0);
        scale = 1;
        color = vec3(1, 1, 1);
    }

    void TextRenderer::Render()
    {
        TextCharactersManager::GetInstance()->BeginRender();
        TextCharactersManager::GetInstance()->SetColor(color);

        float tmpX = pos.x;
        float tmpY = pos.y;
        // 遍历字符串的每个字符
        string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = TextCharactersManager::GetInstance()->Characters[*c];

            // 计算字符位置和大小
            float xpos = tmpX + ch.Bearing.x * scale;
            float ypos = tmpY - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            
            // 设置字符顶点数据
            vec3 points[4] =
            {
                vec3(xpos,     ypos,     0),
                vec3(xpos,     ypos + h, 0),
                vec3(xpos + w, ypos,     0),
                vec3(xpos + w, ypos + h, 0),
            };
            vector<Vertex> vertices;
            for (unsigned int i = 0; i < 4; i++)
            {
                Vertex vertex;
                vertex.Position = points[i];
                vertex.Normal = vec3(1);
                vertex.Tangent = vec3(1);
                vertex.Bitangent = vec3(1);
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