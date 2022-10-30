#pragma once
#include "RenderAPI.h"
#include "PublicEnum.h"

namespace ZXEngine
{
	class Shader
	{
	public:
		Shader(const char* path);
		~Shader() {};

		unsigned int GetID();
		int GetRenderQueue();
		void Use();
		void SetBool(string name, bool value);
		void SetInt(string name, int value);
		void SetFloat(string name, float value);
		void SetVec2(string name, vec2 value);
		void SetVec2(string name, float x, float y);
		void SetVec3(string name, vec3 value);
		void SetVec3(string name, float x, float y, float z);
		void SetVec4(string name, vec4 value);
		void SetVec4(string name, float x, float y, float z, float w);
		void SetMat2(string name, mat2 value);
		void SetMat3(string name, mat3 value);
		void SetMat4(string name, mat4 value);
		void SetTexture(string name, unsigned int textureID, unsigned int idx);

	private:
		unsigned int ID;
		int renderQueue;
	};
}