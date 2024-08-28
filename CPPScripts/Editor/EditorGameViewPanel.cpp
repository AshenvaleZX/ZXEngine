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

		if (ImGui::Begin("Game", NULL, ImGuiWindowFlags_NoCollapse))
		{
			CheckManualResize();

			if (mFirstDraw)
			{
				ImGuiStyle& style = ImGui::GetStyle();
				auto pGUIManager = EditorGUIManager::GetInstance();
				pGUIManager->mViewBorderSize.x = style.WindowPadding.x;
				pGUIManager->mViewBorderSize.y = style.WindowPadding.y;
				pGUIManager->mHeaderSize = style.FramePadding.y * 2 + ImGui::GetTextLineHeight();

				ProjectSetting::SetWindowSize();
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

	void EditorGameViewPanel::PanelSizeChangeBegin()
	{
		mLastWidth = ProjectSetting::gameViewWidth;
		mLastHeight = ProjectSetting::gameViewHeight;
	}

	void EditorGameViewPanel::PanelSizeChanging(const Vector2& size, EditorPanelEdgeFlags flags)
	{
		ProjectSetting::SetGameViewSize(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), flags);
	}

	void EditorGameViewPanel::PanelSizeChangeEnd(const Vector2& size)
	{
		if (mLastWidth == ProjectSetting::gameViewWidth && mLastHeight == ProjectSetting::gameViewHeight)
			return;

		auto pGUIManager = EditorGUIManager::GetInstance();

		GlobalData::srcWidth = static_cast<uint32_t>(size.x - pGUIManager->mViewBorderSize.x * 2.0f);
		GlobalData::srcHeight = static_cast<uint32_t>(size.y - pGUIManager->mViewBorderSize.y * 2.0f - pGUIManager->mHeaderSize);

		RenderAPI::GetInstance()->OnGameViewSizeChange();
	}
}