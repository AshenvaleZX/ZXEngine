#include "EditorInspectorPanel.h"
#include "ImGuiTextureManager.h"
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
#include "../MaterialData.h"

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
				else if (curAsset->type == AssetType::Material || 
					curAsset->type == AssetType::RayTracingMaterial)
					DrawMaterial(static_cast<AssetMaterialInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Model)
					DrawModel(static_cast<AssetModelInfo*>(curAssetInfo));
			}
		}
		ImGui::End();
	}

	void EditorInspectorPanel::DrawMaterial(Material* material)
	{
		string suffix = material->type == MaterialType::Rasterization ? " (Material)" : " (Ray Tracing Material)";
		string title = material->name + suffix;
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader(title.c_str()))
			return;

		if (material->type == MaterialType::Rasterization)
		{
			ImGui::Text("Shader");
			ImGui::SameLine(120);
			if (ImGui::Button(material->shader->name.c_str()))
			{
				Debug::Log("Click Shader");
			}
		}
		else
		{
			ImGui::Text("Hit Group:");
			ImGui::SameLine(120);
			ImGui::Text(std::to_string(material->hitGroupIdx).c_str());
		}

		uint32_t idx = 0;
		ImGui::PushItemWidth(60);
		for (auto& iter : material->data->floatDatas)
		{
			float value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragFloat(("##float" + to_string(idx)).c_str(), &value, 0.1f, -FLT_MAX, FLT_MAX);
			idx++;
		}
		for (auto& iter : material->data->uintDatas)
		{
			int value = static_cast<int>(iter.second);
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragInt(("##uint32_t" + to_string(idx)).c_str(), &value, 1.0f, 0, UINT32_MAX);
			idx++;
		}
		for (auto& iter : material->data->vec2Datas)
		{
			Vector2 value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragFloat(("##Vec2X" + to_string(idx)).c_str(), &value.x, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec2Y" + to_string(idx)).c_str(), &value.y, 0.25f, -FLT_MAX, FLT_MAX);
		}
		for (auto& iter : material->data->vec3Datas)
		{
			Vector3 value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragFloat(("##Vec3X" + to_string(idx)).c_str(), &value.x, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec3Y" + to_string(idx)).c_str(), &value.y, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec3Z" + to_string(idx)).c_str(), &value.z, 0.25f, -FLT_MAX, FLT_MAX);
			idx++;
		}
		for (auto& iter : material->data->vec4Datas)
		{
			Vector4 value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragFloat(("##Vec4X" + to_string(idx)).c_str(), &value.x, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec4Y" + to_string(idx)).c_str(), &value.y, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec4Z" + to_string(idx)).c_str(), &value.z, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec4W" + to_string(idx)).c_str(), &value.w, 0.25f, -FLT_MAX, FLT_MAX);
			idx++;
		}
		ImGui::PopItemWidth();

		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		for (auto& iter : material->data->textures)
		{
			if (iter.second->type == TextureType::ZX_2D)
			{
				uint32_t id = iter.second->GetID();
				if (!ImTextureMgr->CheckExistenceByEngineID(id))
					ImTextureMgr->CreateFromEngineID(id);
				ImGui::Text(iter.first.c_str());
				ImGui::SameLine(120); ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(50.0f, 50.0f));
			}
		}
	}

	void EditorInspectorPanel::DrawTransform(Transform* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Transform"))
			return;

		ImGui::PushItemWidth(60);
		Vector3 position = component->GetLocalPosition();
		ImGui::Text("Position    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##posX", &position.x, 0.15f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##posY", &position.y, 0.15f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##posZ", &position.z, 0.15f, -FLT_MAX, FLT_MAX);
		component->SetLocalPosition(position);
		Vector3 euler = component->GetLocalEulerAngles();
		ImGui::Text("Rotation    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##rotX", &euler.x, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##rotY", &euler.y, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##rotZ", &euler.z, 0.25f, -FLT_MAX, FLT_MAX);
		component->SetLocalEulerAngles(euler);
		Vector3 scale = component->GetLocalScale();
		ImGui::Text("Scale       ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); ImGui::DragFloat("##scaX", &scale.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::DragFloat("##scaY", &scale.y, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::DragFloat("##scaZ", &scale.z, 0.01f, -FLT_MAX, FLT_MAX);
		component->SetLocalScale(scale);
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
		ImGui::Text("Cast Shadow     ");
		ImGui::SameLine(); ImGui::Checkbox("##castShadow", &component->castShadow);
		ImGui::Text("Receive Shadow  ");
		ImGui::SameLine(); ImGui::Checkbox("##receiveShadow", &component->receiveShadow);
	}

	void EditorInspectorPanel::DrawCamera(Camera* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Camera"))
			return;

		ImGui::Text("Field of View ");
		ImGui::PushItemWidth(180);
		ImGui::SameLine(); ImGui::SliderFloat("##fov", &component->Fov, 1.0f, 179.0f, "");
		ImGui::PushItemWidth(50);
		ImGui::SameLine(); ImGui::DragFloat("##fov", &component->Fov, 0.1f, 1.0f, 179.0f);
		ImGui::PopItemWidth();

		ImGui::Text("Near Clip     ");
		ImGui::SameLine(); ImGui::DragFloat("##nearClipDis", &component->nearClipDis, 0.1f, 0.0f, FLT_MAX);
		ImGui::Text("Far Clip      ");
		ImGui::SameLine(); ImGui::DragFloat("##farClipDis", &component->farClipDis, 0.1f, 0.0f, FLT_MAX);
		ImGui::Text("After Effects ");
		ImGui::SameLine(); ImGui::Checkbox("##enableAfterEffects", &component->enableAfterEffects);
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

		float size = component->size;
		ImGui::Text("Size  ");
		ImGui::SameLine(); ImGui::DragFloat("##Size", &size, 0.1f, 0.0f, FLT_MAX);

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

		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		uint32_t id = component->texture->GetID();
		if (!ImTextureMgr->CheckExistenceByEngineID(id))
			ImTextureMgr->CreateFromEngineID(id);
		ImGui::Text("Image  ");
		ImGui::SameLine(); ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(50.0f, 50.0f));
	}

	void EditorInspectorPanel::DrawParticleSystem(ParticleSystem* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("ParticleSystem"))
			return;

		int particleNum = (int)component->particleNum;
		ImGui::Text("ParticleNum ");
		ImGui::SameLine(); ImGui::DragInt("##ParticleNum", &particleNum, 0.1f, 0, INT_MAX);

		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		uint32_t id = component->textureID;
		if (!ImTextureMgr->CheckExistenceByEngineID(id))
			ImTextureMgr->CreateFromEngineID(id);
		ImGui::Text("Texture     ");
		ImGui::SameLine(); ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(50.0f, 50.0f));

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

		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		if (!ImTextureMgr->CheckExistenceByEngineID(id))
			ImTextureMgr->CreateFromEngineID(id);
		ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2((float)width, (float)height));
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