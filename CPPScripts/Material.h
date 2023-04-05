#pragma once
#include "pubh.h"
#include "Resources.h"

namespace ZXEngine
{
	class Shader;
	class MaterialData;
	class Material
	{
	public:
		string name = "";
		string path = "";
		Shader* shader = nullptr;
		MaterialData* data = nullptr;

		Material(MaterialStruct* matStruct);
		Material(Shader* shader);
		~Material();

		void Use();
		int GetRenderQueue();

		void SetEngineProperties();
		void SetMaterialProperties();
		void SetScalar(string name, int value, bool allBuffer = false);
		void SetScalar(string name, bool value, bool allBuffer = false);
		void SetScalar(string name, float value, bool allBuffer = false);
		void SetVector(string name, Vector2 value, bool allBuffer = false);
		void SetVector(string name, Vector2 value, uint32_t idx, bool allBuffer = false);
		void SetVector(string name, Vector3 value, bool allBuffer = false);
		void SetVector(string name, Vector3 value, uint32_t idx, bool allBuffer = false);
		void SetVector(string name, Vector4 value, bool allBuffer = false);
		void SetVector(string name, Vector4 value, uint32_t idx, bool allBuffer = false);
		void SetMatrix(string name, Matrix3 value, bool allBuffer = false);
		void SetMatrix(string name, Matrix3 value, uint32_t idx, bool allBuffer = false);
		void SetMatrix(string name, Matrix4 value, bool allBuffer = false);
		void SetMatrix(string name, Matrix4 value, uint32_t idx, bool allBuffer = false);
		void SetTexture(string name, uint32_t ID, uint32_t idx, bool isBuffer = false);
		void SetCubeMap(string name, uint32_t ID, uint32_t idx, bool isBuffer = false);

	private:
		// 这个材质引用的Shader是还有其他地方引用，会影响材质销毁流程
		bool isShareShader;
		uint32_t textureIdx = 0;

		void SetEngineProperty(const string& name, ShaderPropertyType type);
		void SetMaterialProperty(const string& name, ShaderPropertyType type);
	};
}