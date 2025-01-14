#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Light;
	class Camera;
	class Texture;
	class CubeMap;
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
		RenderEngineProperties();
		~RenderEngineProperties();

		void SetLightMatrix(const Matrix4& mat);
		void SetLightProperties(const vector<Light*>& lights);
		void SetCameraProperties(Camera* camera);
		void SetRendererProperties(MeshRenderer* renderer);
		void SetDepthMap(uint32_t id);
		void SetEmptyShadowMap();
		void SetShadowMap(uint32_t id, bool isBuffer = true);
		void SetEmptyShadowCubeMap();
		void SetShadowCubeMap(uint32_t id, bool isBuffer = true);

	private:
		Matrix4 matM;
		Matrix4 matV;
		Matrix4 matP;
		Matrix4 matM_Inv;
		Matrix4 matV_Inv;
		Matrix4 matP_Inv;
		Vector3 camPos;

		Matrix4 lightMat;
		Vector3 lightPos;
		Vector3 lightDir;
		Vector3 lightColor;
		float lightIntensity = 1.0f;

		uint32_t depthMap = 0;

		uint32_t shadowMap = 0;
		Texture* emptyShadowMap;
		// true: ShadowMap是一个Render Buffer, false: ShadowMap是一个纹理
		bool isShadowMapBuffer = true;

		uint32_t shadowCubeMap = 0;
		CubeMap* emptyShadowCubeMap;
		// true: ShadowCubeMap是一个Render Buffer, false: ShadowCubeMap是一个纹理
		bool isShadowCubeMapBuffer = true;
	};
}