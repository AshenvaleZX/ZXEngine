#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

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
		StaticMesh* screenQuad;
		map<string, uint32_t> aeCommands;
		map<string, Material*> aeMaterials;
		RenderStateSetting* renderState;

		void CreateCommand(const string& name);
		uint32_t GetCommand(const string& name);
		void CreateMaterial(const string& name, const string& path, FrameBufferType type);
		Material* GetMaterial(const string& name);

		// 提取画面高亮部分
		void InitExtractBrightArea(bool isFinal = false);
		string BlitExtractBrightArea(const string& sourceFBO, bool isFinal = false);
		// 高斯模糊
		void InitGaussianBlur(bool isFinal = false);
		string BlitGaussianBlur(const string& sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Kawase模糊，性能比高斯好
		void InitKawaseBlur(bool isFinal = false);
		string BlitKawaseBlur(const string& sourceFBO, int blurTimes, float texOffset, bool isFinal = false);
		// Bloom
		void InitBloomBlend(bool isFinal = false);
		string BlitBloomBlend(const string& originFBO, const string& blurFBO, bool isFinal = false);
		// 简单的图像复制
		void InitCopy(bool isFinal = false);
		string BlitCopy(const string& targetFBO, const string& sourceFBO, bool isFinal = false);
	};
}