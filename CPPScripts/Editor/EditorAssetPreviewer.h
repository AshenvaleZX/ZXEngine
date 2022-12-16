#pragma once
#include "../pubh.h"

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

		void RenderToQuad();
		void InitPreviewQuad();
	};
}