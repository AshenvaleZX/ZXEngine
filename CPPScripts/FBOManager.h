#pragma once
#include "pubh.h"
#include "PublicStruct.h"
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

		void SwitchFBO(const string& name);
		void CreateFBO(const string& name, FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
		void CreateFBO(const string& name, FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0);
		void RecreateAllFollowWindowFBO();
		FrameBufferObject* GetFBO(const string& name);

	private:
		map<string, FrameBufferObject*> allFBO;
	};
}