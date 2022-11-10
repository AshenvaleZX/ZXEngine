#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderPass;
	class RenderPassManager
	{
	public:
		RenderPassManager();
		~RenderPassManager() {};

		static void Create();
		static RenderPassManager* GetInstance();

		vector<RenderPass*> passes;

	private:
		static RenderPassManager* mInstance;
	
	};
}