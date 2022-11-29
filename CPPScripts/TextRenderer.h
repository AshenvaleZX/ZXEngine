#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
	// 固定的字形Mesh顶点采样纹理UV
	const vec2 GlyphCoords[4] = { vec2(0, 1), vec2(0, 0), vec2(1, 1), vec2(1, 0) };
	// 固定的字形Mesh三角形顶点数组
	const vector<unsigned int> GlyphIndices =
	{
		1, 0, 2,
		1, 2, 3,
	};

	class TextRenderer : public Component
	{
	public:
		// 当前渲染的文本
		string text;
		// 字符颜色
		vec4 color;

		TextRenderer();
		~TextRenderer() {};

		void Render();
	};
}