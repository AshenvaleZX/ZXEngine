#pragma once
#include "../pubh.h"
#include "EditorDataManager.h"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class Camera;
	class MeshRenderer;
	class EditorAssetPreviewer
	{
	public:
		EditorAssetPreviewer();
		~EditorAssetPreviewer();

		bool Check();
		void Draw();

	private:
		Camera* camera;
		Mesh* previewQuad;
		Shader* previewShader;
		MeshRenderer* renderer;

		void RenderMaterialPreview(AssetMaterialInfo* info);
		void RenderModelPreview(AssetModelInfo* info);
		void RenderToQuad();
		void InitPreviewQuad();
	};
}