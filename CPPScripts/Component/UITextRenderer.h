#pragma once
#include "../pubh.h"
#include "Component.h"

namespace ZXEngine
{
	enum class TextHorizonAlignment
	{
		Left,
		Center,
		Right
	};

	enum class TextVerticalAlignment
	{
		Top,
		Center,
		Bottom
	};

	// �̶�������Mesh�����������UV
	const Vector2 GlyphCoords[4] = { Vector2(0, 1), Vector2(0, 0), Vector2(1, 1), Vector2(1, 0) };
	// �̶�������Mesh�����ζ�������
	const vector<unsigned int> GlyphIndices =
	{
		1, 0, 2,
		1, 2, 3,
	};

	class Material;
	class DynamicMesh;
	class UITextRenderer : public Component
	{
		friend class EditorInspectorPanel;
	public:
		static ComponentType GetType();

	public:
		Vector4 color;
		float size = 1.0f;
		float mTextWidth = 0.0f;
		float mTextHeight = 0.0f;

		UITextRenderer();
		~UITextRenderer();

		virtual ComponentType GetInsType();

		void Render(const Matrix4& matVP);
		void SetContent(const string& text);
		void SetHorizonAlignment(TextHorizonAlignment align);
		TextHorizonAlignment GetHorizonAlignment() const;
		void SetVerticalAlignment(TextVerticalAlignment align);
		TextVerticalAlignment GetVerticalAlignment() const;
		inline bool IsScreenSpace() const { return isScreenSpace; }

		void OnWindowResize(const string& args);

	private:
		bool isScreenSpace = true;
		uint32_t mWindowResizeCallbackKey = 0;

		// ��ǰ��Ⱦ���ı�
		string text = "";
		vector<Material*> textMaterials;
		vector<DynamicMesh*> textMeshes;
		// ʵ�ʷ�������Ⱦ��Դ���ַ�����(�������ո����ʵ��ͼ���ַ�)
		size_t length = 0;

		TextHorizonAlignment hAlign = TextHorizonAlignment::Center;
		TextVerticalAlignment vAlign = TextVerticalAlignment::Center;
		
		// �ı��Ƿ����˱仯
		bool dirty = false;

		void GenerateRenderData();
	};
}