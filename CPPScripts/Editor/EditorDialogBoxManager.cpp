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

		// ���ھ���
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		// ���ô���
		ImGui::SetNextWindowFocus();

		// ����(����)
		style.WindowTitleAlign.x = 0.5f;
		ImGui::Begin(msg.title.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		style.WindowTitleAlign.x = 0.0f;

		// ����
		string str = "\n" + msg.content + "\n\n";
		ImGui::Text(str.c_str());

		// ��ť(����)
		float size = ImGui::CalcTextSize("OK").x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;
		float off = (avail - size) * 0.5f; // 0.5f, ������
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::Button("OK"))
			mMessageList.pop_front();

		ImGui::End();
	}
}