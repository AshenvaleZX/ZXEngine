#pragma once
#include "EditorEnumStruct.h"

namespace ZXEngine
{
	class GameObject;
	class EditorDataManager
	{
	public:
		static void Create();
		static EditorDataManager* GetInstance();
		static bool isGameStart;
		static bool isGamePause;

	private:
		static EditorDataManager* mInstance;

	public:
		GameObject* selectedGO = nullptr;
		EditorAssetNode* selectedAsset = nullptr;
		void* curAssetInfo = nullptr;

		LogInfo* logHead = nullptr;
		LogInfo* logTail = nullptr;
		int logSize = 0;
		int messageSize = 0;
		int warningSize = 0;
		int errorSize = 0;
		const int maxLogSize = 128;

		EditorDataManager() {};
		~EditorDataManager() {};

		void AddLog(LogType type, string msg);
		void SetSelectedGO(GameObject* go);
		void SetSelectedAsset(EditorAssetNode* asset);

	private:
		long long lastClickTime = 0;

		string GetTextFilePreview(string path);
	};
}