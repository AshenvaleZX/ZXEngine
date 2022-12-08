#pragma once
#include "pubh.h"
#include "FrameBufferObject.h"

#define ScreenBuffer "Screen"

namespace ZXEngine
{
	class FBOManager
	{
	public:
		static void Create();
		static FBOManager* GetInstance();
	private:
		static FBOManager* mInstance;

	public:
		FBOManager() {};
		~FBOManager() {};

		void SwitchFBO(string name);
		void CreateFBO(string name, FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
		FrameBufferObject* GetFBO(string name);

	private:
		map<string, FrameBufferObject*> allFBO;
	};
}