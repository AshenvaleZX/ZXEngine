#include "TextCharactersManager.h"
#include "Resources.h"
#include "ZShader.h"
#include "DynamicMesh.h"
#include "GlobalData.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace ZXEngine
{
	TextCharactersManager* TextCharactersManager::mInstance = nullptr;

	void TextCharactersManager::Create()
	{
		mInstance = new TextCharactersManager();
	}

	TextCharactersManager* TextCharactersManager::GetInstance()
	{
		return mInstance;
	}

	TextCharactersManager::TextCharactersManager()
	{
        // �½���Ⱦ�ַ��Ķ�̬Mesh��һ������Mesh���ɣ�4�����㣬2��������
        characterMesh = new DynamicMesh(4, 6);

        // ��ʼ���ַ���ȾShader
        mat4 mat_P = glm::ortho(0.0f, (float)GlobalData::srcWidth, 0.0f, (float)GlobalData::srcHeight);
        textShader = new Shader(Resources::GetAssetFullPath("Shaders/TextRenderer.zxshader").c_str());
        textShader->Use();
        textShader->SetMat4("projection", mat_P);

        // �����ַ�
        LoadCharacters();
	}

    void TextCharactersManager::LoadCharacters()
    {
        // FreeType
        // --------
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft))
        {
            Debug::LogError("ERROR::FREETYPE: Could not init FreeType Library");
            return;
        }

        // find path to font
        string fontPath = Resources::GetAssetFullPath("Fonts/arial.ttf");
        if (fontPath.empty())
        {
            Debug::LogError("ERROR::FREETYPE: Failed to load font_name");
            return;
        }

        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            Debug::LogError("ERROR::FREETYPE: Failed to load font");
            return;
        }

        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                Debug::LogError("ERROR::FREETYTPE: Failed to load Glyph");
                continue;
            }
            // generate texture
            unsigned int textureID = RenderAPI::GetInstance()->GenerateTextTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
            // now store character for later use
            Character character =
            {
                textureID,
                ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(pair<char, Character>(c, character));
        }

        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void TextCharactersManager::BeginRender()
    {
        textShader->Use();
    }

    void TextCharactersManager::SetColor(vec3 color)
    {
        textShader->SetVec3("_TextColor", color);
    }

    void TextCharactersManager::SetTexture(unsigned int ID)
    {
        textShader->SetTexture("_Text", ID, 0);
    }

    void TextCharactersManager::UpdateCharacterMesh(vector<Vertex> vertices, vector<unsigned int> indices)
    {
        characterMesh->UpdateData(vertices, indices);
    }

    void TextCharactersManager::DrawCharacter()
    {
        characterMesh->Use();
        RenderAPI::GetInstance()->Draw();
    }
}