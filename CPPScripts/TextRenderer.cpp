#include "TextRenderer.h"
#include "ZShader.h"
#include "TextCharactersManager.h"

namespace ZXEngine
{
    TextRenderer::TextRenderer()
    {
        text = "";
        pos = vec2(0, 0);
        scale = 10;
        color = vec3(1, 1, 1);

        // configure VAO/VBO for texture quads
        // -----------------------------------
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void TextRenderer::Render()
    {
        // activate corresponding render state	
        TextCharactersManager::GetInstance()->textShader->Use();
        TextCharactersManager::GetInstance()->textShader->SetVec3("_TextColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        float tmpX = pos.x;
        float tmpY = pos.y;
        // iterate through all characters
        string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = TextCharactersManager::GetInstance()->Characters[*c];

            float xpos = tmpX + ch.Bearing.x * scale;
            float ypos = tmpY - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            TextCharactersManager::GetInstance()->textShader->SetTexture("_Text", ch.TextureID, 0);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            tmpX += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}