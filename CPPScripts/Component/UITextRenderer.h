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

	// 固定的字形Mesh顶点采样纹理UV
	const Vector2 GlyphCoords[4] = { Vector2(0, 1), Vector2(0, 0), Vector2(1, 1), Vector2(1, 0) };
	// 固定的字形Mesh三角形顶点数组
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

		// 当前渲染的文本
		string text = "";
		vector<Material*> textMaterials;
		vector<DynamicMesh*> textMeshes;
		// 实际分配了渲染资源的字符数量(不包括空格等无实际图像字符)
		size_t length = 0;

		TextHorizonAlignment hAlign = TextHorizonAlignment::Center;
		TextVerticalAlignment vAlign = TextVerticalAlignment::Center;
		
		// 文本是否发生了变化
		bool dirty = false;

		void GenerateRenderData();
	};
}