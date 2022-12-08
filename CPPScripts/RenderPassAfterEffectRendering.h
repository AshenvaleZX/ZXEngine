#pragma once
#include "RenderPass.h"

#define ExtractBrightArea "ExtractBrightArea"
#define GaussianBlur "GaussianBlur"
#define BloomBlend "BloomBlend"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class Camera;
	class RenderPassAfterEffectRendering : public RenderPass
	{
	public:
		RenderPassAfterEffectRendering();
		~RenderPassAfterEffectRendering() {};

		virtual void Render(Camera* camera);

	private:
		Mesh* screenQuad;
		map<string, Shader*> aeShaders;

		void CreateShader(string name, string path);
		Shader* GetShader(string name);
		void InitScreenQuad();

		// 提取画面高亮部分
		void InitExtractBrightArea(bool isFinal = false);
		string BlitExtractBrightArea(string sourceFBO, bool isFinal = false);
		// 高斯模糊
		void InitGaussianBlur(bool isFinal = false);
		string BlitGaussianBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Bloom
		void InitBloomBlend(bool isFinal = false);
		string BlitBloomBlend(string originFBO, string blurFBO, bool isFinal = false);
	};
}