#include "EditorInspectorPanel.h"
#include "EditorDataManager.h"
#include "../GameObject.h"
#include "../Component.h"
#include "../Transform.h"
#include "../MeshRenderer.h"
#include "../ZCamera.h"
#include "../Light.h"
#include "../GameLogic.h"
#include "../UITextRenderer.h"
#include "../UITextureRenderer.h"
#include "../ParticleSystem.h"

namespace ZXEngine
{
	void EditorInspectorPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::hierarchyWidth + (float)GlobalData::srcWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::inspectorWidth, (float)ProjectSetting::inspectorHeight));

		// 设置面板具体内容
		if (ImGui::Begin("Inspector"))
		{
			auto curGO = EditorDataManager::GetInstance()->selectedGO;
			if (curGO != nullptr)
			{
				ImGui::Text(curGO->name.c_str());
				for (auto component : curGO->components)
				{
					ImGui::Text(to_string(component.first).c_str());
				}
			}
		}
		ImGui::End();
	}
}