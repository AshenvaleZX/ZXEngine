#include "EditorDialogBoxManager.h"
#include "../External/ImGui/imgui.h"

namespace ZXEngine
{
	EditorDialogBoxManager* EditorDialogBoxManager::mInstance = nullptr;

	void EditorDialogBoxManager::Create()
	{
		mInstance = new EditorDialogBoxManager();
	}

	EditorDialogBoxManager* EditorDialogBoxManager::GetInstance()
	{
		return mInstance;
	}

	void EditorDialogBoxManager::Draw()
	{
		if (mMessageList.size() > 0)
			DrawMessageBox();
	}

	void EditorDialogBoxManager::PopMessage(const string& title, const string& content)
	{
		mMessageList.push_back({ title, content });
	}

	void EditorDialogBoxManager::DrawMessageBox()
	{
		const MessageData& msg = mMessageList.front();

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		// 窗口居中
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		// 顶置窗口
		ImGui::SetNextWindowFocus();

		// 标题(居中)
		style.WindowTitleAlign.x = 0.5f;
		ImGui::Begin(msg.title.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		style.WindowTitleAlign.x = 0.0f;

		// 内容
		string str = "\n" + msg.content + "\n\n";
		ImGui::Text(str.c_str());

		// 按钮(居中)
		float size = ImGui::CalcTextSize("OK").x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;
		float off = (avail - size) * 0.5f; // 0.5f, 即居中
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::Button("OK"))
			mMessageList.pop_front();

		ImGui::End();
	}
}