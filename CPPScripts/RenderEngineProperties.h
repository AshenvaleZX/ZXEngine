#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Light;
	class Camera;
	class MeshRenderer;
	class RenderEngineProperties
	{
		friend class Material;
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
		void SetShadowMap(uint32_t id, bool isBuffer = true);
		void SetShadowCubeMap(uint32_t id, bool isBuffer = true);

	private:
		Matrix4 matM;
		Matrix4 matV;
		Matrix4 matP;
		Matrix4 matM_Inv;
		Vector3 camPos;

		Vector3 lightPos;
		Vector3 lightDir;
		Vector3 lightColor;
		float lightIntensity = 1.0f;

		uint32_t shadowMap = 0;
		bool isShadowMapBuffer = true;
		uint32_t shadowCubeMap = 0;
		bool isShadowCubeMapBuffer = true;
	};
}