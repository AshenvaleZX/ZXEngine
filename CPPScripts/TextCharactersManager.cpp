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
        // 新建渲染字符的动态Mesh，一个矩形Mesh即可，4个顶点，2个三角形
        characterMesh = new DynamicMesh(4, 6);

        // 初始化字符渲染Shader
        Matrix4 mat_P = Math::Orthographic(-static_cast<float>(GlobalData::srcWidth) / 2.0f, static_cast<float>(GlobalData::srcWidth) / 2.0f, -static_cast<float>(GlobalData::srcHeight) / 2.0f, static_cast<float>(GlobalData::srcHeight) / 2.0f);
        textShader = new Shader(Resources::GetAssetFullPath("Shaders/TextRenderer.zxshader", true), FrameBufferType::Present);
        textShader->Use();
        textShader->SetMat4("ENGINE_Projection", mat_P);

        // 加载字符
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
#ifdef ZX_API_VULKAN
            if (face->glyph->bitmap.width == 0 || face->glyph->bitmap.rows == 0)
            {
                Debug::LogWarning("Zero pixel glyph textures are not supported.");
                continue;
            }
#endif
            // generate texture
            unsigned int textureID = RenderAPI::GetInstance()->GenerateTextTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
            // now store character for later use
            Character character =
            {
                textureID,
                { face->glyph->bitmap.width, face->glyph->bitmap.rows },
                { face->glyph->bitmap_left, face->glyph->bitmap_top },
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

    void TextCharactersManager::SetColor(Vector3 color)
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
        RenderAPI::GetInstance()->Draw(characterMesh->VAO);
    }
}