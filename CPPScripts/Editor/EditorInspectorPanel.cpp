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

		ImGui::PushItemWidth(60);
		float posX = component->position.x;
		float posY = component->position.y;
		float posZ = component->position.z;
		ImGui::Text("Position    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##posX", &posX, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##posY", &posY, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##posZ", &posZ, 0.25f, -FLT_MAX, FLT_MAX);
		Vector3 euler = component->rotation.GetEulerAngles();
		float rotX = euler.x;
		float rotY = euler.y;
		float rotZ = euler.z;
		ImGui::Text("Rotation    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##rotX", &rotX, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##rotY", &rotY, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##rotZ", &rotZ, 0.25f, -FLT_MAX, FLT_MAX);
		float scaX = component->scale.x;
		float scaY = component->scale.y;
		float scaZ = component->scale.z;
		ImGui::Text("Scale       ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##scaX", &scaX, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##scaY", &scaY, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##scaZ", &scaZ, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::PopItemWidth();
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