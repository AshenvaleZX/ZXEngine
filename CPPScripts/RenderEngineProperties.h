#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Light;
	class Camera;
	class MeshRenderer;
	class RenderEngineProperties
	{
		friend class Shader;
		friend class EditorAssetPreviewer;
	public:
		static void Create();
		static RenderEngineProperties* GetInstance();

	private:
		static RenderEngineProperties* mInstance;

	public:
		void SetLightProperties(Light* light);
		void SetCameraProperties(Camera* camera);
		void SetRendererProperties(MeshRenderer* renderer);
		void SetShadowMap(unsigned int id);
		void SetShadowCubeMap(unsigned int id);

	private:
		Matrix4 matM;
		Matrix4 matV;
		Matrix4 matP;
		Vector3 camPos;

		Vector3 lightPos;
		Vector3 lightDir;
		Vector3 lightColor;
		float lightIntensity = 1.0f;

		unsigned int shadowMap = 0;
		unsigned int shadowCubeMap = 0;
	};
}