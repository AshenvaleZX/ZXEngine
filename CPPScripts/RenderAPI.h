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
		virtual void SwitchFrameBuffer(unsigned int id) = 0;
		virtual void ClearFrameBuffer(const ClearInfo& clearInfo) = 0;
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0) = 0;
		virtual void DeleteBuffer(unsigned int id) = 0;

		// 资源加载相关
		virtual unsigned int LoadTexture(const char* path, int& width, int& height) = 0;
		virtual void DeleteTexture(unsigned int id) = 0;
		virtual unsigned int LoadCubeMap(vector<string> faces) = 0;
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data) = 0;
		virtual ShaderReference* LoadAndCompileShader(const char* path) = 0;
		virtual void SetUpMaterial(ShaderReference* shaderReference, const map<string, uint32_t>& textures) = 0;
		virtual void DeleteShaderProgram(unsigned int id) = 0;
		virtual unsigned int GenerateParticleMesh() = 0;

		// DrawCall
		virtual void Draw() = 0;
		virtual void Draw(unsigned int VAO, unsigned int size, DrawType type) = 0;

		// Mesh设置
		virtual void SetMesh(unsigned int VAO, unsigned int size) = 0;
		virtual void DeleteMesh(unsigned int VAO) = 0;
		virtual void SetUpStaticMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices) = 0;
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int vertexSize, unsigned int indexSize) = 0;
		virtual void UpdateDynamicMesh(unsigned int VAO, unsigned int VBO, unsigned int EBO, vector<Vertex> vertices, vector<unsigned int> indices) = 0;

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