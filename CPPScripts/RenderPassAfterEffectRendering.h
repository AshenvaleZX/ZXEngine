#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

#define ExtractBrightArea "ExtractBrightArea"
#define GaussianBlur "GaussianBlur"
#define BloomBlend "BloomBlend"
#define CopyTexture "CopyTexture"

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

		void CreateCommand(string name);
		uint32_t GetCommand(const string& name);
		void CreateMaterial(string name, string path, FrameBufferType type, bool isBuiltIn = false);
		Material* GetMaterial(string name);

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
		// �򵥵�ͼ����
		void InitCopy(bool isFinal = false);
		string BlitCopy(string targetFBO, string sourceFBO, bool isFinal = false);
	};
}