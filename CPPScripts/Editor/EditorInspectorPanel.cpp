#include "EditorInspectorPanel.h"
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
#include "../ZShader.h"

namespace ZXEngine
{
	void EditorInspectorPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::hierarchyWidth + (float)GlobalData::srcWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::inspectorWidth, (float)ProjectSetting::inspectorHeight));

		// 设置面板具体内容
		if (ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			auto curGO = EditorDataManager::GetInstance()->selectedGO;
			auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
			if (curGO != nullptr)
			{
				Material* mat = nullptr;
				ImGui::Text(curGO->name.c_str());
				for (auto& iter : curGO->components)
				{
					ComponentType type = iter.first;
					if (type == ComponentType::Transform)
						DrawTransform(static_cast<Transform*>(iter.second));
					else if (type == ComponentType::Camera)
						DrawCamera(static_cast<Camera*>(iter.second));
					else if (type == ComponentType::Light)
						DrawLight(static_cast<Light*>(iter.second));
					else if (type == ComponentType::GameLogic)
						DrawGameLogic(static_cast<GameLogic*>(iter.second));
					else if (type == ComponentType::UITextRenderer)
						DrawUITextRenderer(static_cast<UITextRenderer*>(iter.second));
					else if (type == ComponentType::UITextureRenderer)
						DrawUITextureRenderer(static_cast<UITextureRenderer*>(iter.second));
					else if (type == ComponentType::ParticleSystem)
						DrawParticleSystem(static_cast<ParticleSystem*>(iter.second));
					else if (type == ComponentType::MeshRenderer)
					{
						auto meshRenderer = static_cast<MeshRenderer*>(iter.second);
						mat = meshRenderer->matetrial;
						DrawMeshRenderer(meshRenderer);
					}
				}
				ImGui::Separator();
				if (mat != nullptr)
					DrawMaterial(mat);
			}
			else if (curAsset != nullptr)
			{
				auto curAssetInfo = EditorDataManager::GetInstance()->curAssetInfo;
				if (curAsset->type == AssetType::Script)
					DrawScript(static_cast<AssetScriptInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Shader)
					DrawShader(static_cast<AssetShaderInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Texture)
					DrawTexture(static_cast<AssetTextureInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Material)
					DrawMaterial(static_cast<AssetMaterialInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Model)
					DrawModel(static_cast<AssetModelInfo*>(curAssetInfo));
			}
		}
		ImGui::End();
	}

	void EditorInspectorPanel::DrawMaterial(Material* material)
	{
		string title = material->name + " (Material)";
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader(title.c_str()))
			return;

		ImGui::Text("Shader");
		ImGui::SameLine(120);
		if (ImGui::Button(material->shader->name.c_str()))
		{
			Debug::Log("Click Shader");
		}

		for (auto& iter : material->textures)
		{
			unsigned int id = iter.second->GetID();
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::Image((void*)(intptr_t)id, ImVec2(50.0f, 50.0f));
		}
	}

	void EditorInspectorPanel::DrawTransform(Transform* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Transform"))
			return;

		ImGui::PushItemWidth(60);
		Vector3 position = component->GetLocalPosition();
		float posX = position.x;
		float posY = position.y;
		float posZ = position.z;
		ImGui::Text("Position    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##posX", &posX, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##posY", &posY, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##posZ", &posZ, 0.25f, -FLT_MAX, FLT_MAX);
		Vector3 euler = component->GetLocalEulerAngles();
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
		Vector3 scale = component->GetLocalScale();
		float scaX = scale.x;
		float scaY = scale.y;
		float scaZ = scale.z;
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
		static int item_current_idx = (int)component->type;
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
			string path = Utils::ConvertPathToWindowsFormat(component->luaFullPath);
			std::wstring wsStr = std::wstring(path.begin(), path.end());
			ShellExecute(NULL, NULL, wsStr.c_str(), NULL, NULL, SW_SHOW);
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

		unsigned int id = component->texture->GetID();
		ImGui::Text("Image  ");
		ImGui::SameLine(); ImGui::Image((void*)(intptr_t)id, ImVec2(50.0f, 50.0f));
	}

	void EditorInspectorPanel::DrawParticleSystem(ParticleSystem* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("ParticleSystem"))
			return;

		int particleNum = (int)component->particleNum;
		ImGui::Text("ParticleNum ");
		ImGui::SameLine(); ImGui::DragInt("##ParticleNum", &particleNum, 0.1f, 0, INT_MAX);

		unsigned int id = component->textureID;
		ImGui::Text("Texture     ");
		ImGui::SameLine(); ImGui::Image((void*)(intptr_t)id, ImVec2(50.0f, 50.0f));

		float lifeTime = component->lifeTime;
		ImGui::Text("LifeTime    ");
		ImGui::SameLine(); ImGui::DragFloat("##lifeTime", &lifeTime, 0.01f, 0.0f, FLT_MAX);

		Vector3 velocity = component->velocity;
		ImVec4 v = ImVec4(velocity.x, velocity.y, velocity.z, 1.0f);
		ImGui::Text("Velocity    ");
		ImGui::SameLine(); ImGui::DragFloat3("##velocity", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);

		Vector3 offset = component->offset;
		ImVec4 o = ImVec4(offset.x, offset.y, offset.z, 1.0f);
		ImGui::Text("StartOffset ");
		ImGui::SameLine(); ImGui::DragFloat3("##offset", (float*)&o, 0.01f, -FLT_MAX, FLT_MAX);
	}

	void EditorInspectorPanel::DrawScript(AssetScriptInfo* info)
	{
		string title = info->name + " (Lua Script)";
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader(title.c_str()))
			return;

		ImGui::PushTextWrapPos(0.0f);
		ImGui::TextUnformatted(info->preview.c_str());
		ImGui::PopTextWrapPos();
	}

	void EditorInspectorPanel::DrawShader(AssetShaderInfo* info)
	{
		string title = info->name + " (Shader)";
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader(title.c_str()))
			return;

		ImGui::PushTextWrapPos(0.0f);
		ImGui::TextUnformatted(info->preview.c_str());
		ImGui::PopTextWrapPos();
	}

	void EditorInspectorPanel::DrawTexture(AssetTextureInfo* info)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Texture"))
			return;

		ImGui::Text("Name:");
		ImGui::SameLine(120);
		ImGui::Text(info->name.c_str());

		ImGui::Text("Format:");
		ImGui::SameLine(120);
		ImGui::Text(info->format.c_str());

		auto id = info->texture->GetID();
		auto width = info->texture->width;
		auto height = info->texture->height;

		string sizeText = to_string(width) + "x" + to_string(height);
		ImGui::Text("Size:");
		ImGui::SameLine(120);
		ImGui::Text(sizeText.c_str());
		
		int maxWidth = (int)ProjectSetting::inspectorWidth - 16;
		if (width > maxWidth)
		{
			height = height * maxWidth / width;
			width = maxWidth;
		}
		ImGui::Image((void*)(intptr_t)id, ImVec2((float)width, (float)height));
	}

	void EditorInspectorPanel::DrawMaterial(AssetMaterialInfo* info)
	{
		DrawMaterial(info->material);
	}

	void EditorInspectorPanel::DrawModel(AssetModelInfo* info)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Model"))
			return;

		ImGui::Text("Name:");
		ImGui::SameLine(120);
		ImGui::Text(info->name.c_str());

		ImGui::Text("Format:");
		ImGui::SameLine(120);
		ImGui::Text(info->format.c_str());

		ImGui::Text("Vertices Num:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->verticesNum).c_str());

		ImGui::Text("Triangles Num:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->trianglesNum).c_str());

		ImGui::Text("Bounds Size:");
		ImGui::Text("          X:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->boundsSizeX).c_str());
		ImGui::Text("          Y:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->boundsSizeY).c_str());
		ImGui::Text("          Z:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->boundsSizeZ).c_str());
	}
}