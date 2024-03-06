#pragma once
#include "PublicStruct.h"

namespace ZXEngine
{
	class Shader
	{
	private:
		static vector<ShaderReference*> loadedShaders;

	public:
		string name;
		ShaderReference* reference = nullptr;

		Shader(const string& path, FrameBufferType type);
		Shader(const string& path, const string& shaderCode, FrameBufferType type);
		~Shader();

		void Use();
		unsigned int GetID();
		LightType GetLightType();
		ShadowType GetShadowType();
	};
}