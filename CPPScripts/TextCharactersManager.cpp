#include "TextCharactersManager.h"
#include "Resources.h"
#include "ZShader.h"
#include "RenderAPI.h"
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
        // 创建字符渲染Shader
#ifdef ZX_EDITOR
        textShader = new Shader(Resources::GetAssetFullPath("Shaders/TextRenderer.zxshader", true), FrameBufferType::Normal);
#else
        textShader = new Shader(Resources::GetAssetFullPath("Shaders/TextRenderer.zxshader", true), FrameBufferType::Present);
#endif

        // 加载字符
        LoadCharacters();
	}

    void TextCharactersManager::LoadCharacters()
    {
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft))
        {
            Debug::LogError("ERROR::FREETYPE: Could not init FreeType Library");
            return;
        }

        string fontPath = Resources::GetAssetFullPath("Fonts/arial.ttf", true);
        if (fontPath.empty())
        {
            Debug::LogError("ERROR::FREETYPE: Failed to load font_name");
            return;
        }

        vector<char> fontFile;
        Resources::LoadBinaryFile(fontFile, fontPath);

        FT_Face face;
        if (FT_New_Memory_Face(ft, reinterpret_cast<const FT_Byte*>(fontFile.data()), fontFile.size(), 0, &face))
        {
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

            Character character =
            {
                UINT32_MAX,
                { face->glyph->bitmap.width, face->glyph->bitmap.rows },
                { face->glyph->bitmap_left, face->glyph->bitmap_top },
                static_cast<unsigned int>(face->glyph->advance.x)
            };

            // 只生成有实际图像的字符纹理
            if (face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0)
                character.TextureID = RenderAPI::GetInstance()->GenerateTextTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

            Characters.insert(pair<char, Character>(c, character));
        }

        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
}