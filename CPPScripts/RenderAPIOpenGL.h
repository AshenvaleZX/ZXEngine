#pragma once
#include "RenderAPI.h"
#include "OpenGLEnumStruct.h"

namespace ZXEngine
{
	class Material;
	class MaterialData;
	class RenderAPIOpenGL : public RenderAPI
	{
	/// <summary>
	/// ��׼RenderAPI�ӿ�
	/// </summary>
	public:
		RenderAPIOpenGL();
		~RenderAPIOpenGL() {};

		virtual void BeginFrame();
		virtual void EndFrame();

		// ��Ⱦ״̬����
		virtual void SetRenderState(RenderStateSetting* state);
		virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);

		// FrameBuffer���
		virtual void SwitchFrameBuffer(uint32_t id);
		virtual void ClearFrameBuffer(const ClearInfo& clearInfo);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);

		// ��Դ�������
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual void DeleteTexture(unsigned int id);
		virtual unsigned int LoadCubeMap(vector<string> faces);
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
		virtual ShaderReference* LoadAndSetUpShader(const char* path, FrameBufferType type);
		virtual uint32_t CreateMaterialData();
		virtual void UseMaterialData(uint32_t ID);
		virtual void SetUpMaterial(ShaderReference* shaderReference, MaterialData* materialData);
		virtual void DeleteShader(uint32_t id);
		virtual void DeleteMaterialData(uint32_t id);

		// Draw
		virtual uint32_t AllocateDrawCommand();
		virtual void Draw(uint32_t VAO);
		virtual void GenerateDrawCommand(uint32_t id);

		// Mesh����
		virtual void DeleteMesh(unsigned int VAO);
		virtual void SetUpStaticMesh(unsigned int& VAO, vector<Vertex> vertices, vector<unsigned int> indices);
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
		virtual void UpdateDynamicMesh(unsigned int VAO, vector<Vertex> vertices, vector<unsigned int> indices);
		virtual void GenerateParticleMesh(unsigned int& VAO);

		// Shader����
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderScalar(Material* material, const string& name, bool value);
		virtual void SetShaderScalar(Material* material, const string& name, int value);
		virtual void SetShaderScalar(Material* material, const string& name, float value);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx);
		virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool isBuffer = false);
		virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool isBuffer = false);

	/// <summary>
	/// ʵ�ֱ�׼RenderAPI�ӿڵ��ڲ��ӿ������
	/// </summary>
	private:
		bool stateDirty = false;
		RenderStateSetting* targetState = nullptr;
		RenderStateSetting* curRealState = nullptr;

		vector<OpenGLVAO*> OpenGLVAOArray;

		uint32_t GetNextVAOIndex();
		OpenGLVAO* GetVAOByIndex(uint32_t idx);

		void CheckError();
		void CheckCompileErrors(unsigned int shader, std::string type);
		void UpdateRenderState();

		void ClearColorBuffer(const Vector4& color);
		void ClearDepthBuffer(float depth);
		void ClearStencilBuffer(int stencil);
	};
}