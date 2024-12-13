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
		bool isGameView = true;

		LogInfo* logHead = nullptr;
		LogInfo* logTail = nullptr;
		int logSize = 0;
		int messageSize = 0;
		int warningSize = 0;
		int errorSize = 0;
		const int maxLogSize = 128;

		GameObject* mTransPosWidget = nullptr;
		GameObject* mTransRotWidget = nullptr;
		GameObject* mTransScaleWidget = nullptr;

		void InitWidgets();
		void AddLog(LogType type, string msg);
		void SetSelectedGO(GameObject* go);
		void SetSelectedAsset(EditorAssetNode* asset);

	private:
		std::mutex logMutex;
		long long lastClickTime = 0;

		EditorDataManager() = default;
		~EditorDataManager();

		void DeleteCurAssetInfo();
		string GetTextFilePreview(string path);
	};
}