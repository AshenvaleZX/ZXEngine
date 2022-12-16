#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class GameObject;
	class EditorDataManager
	{
	public:
		static void Create();
		static EditorDataManager* GetInstance();

	private:
		static EditorDataManager* mInstance;

	public:
		void* selectedAsset;
		GameObject* selectedGO;

		EditorDataManager();
		~EditorDataManager() {};
	};
}