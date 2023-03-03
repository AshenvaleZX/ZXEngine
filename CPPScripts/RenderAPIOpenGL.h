#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class RenderAPIOpenGL : public RenderAPI
	{
	public:
		RenderAPIOpenGL();
		~RenderAPIOpenGL() {};

		// 渲染状态设置
		virtual void InitRenderSetting();
		virtual void SetRenderState(RenderStateSetting* state);
		virtual void EnableDepthTest(bool enable);
		virtual void EnableDepthWrite(bool enable);
		virtual void SetBlendMode(BlendFactor sfactor, BlendFactor dfactor);
		virtual void SetClearColor(const Vector4& color);
		virtual void EnableFaceCull(bool enable);
		virtual void SetFaceCullMode(FaceCullOption mode);

		// 渲染操作
		virtual void SwitchFrameBuffer(unsigned int id);
		virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);
		virtual void ClearFrameBuffer();
		virtual void ClearColorBuffer();
		virtual void ClearColorBuffer(const Vector4& color);
		virtual void ClearDepthBuffer();
		virtual void ClearDepthBuffer(float depth);
		virtual void ClearStencilBuffer();
		virtual void ClearStencilBuffer(int stencil);
		virtual void CheckError();

		// 资源加载相关
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual void DeleteTexture(unsigned int id);
		virtual unsigned int LoadCubeMap(vector<string> faces);
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
		virtual ShaderReference* LoadAndCompileShader(const char* path);
		virtual void SetUpMaterial(ShaderReference* shaderReference, const map<string, uint32_t>& textures);
		virtual void DeleteShaderProgram(unsigned int id);
		virtual void CheckCompileErrors(unsigned int shader, std::string type);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
		virtual unsigned int GenerateParticleMesh();
		virtual void DeleteBuffer(unsigned int id);

		// DrawCall
		virtual void Draw();
		virtual void Draw(unsigned int VAO, unsigned int size, DrawType type);

		// Mesh设置
		virtual void SetMesh(unsigned int VAO, unsigned int size);
		virtual void DeleteMesh(unsigned int VAO);
		virtual void SetUpStaticMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices);
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int vertexSize, unsigned int indexSize);
		virtual void UpdateDynamicMesh(unsigned int VAO, unsigned int VBO, unsigned int EBO, vector<Vertex> vertices, vector<unsigned int> indices);

		// Shader设置
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderBool(unsigned int ID, string name, bool value);
		virtual void SetShaderInt(unsigned int ID, string name, int value);
		virtual void SetShaderFloat(unsigned int ID, string name, float value);
		virtual void SetShaderVec2(unsigned int ID, string name, const Vector2& value);
		virtual void SetShaderVec2(unsigned int ID, string name, float x, float y);
		virtual void SetShaderVec3(unsigned int ID, string name, const Vector3& value);
		virtual void SetShaderVec3(unsigned int ID, string name, float x, float y, float z);
		virtual void SetShaderVec4(unsigned int ID, string name, const Vector4& value);
		virtual void SetShaderVec4(unsigned int ID, string name, float x, float y, float z, float w);
		virtual void SetShaderMat3(unsigned int ID, string name, const Matrix3& value);
		virtual void SetShaderMat4(unsigned int ID, string name, const Matrix4& value);
		virtual void SetShaderTexture(unsigned int ID, string name, unsigned int textureID, unsigned int idx);
		virtual void SetShaderCubeMap(unsigned int ID, string name, unsigned int textureID, unsigned int idx);

	private:
		virtual void UpdateRenderState();

	private:
		bool stateDirty = false;
		RenderStateSetting* targetState = nullptr;
		RenderStateSetting* curRealState = nullptr;
		unsigned int VAO = 0;
		// 与VAO对应的图元数量
		unsigned int primitiveSize = 0;

		map<BlendFactor, int> BlendMap;
		map<FaceCullOption, int> FaceCullMap;
		void InitGLConstMap();
	};
}