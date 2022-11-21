#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class RenderAPIOpenGL : public RenderAPI
	{
	public:
		RenderAPIOpenGL() {};
		~RenderAPIOpenGL() {};

		// ��Ⱦ״̬����
		virtual void InitRenderSetting();
		virtual void EnableDepthTest(bool enable);
		virtual void EnableDepthWrite(bool enable);
		virtual void SwitchFrameBuffer(unsigned int id);
		virtual void ClearFrameBuffer();
		virtual void CheckError();

		// ��Դ�������
		virtual unsigned int LoadTexture(const char* path);
		virtual unsigned int LoadCubeMap(vector<string> faces);
		virtual ShaderInfo LoadAndCompileShader(const char* path);
		virtual void CheckCompileErrors(unsigned int shader, std::string type);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);

		// DrawCall
		virtual void Draw();

		// Mesh����
		virtual void SetMesh(unsigned int VAO, unsigned int size);
		virtual void SetUpMesh(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, vector<Vertex> vertices, vector<unsigned int> indices);

		// Shader����
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderBool(unsigned int ID, string name, bool value);
		virtual void SetShaderInt(unsigned int ID, string name, int value);
		virtual void SetShaderFloat(unsigned int ID, string name, float value);
		virtual void SetShaderVec2(unsigned int ID, string name, vec2 value);
		virtual void SetShaderVec2(unsigned int ID, string name, float x, float y);
		virtual void SetShaderVec3(unsigned int ID, string name, vec3 value);
		virtual void SetShaderVec3(unsigned int ID, string name, float x, float y, float z);
		virtual void SetShaderVec4(unsigned int ID, string name, vec4 value);
		virtual void SetShaderVec4(unsigned int ID, string name, float x, float y, float z, float w);
		virtual void SetShaderMat2(unsigned int ID, string name, mat2 value);
		virtual void SetShaderMat3(unsigned int ID, string name, mat3 value);
		virtual void SetShaderMat4(unsigned int ID, string name, mat4 value);
		virtual void SetShaderTexture(unsigned int ID, string name, unsigned int textureID, unsigned int idx);
		virtual void SetShaderCubeMap(unsigned int ID, string name, unsigned int textureID, unsigned int idx);

	private:
		unsigned int VAO;
		// ��VAO��Ӧ��ͼԪ����
		unsigned int primitiveSize;
	};
}