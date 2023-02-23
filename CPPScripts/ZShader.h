#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class Shader
	{
	private:
		static vector<ShaderReference*> loadedShaders;

	public:
		string name;

		Shader(const string& path);
		~Shader();

		unsigned int GetID();
		LightType GetLightType();
		ShadowType GetShadowType();
		int GetRenderQueue();
		void Use();
		void SetBool(string name, bool value);
		void SetInt(string name, int value);
		void SetFloat(string name, float value);
		void SetVec2(string name, Vector2 value);
		void SetVec2(string name, float x, float y);
		void SetVec3(string name, Vector3 value);
		void SetVec3(string name, float x, float y, float z);
		void SetVec4(string name, Vector4 value);
		void SetVec4(string name, float x, float y, float z, float w);
		void SetMat3(string name, Matrix3 value);
		void SetMat4(string name, Matrix4 value);
		void SetTexture(string name, unsigned int textureID, unsigned int idx);
		void SetCubeMap(string name, unsigned int textureID, unsigned int idx);

	private:
		int renderQueue;
		ShaderReference* reference = nullptr;
	};
}