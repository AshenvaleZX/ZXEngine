#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class TextRenderer
	{
	public:
		string text;
		vec2 pos;
		float scale;
		vec3 color;

		TextRenderer();
		~TextRenderer() {};

		void Render();

	private:
		// Vertex Buffer Objects
		unsigned int VBO;
		// Vertex Arrays Objects
		unsigned int VAO;
	};
}