#pragma once
#include "../pubh.h"
#include "EditorDataManager.h"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class GameObject;
	class MeshRenderer;
	class EditorAssetPreviewer
	{
	public:
		EditorAssetPreviewer();
		~EditorAssetPreviewer();

		bool Check();
		void Draw();

	private:
		const unsigned int previewSize = 512;
		GameObject* cameraGO;
		Mesh* previewQuad;
		Shader* previewQuadShader;
		Shader* previewModelShader;
		MeshRenderer* renderer;

		void RenderMaterialPreview(AssetMaterialInfo* info);
		void RenderModelPreview(AssetModelInfo* info);
		void RenderToQuad();
		void InitPreviewQuad();
	};
}