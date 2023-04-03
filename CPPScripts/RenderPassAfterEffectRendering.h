#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

#define ExtractBrightArea "ExtractBrightArea"
#define GaussianBlur "GaussianBlur"
#define KawaseBlur "KawaseBlur"
#define BloomBlend "BloomBlend"

namespace ZXEngine
{
	class Material;
	class Camera;
	class StaticMesh;
	class RenderStateSetting;
	class RenderPassAfterEffectRendering : public RenderPass
	{
	public:
		RenderPassAfterEffectRendering();
		~RenderPassAfterEffectRendering() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t drawCommandID = 0;
		StaticMesh* screenQuad;
		map<string, Material*> aeMaterials;
		RenderStateSetting* renderState;
		ClearInfo clearInfo;

		void CreateMaterial(string name, string path, FrameBufferType type);
		Material* GetMaterial(string name);
		void InitScreenQuad();

		// 提取画面高亮部分
		void InitExtractBrightArea(bool isFinal = false);
		string BlitExtractBrightArea(string sourceFBO, bool isFinal = false);
		// 高斯模糊
		void InitGaussianBlur(bool isFinal = false);
		string BlitGaussianBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Kawase模糊，性能比高斯好
		void InitKawaseBlur(bool isFinal = false);
		string BlitKawaseBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Bloom
		void InitBloomBlend(bool isFinal = false);
		string BlitBloomBlend(string originFBO, string blurFBO, bool isFinal = false);
	};
}