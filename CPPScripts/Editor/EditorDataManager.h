#pragma once
#include "EditorEnumStruct.h"

namespace ZXEngine
{
	class GameObject;
	class BoxCollider;
	class Circle2DCollider;

	using WidgetColliderMap = unordered_map<AxisType, BoxCollider*>;
	using RotWidgetColliderMap = unordered_map<AxisType, Circle2DCollider*>;
	using WidgetOrientationMap = unordered_map<AxisType, pair<GameObject*, GameObject*>>;
	using RotWidgetTurnplateMap = unordered_map<AxisType, GameObject*>;

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

		// -------------------------------- Console Log --------------------------------
		LogInfo* logHead = nullptr;
		LogInfo* logTail = nullptr;
		int logSize = 0;
		int messageSize = 0;
		int warningSize = 0;
		int errorSize = 0;
		const int maxLogSize = 128;

		// -------------------------------- Transform Widget --------------------------------
		TransformType mCurTransType = TransformType::Position;

		GameObject* mTransPosWidget = nullptr;
		GameObject* mTransRotWidget = nullptr;
		GameObject* mTransScaleWidget = nullptr;

		WidgetColliderMap mTransPosWidgetColliders;
		WidgetOrientationMap mTransPosWidgetOrientations;
		WidgetColliderMap mTransScaleWidgetColliders;
		WidgetOrientationMap mTransScaleWidgetOrientations;

		RotWidgetColliderMap mTransRotWidgetColliders;
		RotWidgetTurnplateMap mTransRotWidgetTurnplates;

		void InitWidgets();
		void AddLog(LogType type, string msg);
		void SetSelectedGO(GameObject* go);
		void SetSelectedAsset(EditorAssetNode* asset);
		GameObject* GetTransWidget() const;

	private:
		std::mutex logMutex;
		long long mLastGOClick = 0;
		long long mLastAssetClick = 0;
		const long long mDoubleClickInterval = 300'000LL; // 300ms

		EditorDataManager() = default;
		~EditorDataManager();

		void DeleteCurAssetInfo();
		string GetTextFilePreview(string path);
		void RecordWidgetAxisInfo(WidgetColliderMap& colliders, WidgetOrientationMap& orientations, GameObject* widget);
		void RecordRotWidgetAxisInfo(RotWidgetColliderMap& colliders, RotWidgetTurnplateMap& turnplates, GameObject* widget);
	};
}