#pragma once
#include "../pubh.h"
#include "EditorEnumStruct.h"

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
		void Reset(float size);
		void UpdateModelScale(float delta);
		void UpdateModelRotation(float xOffset, float yOffset);

	private:
		float yaw = 0.0f;
		float pitch = 0.0f;
		const float rotSensitivity = 0.01f;
		float scale = 1.0f;
		float minScale = 0.1f;
		float maxScale = 10.0f;
		const float standardSize = 6.0f;
		float scaleSensitivity = 0.05f;
		const float standardScaleSensitivity = 0.05f;
		const unsigned int previewSize = 512;
		GameObject* cameraGO;
		Mesh* previewQuad;
		Shader* previewQuadShader;
		Shader* previewModelShader;
		MeshRenderer* renderer;

		void RenderMaterialPreview(AssetMaterialInfo* info);
		void RenderModelPreview(AssetModelInfo* info);
		void RenderToQuad();
		Matrix4 GetModelMatrix();
		void InitPreviewQuad();
	};
}