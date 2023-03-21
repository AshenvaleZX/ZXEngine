#pragma once
#include "pubh.h"
#include "PublicStruct.h"
#include "FrameBufferObject.h"

namespace ZXEngine
{
	class RenderStateSetting;
	class RenderAPI
	{
	public:
		static void Creat();
		static RenderAPI* GetInstance();
	private:
		static RenderAPI* mInstance;

	public:
		RenderAPI() {};
		~RenderAPI() {};

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		// 渲染状态设置
		virtual void SetRenderState(RenderStateSetting* state) = 0;
		virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0) = 0;

		// FrameBuffer相关
		virtual void SwitchFrameBuffer(uint32_t id) = 0;
		virtual void ClearFrameBuffer(const ClearInfo& clearInfo) = 0;
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0) = 0;

		// 资源加载相关
		virtual unsigned int LoadTexture(const char* path, int& width, int& height) = 0;
		virtual void DeleteTexture(unsigned int id) = 0;
		virtual unsigned int LoadCubeMap(vector<string> faces) = 0;
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data) = 0;
		virtual ShaderReference* LoadAndSetUpShader(const char* path) = 0;
		virtual void SetUpMaterial(ShaderReference* shaderReference, const map<string, uint32_t>& textures) = 0;
		virtual void DeleteShader(unsigned int id) = 0;

		// Draw
		virtual uint32_t AllocateDrawCommand() = 0;
		virtual void Draw(uint32_t VAO) = 0;
		virtual void GenerateDrawCommand(uint32_t id) = 0;

		// Mesh设置
		virtual void DeleteMesh(unsigned int VAO) = 0;
		virtual void SetUpStaticMesh(unsigned int& VAO, vector<Vertex> vertices, vector<unsigned int> indices) = 0;
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize) = 0;
		virtual void UpdateDynamicMesh(unsigned int VAO, vector<Vertex> vertices, vector<unsigned int> indices) = 0;
		virtual void GenerateParticleMesh(unsigned int& VAO) = 0;

		// Shader设置
		virtual void UseShader(unsigned int ID) = 0;
		virtual void SetShaderBool(ShaderReference* reference, const string& name, bool value) = 0;
		virtual void SetShaderInt(ShaderReference* reference, const string& name, int value) = 0;
		virtual void SetShaderFloat(ShaderReference* reference, const string& name, float value) = 0;
		virtual void SetShaderVector(ShaderReference* reference, const string& name, const Vector2& value) = 0;
		virtual void SetShaderVector(ShaderReference* reference, const string& name, float x, float y) = 0;
		virtual void SetShaderVector(ShaderReference* reference, const string& name, const Vector3& value) = 0;
		virtual void SetShaderVector(ShaderReference* reference, const string& name, float x, float y, float z) = 0;
		virtual void SetShaderVector(ShaderReference* reference, const string& name, const Vector4& value) = 0;
		virtual void SetShaderVector(ShaderReference* reference, const string& name, float x, float y, float z, float w) = 0;
		virtual void SetShaderMatrix(ShaderReference* reference, const string& name, const Matrix3& value) = 0;
		virtual void SetShaderMatrix(ShaderReference* reference, const string& name, const Matrix4& value) = 0;
		virtual void SetShaderTexture(ShaderReference* reference, const string& name, unsigned int textureID, unsigned int idx) = 0;
		virtual void SetShaderCubeMap(ShaderReference* reference, const string& name, unsigned int textureID, unsigned int idx) = 0;
	};
}