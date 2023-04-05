#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
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
		float size = 1;

		UITextRenderer();
		~UITextRenderer() {};

		virtual ComponentType GetInsType();

		void Render();
		void SetContent(const string& text);

	private:
		// 当前渲染的文本
		string text = "";
		vector<Material*> textMaterials;
		vector<DynamicMesh*> textMeshes;
		// 实际分配了渲染资源的字符数量(不包括空格等无实际图像字符)
		size_t length = 0;
		// 文本是否发生了变化
		bool dirty = false;

		void GenerateRenderData();
	};
}