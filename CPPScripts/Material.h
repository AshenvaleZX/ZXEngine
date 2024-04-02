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
		uint32_t hitGroupIdx = 0;
		Shader* shader = nullptr;
		MaterialData* data = nullptr;
		MaterialType type = MaterialType::Forward;

		Material(MaterialStruct* matStruct);
		// ע��: �˹��캯�������ڹ�դ����Ⱦ���߲���
		Material(Shader* shader);
		~Material();

		void Use();
		int GetRenderQueue() const;

		void SetEngineProperties();
		void SetMaterialProperties();
		void SetScalar(const string& name, bool value, bool allBuffer = false);
		void SetScalar(const string& name, float value, bool allBuffer = false);
		void SetScalar(const string& name, int32_t value, bool allBuffer = false);
		void SetScalar(const string& name, uint32_t value, bool allBuffer = false);
		void SetVector(const string& name, const Vector2& value, bool allBuffer = false);
		void SetVector(const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false);
		void SetVector(const string& name, const Vector3& value, bool allBuffer = false);
		void SetVector(const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false);
		void SetVector(const string& name, const Vector4& value, bool allBuffer = false);
		void SetVector(const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false);
		void SetVector(const string& name, const Vector4* value, uint32_t count, bool allBuffer = false);
		void SetMatrix(const string& name, const Matrix3& value, bool allBuffer = false);
		void SetMatrix(const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false);
		void SetMatrix(const string& name, const Matrix4& value, bool allBuffer = false);
		void SetMatrix(const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false);
		void SetMatrix(const string& name, const Matrix4* value, uint32_t count, bool allBuffer = false);
		void SetTexture(const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);
		void SetCubeMap(const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);

	private:
		// ����������õ�Shader�ǻ��������ط����ã���Ӱ�������������
		bool isShareShader;
		uint32_t textureIdx = 0;
		int renderQueue = 0;

		void SetEngineProperty(const string& name, ShaderPropertyType type);
		void SetMaterialProperty(const string& name, ShaderPropertyType type);
		void CopyMaterialStructToMaterialData(MaterialStruct* matStruct, MaterialData* data);
	};
}