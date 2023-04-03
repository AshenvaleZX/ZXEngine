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
		~Shader();

		unsigned int GetID();
		LightType GetLightType();
		ShadowType GetShadowType();
		int GetRenderQueue();
		void Use();

	private:
		int renderQueue;
	};
}