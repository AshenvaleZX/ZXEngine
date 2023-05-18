#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderPass;
	class RenderPassManager
	{
	public:
		static void Create();
		static RenderPassManager* GetInstance();

	private:
		static RenderPassManager* mInstance;

	public:
		RenderPassManager();
		~RenderPassManager() {};

		vector<RenderPass*> passes;

	private:
		void SetUpRenderPasses();
	};
}