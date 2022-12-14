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
				for (auto iter : curGO->components)
				{
					ComponentType type = iter.first;
					if (type == ComponentType::T_Transform)
						DrawTransform(static_cast<Transform*>(iter.second));
					else if (type == ComponentType::T_MeshRenderer)
						DrawMeshRenderer(static_cast<MeshRenderer*>(iter.second));
					else if (type == ComponentType::T_Camera)
						DrawCamera(static_cast<Camera*>(iter.second));
					else if (type == ComponentType::T_Light)
						DrawLight(static_cast<Light*>(iter.second));
					else if (type == ComponentType::T_GameLogic)
						DrawGameLogic(static_cast<GameLogic*>(iter.second));
					else if (type == ComponentType::T_UITextRenderer)
						DrawUITextRenderer(static_cast<UITextRenderer*>(iter.second));
					else if (type == ComponentType::T_UITextureRenderer)
						DrawUITextureRenderer(static_cast<UITextureRenderer*>(iter.second));
					else if (type == ComponentType::T_ParticleSystem)
						DrawParticleSystem(static_cast<ParticleSystem*>(iter.second));
				}
			}
		}
		ImGui::End();
	}

	void EditorInspectorPanel::DrawTransform(Transform* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Transform"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawMeshRenderer(MeshRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("MeshRenderer"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawCamera(Camera* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Camera"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawLight(Light* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Light"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawGameLogic(GameLogic* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("GameLogic"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawUITextRenderer(UITextRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("UITextRenderer"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawUITextureRenderer(UITextureRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("UITextureRenderer"))
			return;

		ImGui::Text("Temp Content");
	}

	void EditorInspectorPanel::DrawParticleSystem(ParticleSystem* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("ParticleSystem"))
			return;

		ImGui::Text("Temp Content");
	}
}