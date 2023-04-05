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
		// ��ǰ��Ⱦ���ı�
		string text = "";
		vector<Material*> textMaterials;
		vector<DynamicMesh*> textMeshes;
		// ʵ�ʷ�������Ⱦ��Դ���ַ�����(�������ո����ʵ��ͼ���ַ�)
		size_t length = 0;
		// �ı��Ƿ����˱仯
		bool dirty = false;

		void GenerateRenderData();
	};
}