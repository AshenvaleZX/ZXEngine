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

		Material() {};
		Material(MaterialStruct* matStruct);
		Material(Shader* shader);
		~Material();

		void Use();
		int GetRenderQueue();

		void SetEngineProperties();
		void SetMaterialProperties();
		void SetScalar(string name, bool value);
		void SetScalar(string name, int value);
		void SetScalar(string name, float value);
		void SetVector(string name, Vector2 value);
		void SetVector(string name, Vector2 value, uint32_t idx);
		void SetVector(string name, Vector3 value);
		void SetVector(string name, Vector3 value, uint32_t idx);
		void SetVector(string name, Vector4 value);
		void SetVector(string name, Vector4 value, uint32_t idx);
		void SetMatrix(string name, Matrix3 value);
		void SetMatrix(string name, Matrix3 value, uint32_t idx);
		void SetMatrix(string name, Matrix4 value);
		void SetMatrix(string name, Matrix4 value, uint32_t idx);
		void SetTexture(string name, uint32_t ID, uint32_t idx, bool isBuffer = false);
		void SetCubeMap(string name, uint32_t ID, uint32_t idx, bool isBuffer = false);

	private:
		uint32_t textureIdx = 0;

		void SetEngineProperty(const string& name, ShaderPropertyType type);
		void SetMaterialProperty(const string& name, ShaderPropertyType type);
	};
}