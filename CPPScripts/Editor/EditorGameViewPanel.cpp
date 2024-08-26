#include "EditorGameViewPanel.h"
#include "ImGuiTextureManager.h"
#include "EditorGUIManager.h"
#include "../FBOManager.h"
#include "../RenderAPI.h"

namespace ZXEngine
{
	void EditorGameViewPanel::DrawPanel()
	{
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::hierarchyWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::gameViewWidth, (float)ProjectSetting::gameViewHeight));

		if (ImGui::Begin("Game", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			if (mFirstDraw)
			{
				ImGuiStyle& style = ImGui::GetStyle();
				auto pGUIManager = EditorGUIManager::GetInstance();
				pGUIManager->mViewBorderSize.x = style.WindowPadding.x;
				pGUIManager->mViewBorderSize.y = style.WindowPadding.y;
				pGUIManager->mHeaderSize = style.FramePadding.y * 2 + ImGui::GetTextLineHeight();

				ProjectSetting::SetWindowSize(ProjectSetting::srcWidth, ProjectSetting::srcHeight);
				mFirstDraw = false;
			}

			auto ImTextureMgr = ImGuiTextureManager::GetInstance();
			auto buffer = FBOManager::GetInstance()->GetFBO("GameView");
			uint32_t id = RenderAPI::GetInstance()->GetRenderBufferTexture(buffer->ColorBuffer);
			if (!ImTextureMgr->CheckExistenceByEngineID(id))
				ImTextureMgr->CreateFromEngineID(id);

#ifdef ZX_API_OPENGL
			ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(static_cast<float>(buffer->width), static_cast<float>(buffer->height)), ImVec2(0, 1), ImVec2(1, 0));
#else
			ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(static_cast<float>(buffer->width), static_cast<float>(buffer->height)));
#endif
		}
		ImGui::End();
	}
}