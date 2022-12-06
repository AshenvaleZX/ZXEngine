#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
	// �̶�������Mesh�����������UV
	const Vector2 GlyphCoords[4] = { Vector2(0, 1), Vector2(0, 0), Vector2(1, 1), Vector2(1, 0) };
	// �̶�������Mesh�����ζ�������
	const vector<unsigned int> GlyphIndices =
	{
		1, 0, 2,
		1, 2, 3,
	};

	class UITextRenderer : public Component
	{
	public:
		// ��ǰ��Ⱦ���ı�
		string text;
		// �ַ���ɫ
		Vector4 color;

		UITextRenderer();
		~UITextRenderer() {};

		static ComponentType GetType();

		void Render();
	};
}