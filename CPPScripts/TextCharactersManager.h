#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	struct Character {
		unsigned int TextureID;	// 字形纹理ID
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

		TextCharactersManager();
		~TextCharactersManager() {};

		map<char, Character> Characters;

		void BeginRender();
		// 设置字符颜色
		void SetColor(Vector3 color);
		// 设置字符的字形(glyph)纹理
		void SetTexture(unsigned int ID);
		// 更新字符Mesh数据
		void UpdateCharacterMesh(vector<Vertex> vertices, vector<unsigned int> indices);
		void DrawCharacter();

		// 渲染一个字符用的
		// Vertex Arrays Objects
		unsigned int VAO;
		// Vertex Buffer Objects
		unsigned int VBO;
		// Element Buffer Objects
		unsigned int EBO;

	private:
		static TextCharactersManager* mInstance;
		Shader* textShader = nullptr;
		DynamicMesh* characterMesh = nullptr;

		void LoadCharacters();
	};
}