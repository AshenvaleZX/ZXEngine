#include "EditorAssetPreviewPanel.h"
#include "EditorAssetPreviewer.h"
#include "ImGuiTextureManager.h"
#include "EditorGUIManager.h"
#include "../FBOManager.h"
#include "../RenderAPI.h"
#include "../Resources.h"
#include "../Texture.h"

namespace ZXEngine
{
	EditorAssetPreviewPanel::EditorAssetPreviewPanel()
	{
		mLoadingTexture = new Texture(Resources::GetAssetFullPath("Textures/loading.jpg", true));
	}

	EditorAssetPreviewPanel::~EditorAssetPreviewPanel()
	{
		if (mLoadingTexture)
			delete mLoadingTexture;
	}

	void EditorAssetPreviewPanel::DrawPanel()
	{
		auto pGUIManager = EditorGUIManager::GetInstance();
		if (!pGUIManager->mAssetPreviewer->Check())
			return;

		ImGui::SetNextWindowPos(ImVec2((float)(ProjectSetting::srcWidth - ProjectSetting::inspectorWidth), (float)(ProjectSetting::mainBarHeight + ProjectSetting::inspectorHeight)));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::inspectorWidth, (float)ProjectSetting::inspectorWidth));

		if (ImGui::Begin("AssetPreview", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			auto ImTextureMgr = ImGuiTextureManager::GetInstance();
			auto buffer = FBOManager::GetInstance()->GetFBO("AssetPreview");
			uint32_t id = RenderAPI::GetInstance()->GetRenderBufferTexture(buffer->ColorBuffer);
			if (!ImTextureMgr->CheckExistenceByEngineID(id))
				ImTextureMgr->CreateFromEngineID(id);

			float imageWidth = static_cast<float>(ProjectSetting::inspectorWidth) - pGUIManager->mViewBorderSize.x * 2.0f;

			if (pGUIManager->mAssetPreviewer->isLoading)
			{
				id = mLoadingTexture->GetID();
				if (!ImTextureMgr->CheckExistenceByEngineID(id))
					ImTextureMgr->CreateFromEngineID(id);

				ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(imageWidth, imageWidth));
			}
			else
			{
#ifdef ZX_API_OPENGL
				ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(imageWidth, imageWidth), ImVec2(0, 1), ImVec2(1, 0));
#else
				ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(imageWidth, imageWidth));
#endif
			}
		}
		ImGui::End();
	}
}