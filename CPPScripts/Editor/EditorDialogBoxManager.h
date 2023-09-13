#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	struct MessageData
	{
		string title;
		string content;
	};

	class EditorDialogBoxManager
	{
	public:
		static void Create();
		static EditorDialogBoxManager* GetInstance();

	private:
		static EditorDialogBoxManager* mInstance;


	public:
		void Draw();

		void PopMessage(const string& title, const string& content);

	private:
		deque<MessageData> mMessageList;

		void DrawMessageBox();
	};
}