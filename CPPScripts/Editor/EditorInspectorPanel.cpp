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

		ImGui::Text("Mesh            ");
		ImGui::SameLine(); 
		if (ImGui::Button(component->modelName.c_str()))
		{
			Debug::Log("Click Mesh");
		}
		ImGui::Text("Material        ");
		ImGui::SameLine(); 
		if (ImGui::Button(component->matetrial->name.c_str()))
		{
			Debug::Log("Click Material");
		}
		bool castShadow = component->castShadow;
		ImGui::Text("Cast Shadow     ");
		ImGui::SameLine(); ImGui::Checkbox("##castShadow", &castShadow);
		bool receiveShadow = component->receiveShadow;
		ImGui::Text("Receive Shadow  ");
		ImGui::SameLine(); ImGui::Checkbox("##receiveShadow", &receiveShadow);
	}

	void EditorInspectorPanel::DrawCamera(Camera* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Camera"))
			return;

		float fov = component->Fov;
		ImGui::Text("Field of View ");
		ImGui::PushItemWidth(180);
		ImGui::SameLine(); ImGui::SliderFloat("##fov", &fov, 1.0f, 179.0f, "");
		ImGui::PushItemWidth(50);
		ImGui::SameLine(); ImGui::DragFloat("##fov", &fov, 0.1f, 1.0f, 179.0f);
		ImGui::PopItemWidth();

		float nearClipDis = component->nearClipDis;
		ImGui::Text("Near Clip     ");
		ImGui::SameLine(); ImGui::DragFloat("##nearClipDis", &nearClipDis, 0.1f, 0.0f, FLT_MAX);
		float farClipDis = component->farClipDis;
		ImGui::Text("Far Clip      ");
		ImGui::SameLine(); ImGui::DragFloat("##farClipDis", &farClipDis, 0.1f, 0.0f, FLT_MAX);
	}

	void EditorInspectorPanel::DrawLight(Light* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Light"))
			return;

		// Type
		static ImGuiComboFlags flags = 0;
		const char* items[] = { "None", "Directional", "Point" };
		static int item_current_idx = component->type;
		const char* combo_preview_value = items[item_current_idx];
		ImGui::Text("Type      ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##Type", combo_preview_value, flags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (item_current_idx == i);
				if (ImGui::Selectable(items[i], is_selected))
					item_current_idx = i;
			}
			ImGui::EndCombo();
		}

		// Color
		Vector3 lightColor = component->color;
		ImVec4 color = ImVec4(lightColor.r, lightColor.g, lightColor.b, 1.0f);
		ImGui::Text("Color     ");
		ImGui::SameLine(); ImGui::ColorEdit3("##color", (float*)&color);

		// Intensity
		float intensity = component->intensity;
		ImGui::Text("Intensity ");
		ImGui::SameLine(); ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, FLT_MAX);
	}

	void EditorInspectorPanel::DrawGameLogic(GameLogic* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("GameLogic"))
			return;

		ImGui::Text("Lua Script    ");
		ImGui::SameLine();
		if (ImGui::Button(component->luaName.c_str()))
		{
			Debug::Log("Click Lua");
		}
	}

	void EditorInspectorPanel::DrawUITextRenderer(UITextRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("UITextRenderer"))
			return;

		char* text = (char*)component->text.c_str();
		ImGui::Text("Text  ");
		ImGui::SameLine(); ImGui::InputTextMultiline("##text", text, 256);

		Vector3 textColor = component->color;
		ImVec4 color = ImVec4(textColor.r, textColor.g, textColor.b, 1.0f);
		ImGui::Text("Color ");
		ImGui::SameLine(); ImGui::ColorEdit3("##color", (float*)&color);
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