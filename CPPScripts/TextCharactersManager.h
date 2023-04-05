#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	struct Character {
		uint32_t TextureID;	    // ×ÖÐÎÎÆÀíID
		unsigned int Size[2];	// Size of glyph
		int Bearing[2];			// Offset from baseline to left/top of glyph
		unsigned int Advance;	// Horizontal offset to advance to next glyph
	};

	class Shader;
	class DynamicMesh;
	class TextCharactersManager
	{
	public:
		static void Create();
		static TextCharactersManager* GetInstance();

	private:
		static TextCharactersManager* mInstance;

	public:
		Shader* textShader = nullptr;
		map<char, Character> Characters;

		TextCharactersManager();
		~TextCharactersManager() {};

	private:
		void LoadCharacters();
	};
}