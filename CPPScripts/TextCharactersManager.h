#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct Character {
		unsigned int TextureID;	// ID handle of the glyph texture
		ivec2 Size;				// Size of glyph
		ivec2 Bearing;			// Offset from baseline to left/top of glyph
		unsigned int Advance;	// Horizontal offset to advance to next glyph
	};

	class Shader;
	class TextCharactersManager
	{
	public:
		static void Create();
		static TextCharactersManager* GetInstance();

		TextCharactersManager();
		~TextCharactersManager() {};

		Shader* textShader = nullptr;
		map<char, Character> Characters;

	private:
		static TextCharactersManager* mInstance;
	};
}