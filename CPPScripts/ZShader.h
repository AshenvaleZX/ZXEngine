#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class Material;
	class RenderEngineProperties;
	class Shader
	{
		friend class Material;
	private:
		static vector<ShaderReference*> loadedShaders;

	public:
		string name;

		Shader(const string& path, FrameBufferType type);
		~Shader();

		unsigned int GetID();
		LightType GetLightType();
		ShadowType GetShadowType();
		int GetRenderQueue();
		void Use();
		void SetEngineProperties();
		void SetMaterialProperties(Material* material);
		void SetBool(string name, bool value);
		void SetInt(string name, int value);
		void SetFloat(string name, float value);
		void SetVec2(string name, Vector2 value);
		void SetVec2(string name, Vector2 value, uint32_t idx);
		void SetVec3(string name, Vector3 value);
		void SetVec3(string name, Vector3 value, uint32_t idx);
		void SetVec4(string name, Vector4 value);
		void SetVec4(string name, Vector4 value, uint32_t idx);
		void SetMat3(string name, Matrix3 value);
		void SetMat3(string name, Matrix3 value, uint32_t idx);
		void SetMat4(string name, Matrix4 value);
		void SetMat4(string name, Matrix4 value, uint32_t idx);
		void SetTexture(string name, uint32_t ID, uint32_t idx, bool isBuffer = false);
		void SetCubeMap(string name, uint32_t ID, uint32_t idx, bool isBuffer = false);

	private:
		uint16_t textureIdx = 0;
		int renderQueue;
		ShaderReference* reference = nullptr;
		RenderEngineProperties* engineProperties = nullptr;

		void SetEngineProperty(const string& name, ShaderPropertyType type);
		void SetMaterialProperty(const string& name, ShaderPropertyType type);
	};
}