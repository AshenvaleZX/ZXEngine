#pragma once
#include "pubh.h"
#include "FrameBufferObject.h"

namespace ZXEngine
{
	class FBOManager
	{
	public:
		FrameBufferObject* mainFBO = nullptr;
		FrameBufferObject* shadowMapFBO = nullptr;
		FrameBufferObject* shadowCubeMapFBO = nullptr;

		FBOManager() {};
		~FBOManager() {};

		static void Create();
		static FBOManager* GetInstance();

	private:
		static FBOManager* mInstance;

	};
}