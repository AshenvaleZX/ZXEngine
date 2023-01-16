#pragma once
#include "RenderPass.h"

#define ExtractBrightArea "ExtractBrightArea"
#define GaussianBlur "GaussianBlur"
#define KawaseBlur "KawaseBlur"
#define BloomBlend "BloomBlend"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class Camera;
	class RenderStateSetting;
	class RenderPassAfterEffectRendering : public RenderPass
	{
	public:
		RenderPassAfterEffectRendering();
		~RenderPassAfterEffectRendering() {};

		virtual void Render(Camera* camera);

	private:
		Mesh* screenQuad;
		map<string, Shader*> aeShaders;
		RenderStateSetting* renderState;

		void CreateShader(string name, string path);
		Shader* GetShader(string name);
		void InitScreenQuad();

		// ��ȡ�����������
		void InitExtractBrightArea(bool isFinal = false);
		string BlitExtractBrightArea(string sourceFBO, bool isFinal = false);
		// ��˹ģ��
		void InitGaussianBlur(bool isFinal = false);
		string BlitGaussianBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Kawaseģ�������ܱȸ�˹��
		void InitKawaseBlur(bool isFinal = false);
		string BlitKawaseBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Bloom
		void InitBloomBlend(bool isFinal = false);
		string BlitBloomBlend(string originFBO, string blurFBO, bool isFinal = false);
	};
}