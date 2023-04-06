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
	/// 标准RenderAPI接口
	/// </summary>
	public:
		RenderAPIOpenGL();
		~RenderAPIOpenGL() {};

		virtual void BeginFrame();
		virtual void EndFrame();

		// 渲染状态设置
		virtual void SetRenderState(RenderStateSetting* state);
		virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);

		// FrameBuffer相关
		virtual void SwitchFrameBuffer(uint32_t id);
		virtual void ClearFrameBuffer();
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0);

		// 资源加载相关
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual void DeleteTexture(unsigned int id);
		virtual unsigned int LoadCubeMap(const vector<string>& faces);
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

		// Mesh设置
		virtual void DeleteMesh(unsigned int VAO);
		virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
		virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void GenerateParticleMesh(unsigned int& VAO);

		// Shader设置
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderScalar(Material* material, const string& name, int value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, float value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool isBuffer = false);
		virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool isBuffer = false);

	/// <summary>
	/// 实现标准RenderAPI接口的内部接口与变量
	/// </summary>
	private:
		bool stateDirty = false;
		uint32_t curFBOID = 0;
		uint32_t curShaderID = 0;
		uint32_t curMaterialDataID = 0;
		RenderStateSetting* targetState = nullptr;
		RenderStateSetting* curRealState = nullptr;

		vector<OpenGLVAO*> OpenGLVAOArray;
		vector<OpenGLMaterialData*> OpenGLMaterialDataArray;
		unordered_map<uint32_t, OpenGLMaterialData*> materialDataInShaders;
		unordered_map<uint32_t, ClearInfo> FBOClearInfoMap;

		uint32_t GetNextVAOIndex();
		OpenGLVAO* GetVAOByIndex(uint32_t idx);
		uint32_t GetNextMaterialDataIndex();
		OpenGLMaterialData* GetMaterialDataByIndex(uint32_t idx);

		void CheckError();
		void CheckCompileErrors(unsigned int shader, std::string type);
		void UpdateRenderState();
		void UpdateMaterialData();

		void ClearColorBuffer(const Vector4& color);
		void ClearDepthBuffer(float depth);
		void ClearStencilBuffer(int stencil);

		void RealSetShaderScalar(const string& name, bool value);
		void RealSetShaderScalar(const string& name, int value);
		void RealSetShaderScalar(const string& name, float value);
		void RealSetShaderVector(const string& name, const Vector2& value);
		void RealSetShaderVector(const string& name, const Vector3& value);
		void RealSetShaderVector(const string& name, const Vector4& value);
		void RealSetShaderMatrix(const string& name, const Matrix3& value);
		void RealSetShaderMatrix(const string& name, const Matrix4& value);
		void RealSetShaderTexture(const string& name, uint32_t ID, uint32_t idx);
		void RealSetShaderCubeMap(const string& name, uint32_t ID, uint32_t idx);
	};
}