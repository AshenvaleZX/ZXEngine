#include "EditorInspectorPanel.h"
#include "ImGuiTextureManager.h"
#include "../GameObject.h"
#include "../Component/Component.h"
#include "../Component/Transform.h"
#include "../Component/RectTransform.h"
#include "../Component/MeshRenderer.h"
#include "../Component/ZCamera.h"
#include "../Component/Light.h"
#include "../Component/GameLogic.h"
#include "../Component/UITextRenderer.h"
#include "../Component/UITextureRenderer.h"
#include "../Component/ParticleSystem.h"
#include "../Component/Physics/BoxCollider.h"
#include "../Component/Physics/PlaneCollider.h"
#include "../Component/Physics/SphereCollider.h"
#include "../ZShader.h"
#include "../Material.h"
#include "../MaterialData.h"
#include "../Audio/AudioClip.h"

namespace ZXEngine
{
	void EditorInspectorPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::hierarchyWidth + (float)ProjectSetting::gameViewWidth, (float)ProjectSetting::mainBarHeight));
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
					else if (type == ComponentType::RectTransform)
						DrawRectTransform(static_cast<RectTransform*>(iter.second));
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
					else if (type == ComponentType::UIButton)
						DrawUIButton(static_cast<UIButton*>(iter.second));
					else if (type == ComponentType::ParticleSystem)
						DrawParticleSystem(static_cast<ParticleSystem*>(iter.second));
					else if (type == ComponentType::BoxCollider)
						DrawBoxCollider(static_cast<BoxCollider*>(iter.second));
					else if (type == ComponentType::PlaneCollider)
						DrawPlaneCollider(static_cast<PlaneCollider*>(iter.second));
					else if (type == ComponentType::SphereCollider)
						DrawSphereCollider(static_cast<SphereCollider*>(iter.second));
					else if (type == ComponentType::Circle2DCollider)
						DrawCircle2DCollider(static_cast<Circle2DCollider*>(iter.second));
					else if (type == ComponentType::RigidBody)
						DrawRigidBody(static_cast<ZRigidBody*>(iter.second));
					else if (type == ComponentType::Animator)
						DrawAnimator(static_cast<Animator*>(iter.second));
					else if (type == ComponentType::SpringJoint)
						DrawSpringJoint(static_cast<SpringJoint*>(iter.second));
					else if (type == ComponentType::DistanceJoint)
						DrawDistanceJoint(static_cast<ZDistanceJoint*>(iter.second));
					else if (type == ComponentType::Cloth)
						DrawCloth(static_cast<Cloth*>(iter.second));
					else if (type == ComponentType::AudioSource)
						DrawAudioSource(static_cast<AudioSource*>(iter.second));
					else if (type == ComponentType::AudioListener)
						DrawAudioListener(static_cast<AudioListener*>(iter.second));
					else if (type == ComponentType::MeshRenderer)
					{
						auto meshRenderer = static_cast<MeshRenderer*>(iter.second);
						mat = meshRenderer->mMatetrial;
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
				if (curAsset->type == AssetType::Script ||
					curAsset->type == AssetType::Text)
					DrawScript(static_cast<AssetScriptInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Shader ||
					curAsset->type == AssetType::RayTracingShader)
					DrawShader(static_cast<AssetShaderInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Texture)
					DrawTexture(static_cast<AssetTextureInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Material || 
					curAsset->type == AssetType::RayTracingMaterial ||
					curAsset->type == AssetType::DeferredMaterial)
					DrawMaterial(static_cast<AssetMaterialInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Model)
					DrawModel(static_cast<AssetModelInfo*>(curAssetInfo));
				else if (curAsset->type == AssetType::Audio)
					DrawAudio(static_cast<AssetAudioInfo*>(curAssetInfo));
			}
		}
		ImGui::End();
	}

	void EditorInspectorPanel::DrawMaterial(Material* material)
	{
		if (material == nullptr)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (!ImGui::CollapsingHeader("Unknown (Material)"))
				return;

			ImGui::Text("Material is loading......");

			return;
		}

		string suffix = " (Material)";
		if (material->type == MaterialType::Deferred)
			suffix = " (Deferred Material)";
		else if (material->type == MaterialType::RayTracing)
			suffix = " (Ray Tracing Material)";

		string title = material->name + suffix;
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader(title.c_str()))
			return;

		if (material->type == MaterialType::Forward)
		{
			ImGui::Text("Shader");
			ImGui::SameLine(120);
			if (ImGui::Button(material->shader->name.c_str()))
			{
				Debug::Log("Click Shader");
			}
		}
		else if (material->type == MaterialType::RayTracing)
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

			if (value != iter.second)
			{
				material->data->isDirty = true;
				material->data->floatDatas[iter.first] = value;
			}
		}
		for (auto& iter : material->data->uintDatas)
		{
			int value = static_cast<int>(iter.second);
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragInt(("##uint32_t" + to_string(idx)).c_str(), &value, 1.0f, 0, UINT32_MAX);
			idx++;

			uint32_t uValue = static_cast<uint32_t>(value);
			if (uValue != iter.second)
			{
				material->data->isDirty = true;
				material->data->uintDatas[iter.first] = uValue;
			}
		}
		for (auto& iter : material->data->vec2Datas)
		{
			Vector2 value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragFloat(("##Vec2X" + to_string(idx)).c_str(), &value.x, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec2Y" + to_string(idx)).c_str(), &value.y, 0.25f, -FLT_MAX, FLT_MAX);
			idx++;

			if (value != iter.second)
			{
				material->data->isDirty = true;
				material->data->vec2Datas[iter.first] = value;
			}
		}
		for (auto& iter : material->data->vec3Datas)
		{
			Vector3 value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::DragFloat(("##Vec3X" + to_string(idx)).c_str(), &value.x, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec3Y" + to_string(idx)).c_str(), &value.y, 0.25f, -FLT_MAX, FLT_MAX);
			ImGui::SameLine(); ImGui::DragFloat(("##Vec3Z" + to_string(idx)).c_str(), &value.z, 0.25f, -FLT_MAX, FLT_MAX);
			idx++;

			if (value != iter.second)
			{
				material->data->isDirty = true;
				material->data->vec3Datas[iter.first] = value;
			}
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

			if (value != iter.second)
			{
				material->data->isDirty = true;
				material->data->vec4Datas[iter.first] = value;
			}
		}
		ImGui::PopItemWidth();

		for (auto& iter : material->data->colorDatas)
		{
			Vector4 value = iter.second;
			ImVec4 color = ImVec4(value.x, value.y, value.z, value.w);
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(120); ImGui::ColorEdit4(("##color" + to_string(idx)).c_str(), (float*)&color);
			idx++;

			Vector4 newValue = Vector4(color.x, color.y, color.z, color.w);
			if (newValue != iter.second)
			{
				material->data->isDirty = true;
				material->data->colorDatas[iter.first] = newValue;
			}
		}

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

		if (material->type != MaterialType::RayTracing)
		{
			ImGui::Text("RenderQueue");
			auto renderQueueType = material->GetRenderQueueType();
			if (renderQueueType == RenderQueueType::Deferred)
			{
				ImGui::SameLine(120);
				ImGui::Text("Deferred");
			}
			else if (renderQueueType == RenderQueueType::Opaque)
			{
				ImGui::SameLine(120);
				ImGui::Text("Opaque");
			}
			else if (renderQueueType == RenderQueueType::Transparent)
			{
				ImGui::SameLine(120);
				ImGui::Text("Transparent");
			}
		}
	}

	void EditorInspectorPanel::DrawTransform(Transform* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Transform"))
			return;

		bool xChange = false, yChange = false, zChange = false;

		ImGui::PushItemWidth(60);
		Vector3 position = component->GetLocalPosition();
		ImGui::Text("Position    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); xChange = ImGui::DragFloat("##posX", &position.x, 0.15f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); yChange = ImGui::DragFloat("##posY", &position.y, 0.15f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); zChange = ImGui::DragFloat("##posZ", &position.z, 0.15f, -FLT_MAX, FLT_MAX);
		if (xChange || yChange || zChange)
			component->SetLocalPosition(position);

		Vector3 euler = component->GetLocalEulerAngles();
		Vector3 eulerOrigin = euler;
		ImGui::Text("Rotation    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); xChange = ImGui::DragFloat("##rotX", &euler.x, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); yChange = ImGui::DragFloat("##rotY", &euler.y, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); zChange = ImGui::DragFloat("##rotZ", &euler.z, 0.25f, -FLT_MAX, FLT_MAX);
		if (xChange)
		{
			float delta = Math::Deg2Rad(euler.x - eulerOrigin.x);
			component->Rotate(component->GetRight(), delta);
		}
		if (yChange)
		{
			float delta = Math::Deg2Rad(euler.y - eulerOrigin.y);
			component->Rotate(component->GetUp(), delta);
		}
		if (zChange)
		{
			float delta = Math::Deg2Rad(euler.z - eulerOrigin.z);
			component->Rotate(component->GetForward(), delta);
		}

		Vector3 scale = component->GetLocalScale();
		ImGui::Text("Scale       ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); xChange = ImGui::DragFloat("##scaX", &scale.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); yChange = ImGui::DragFloat("##scaY", &scale.y, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); zChange = ImGui::DragFloat("##scaZ", &scale.z, 0.01f, -FLT_MAX, FLT_MAX);
		if (xChange || yChange || zChange)
			component->SetLocalScale(scale);
		ImGui::PopItemWidth();
	}

	void EditorInspectorPanel::DrawRectTransform(RectTransform* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Rect Transform"))
			return;

		bool xChange = false, yChange = false, zChange = false;

		ImGui::PushItemWidth(60);
		Vector3 position = component->GetLocalRectPosition();
		ImGui::Text("Position    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); xChange = ImGui::DragFloat("##posX", &position.x, 0.15f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); yChange = ImGui::DragFloat("##posY", &position.y, 0.15f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); zChange = ImGui::DragFloat("##posZ", &position.z, 0.15f, -FLT_MAX, FLT_MAX);
		if (xChange || yChange || zChange)
			component->SetLocalRectPosition(position);
		
		Vector3 euler = component->GetLocalEulerAngles();
		Vector3 eulerOrigin = euler;
		ImGui::Text("Rotation    ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); xChange = ImGui::DragFloat("##rotX", &euler.x, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); yChange = ImGui::DragFloat("##rotY", &euler.y, 0.25f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); zChange = ImGui::DragFloat("##rotZ", &euler.z, 0.25f, -FLT_MAX, FLT_MAX);
		if (xChange)
		{
			float delta = Math::Deg2Rad(euler.x - eulerOrigin.x);
			component->Rotate(component->GetRight(), delta);
		}
		if (yChange)
		{
			float delta = Math::Deg2Rad(euler.y - eulerOrigin.y);
			component->Rotate(component->GetUp(), delta);
		}
		if (zChange)
		{
			float delta = Math::Deg2Rad(euler.z - eulerOrigin.z);
			component->Rotate(component->GetForward(), delta);
		}

		Vector3 scale = component->GetLocalScale();
		ImGui::Text("Scale       ");
		ImGui::SameLine(); ImGui::Text("X");
		ImGui::SameLine(); xChange = ImGui::DragFloat("##scaX", &scale.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); yChange = ImGui::DragFloat("##scaY", &scale.y, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); zChange = ImGui::DragFloat("##scaZ", &scale.z, 0.01f, -FLT_MAX, FLT_MAX);
		if (xChange || yChange || zChange)
			component->SetLocalScale(scale);
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(95);
		ImGui::Text("Size              ");
		float width = component->GetWidth();
		ImGui::SameLine(); ImGui::DragFloat("##width", &width, 0.1f, 0.0f, FLT_MAX);
		float height = component->GetHeight();
		ImGui::SameLine(); ImGui::DragFloat("##height", &height, 0.1f, 0.0f, FLT_MAX);
		ImGui::PopItemWidth();
		if (width != component->GetWidth() || height != component->GetHeight())
			component->SetSize(width, height);

		ImGui::PushItemWidth(200);
		UIAnchorVertical anchorV = component->mAnchorV;
		static const char* rectTransVItems[] = { "Top", "Middle", "Bottom" };
		int rectTransVItemCurrentIdx = (int)anchorV;
		const char* rectTransVComboPreviewValue = rectTransVItems[rectTransVItemCurrentIdx];
		ImGui::Text("Vertical Anchor   ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##RectVerticalAnchor", rectTransVComboPreviewValue, 0))
		{
			for (int i = 0; i < IM_ARRAYSIZE(rectTransVItems); i++)
			{
				const bool is_selected = (rectTransVItemCurrentIdx == i);
				if (ImGui::Selectable(rectTransVItems[i], is_selected))
					rectTransVItemCurrentIdx = i;
			}
			ImGui::EndCombo();
		}
		if (anchorV != (UIAnchorVertical)rectTransVItemCurrentIdx)
			component->SetVerticalAnchor((UIAnchorVertical)rectTransVItemCurrentIdx);

		UIAnchorHorizontal anchorH = component->mAnchorH;
		static const char* rectTransHItems[] = { "Left", "Center", "Right" };
		int rectTransHItemCurrentIdx = (int)anchorH;
		const char* rectTransHComboPreviewValue = rectTransHItems[rectTransHItemCurrentIdx];
		ImGui::Text("Horizontal Anchor ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##RectHorizonAnchor", rectTransHComboPreviewValue, 0))
		{
			for (int i = 0; i < IM_ARRAYSIZE(rectTransHItems); i++)
			{
				const bool is_selected = (rectTransHItemCurrentIdx == i);
				if (ImGui::Selectable(rectTransHItems[i], is_selected))
					rectTransHItemCurrentIdx = i;
			}
			ImGui::EndCombo();
		}
		if (anchorH != (UIAnchorHorizontal)rectTransHItemCurrentIdx)
			component->SetHorizontalAnchor((UIAnchorHorizontal)rectTransHItemCurrentIdx);
		ImGui::PopItemWidth();
	}

	void EditorInspectorPanel::DrawMeshRenderer(MeshRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Mesh Renderer"))
			return;

		ImGui::Text("Enabled		  ");
		ImGui::SameLine(); ImGui::Checkbox("##enabled", &component->mEnabled);

		ImGui::Text("Mesh             ");
		ImGui::SameLine(); 
		if (ImGui::Button(component->mModelName.c_str()))
		{
			Debug::Log("Click Mesh");
		}
		ImGui::Text("Material         ");
		ImGui::SameLine(); 
		if (ImGui::Button(component->mMatetrial->name.c_str()))
		{
			Debug::Log("Click Material");
		}
		ImGui::Text("Cast Shadow      ");
		ImGui::SameLine(); ImGui::Checkbox("##castShadow", &component->mCastShadow);
		ImGui::Text("Receive Shadow   ");
		ImGui::SameLine(); ImGui::Checkbox("##receiveShadow", &component->mReceiveShadow);
	}

	void EditorInspectorPanel::DrawCamera(Camera* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Camera"))
			return;

		ImGui::Text("Field of View ");
		ImGui::PushItemWidth(180);
		ImGui::SameLine(); ImGui::SliderFloat("##fov", &component->mFOV, 1.0f, 179.0f, "");
		ImGui::PushItemWidth(50);
		ImGui::SameLine(); ImGui::DragFloat("##fov", &component->mFOV, 0.1f, 1.0f, 179.0f);
		ImGui::PopItemWidth();

		ImGui::Text("Near Clip     ");
		ImGui::SameLine(); ImGui::DragFloat("##nearClipDis", &component->mNearClipDis, 0.1f, 0.0f, FLT_MAX);
		ImGui::Text("Far Clip      ");
		ImGui::SameLine(); ImGui::DragFloat("##farClipDis", &component->mFarClipDis, 0.1f, 0.0f, FLT_MAX);
		ImGui::Text("After Effects ");
		ImGui::SameLine(); ImGui::Checkbox("##enableAfterEffects", &component->mEnableAfterEffects);
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
		ImGui::Text("Type         ");
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
		ImGui::Text("Color        ");
		ImGui::SameLine(); ImGui::ColorEdit3("##color", (float*)&color);

		// Intensity
		float intensity = component->intensity;
		ImGui::Text("Intensity    ");
		ImGui::SameLine(); ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, FLT_MAX);

		// Shadow Range
		ImGui::Text("Shadow Range ");
		ImGui::SameLine(); ImGui::DragFloat("##ShadowRange", &component->mDirectionalLightSpaceSize, 0.01f, 1.0f, 100.0f);
	}

	void EditorInspectorPanel::DrawGameLogic(GameLogic* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("GameLogic"))
			return;

		ImGui::Text("Lua Script       ");
		ImGui::SameLine();
		auto& name = component->GetLuaName();
		if (ImGui::Button(name.c_str()))
		{
			auto& path = component->GetLuaFullPath();
#if defined(ZX_PLATFORM_WINDOWS)
			system(("start " + path).c_str());
#elif defined(ZX_PLATFORM_MACOS)
			system(("open " + path).c_str());
#endif
		}

		for (auto& iter : component->mBoolVariables)
		{
			bool value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(135); ImGui::Checkbox(("##" + iter.first).c_str(), &value);

			if (value != iter.second)
				component->SetLuaVariable(iter.first, value);
		}

		for (auto& iter : component->mFloatVariables)
		{
			float value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(135); ImGui::DragFloat(("##" + iter.first).c_str(), &value, 0.1f, -FLT_MAX, FLT_MAX);

			if (value != iter.second)
				component->SetLuaVariable(iter.first, value);
		}

		for (auto& iter : component->mIntVariables)
		{
			int value = iter.second;
			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(135); ImGui::DragInt(("##" + iter.first).c_str(), &value, 1, INT_MIN, INT_MAX);

			if (value != iter.second)
				component->SetLuaVariable(iter.first, value);
		}

		static const size_t StagingSize = 256;
		static char StagingBuffer[StagingSize];

		for (auto& iter : component->mStringVariables)
		{
			const char* value = iter.second.c_str();
			// -1是为了保留一个位置放'\0'
#if defined(ZX_PLATFORM_WINDOWS)
			errno_t err = strncpy_s(StagingBuffer, value, StagingSize - 1);
			if (err != 0)
				continue;
#elif defined(ZX_PLATFORM_MACOS)
			// 不知道为什么在MacOS上无法识别strncpy_s，按理说应该是支持C11标准的
			strncpy(StagingBuffer, value, StagingSize - 1);
			StagingBuffer[StagingSize - 1] = '\0';
#endif

			ImGui::Text(iter.first.c_str());
			ImGui::SameLine(135); ImGui::InputText(("##" + iter.first).c_str(), StagingBuffer, StagingSize);

			if (strcmp(StagingBuffer, value) != 0)
				component->SetLuaVariable(iter.first, string(StagingBuffer));
		}
	}

	void EditorInspectorPanel::DrawUITextRenderer(UITextRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("UI Text Renderer"))
			return;

		char* text = (char*)component->text.c_str();
		ImGui::Text("Text               ");
		ImGui::SameLine(); ImGui::InputTextMultiline("##text", text, 256);
		component->SetContent(text);

		float size = component->size;
		ImGui::Text("Size               ");
		ImGui::SameLine(); ImGui::DragFloat("##Size", &size, 0.1f, 0.0f, FLT_MAX);

		TextHorizonAlignment hAlign = component->GetHorizonAlignment();
		static const char* hAlignItems[] = { "Left", "Center", "Right" };
		int hAlignItemCurrentIdx = (int)hAlign;
		const char* hAlignComboPreviewValue = hAlignItems[hAlignItemCurrentIdx];
		ImGui::Text("Horizon Alignment  ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##HorizonAlignment", hAlignComboPreviewValue, 0))
		{
			for (int i = 0; i < IM_ARRAYSIZE(hAlignItems); i++)
			{
				const bool is_selected = (hAlignItemCurrentIdx == i);
				if (ImGui::Selectable(hAlignItems[i], is_selected))
					hAlignItemCurrentIdx = i;
			}
			ImGui::EndCombo();
		}
		component->SetHorizonAlignment((TextHorizonAlignment)hAlignItemCurrentIdx);

		TextVerticalAlignment vAlign = component->GetVerticalAlignment();
		static const char* vAlignItems[] = { "Top", "Center", "Bottom" };
		int vAlignItemCurrentIdx = (int)vAlign;
		const char* vAlignComboPreviewValue = vAlignItems[vAlignItemCurrentIdx];
		ImGui::Text("Vertical Alignment ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##VerticalAlignment", vAlignComboPreviewValue, 0))
		{
			for (int i = 0; i < IM_ARRAYSIZE(vAlignItems); i++)
			{
				const bool is_selected = (vAlignItemCurrentIdx == i);
				if (ImGui::Selectable(vAlignItems[i], is_selected))
					vAlignItemCurrentIdx = i;
			}
			ImGui::EndCombo();
		}
		component->SetVerticalAlignment((TextVerticalAlignment)vAlignItemCurrentIdx);

		Vector3 textColor = component->color;
		ImVec4 color = ImVec4(textColor.r, textColor.g, textColor.b, 1.0f);
		ImGui::Text("Color              ");
		ImGui::SameLine(); ImGui::ColorEdit3("##color", (float*)&color);
	}

	void EditorInspectorPanel::DrawUITextureRenderer(UITextureRenderer* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("UI Texture Renderer"))
			return;

		ImGui::Text("Path     ");
		ImGui::SameLine(); ImGui::Text(component->texture->path.c_str());

		string size = to_string(component->texture->width) + "x" + to_string(component->texture->height);
		ImGui::Text("Size     ");
		ImGui::SameLine(); ImGui::Text(size.c_str());

		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		uint32_t id = component->texture->GetID();
		if (!ImTextureMgr->CheckExistenceByEngineID(id))
			ImTextureMgr->CreateFromEngineID(id);
		ImGui::Text("Image    ");
		ImGui::SameLine(); ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(50.0f, 50.0f));
	}

	void EditorInspectorPanel::DrawUIButton(UIButton* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("UI Button"))
			return;

		string size = to_string(component->mWidth) + "x" + to_string(component->mHeight);
		ImGui::Text("Size     ");
		ImGui::SameLine(); ImGui::Text(size.c_str());
	}

	void EditorInspectorPanel::DrawParticleSystem(ParticleSystem* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Particle System"))
			return;

		// Particle System
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 oldWindowPadding = style.WindowPadding;
			style.WindowPadding = ImVec2(5.0f, 5.0f);

			// 5 + 19 + 5 = 29, 19是Checkbox高度，可由ImGui::GetFrameHeight()获取
			static bool expand = true;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
			ImGui::BeginChild("Particle System", ImVec2(0.0f, 29.0f), ImGuiChildFlags_Border, ImGuiWindowFlags_None);
			ImGui::Checkbox("Particle System", &expand);
			ImGui::EndChild();
			ImGui::PopStyleVar();

			style.WindowPadding = oldWindowPadding;

			if (expand)
			{
				int particleNum = (int)component->mState.mMaxParticleNum;
				ImGui::Text("ParticleNum      ");
				ImGui::SameLine(); ImGui::DragInt("##ParticleNum", &particleNum, 0.1f, 0, INT_MAX);

				float lifeTime = component->mState.mLifeTime;
				ImGui::Text("LifeTime         ");
				ImGui::SameLine(); ImGui::DragFloat("##lifeTime", &lifeTime, 0.01f, 0.0f, FLT_MAX);

				Vector3 velocity = component->mState.mVelocity;
				ImVec4 v = ImVec4(velocity.x, velocity.y, velocity.z, 1.0f);
				ImGui::Text("Velocity         ");
				ImGui::SameLine(); ImGui::DragFloat3("##velocity", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);

				Vector3 offset = component->mState.mOffset;
				ImVec4 o = ImVec4(offset.x, offset.y, offset.z, 1.0f);
				ImGui::Text("StartOffset      ");
				ImGui::SameLine(); ImGui::DragFloat3("##offset", (float*)&o, 0.01f, -FLT_MAX, FLT_MAX);
			}
		}

		// Particle Emitter
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 oldWindowPadding = style.WindowPadding;
			style.WindowPadding = ImVec2(5.0f, 5.0f);

			static bool expand = true;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
			ImGui::BeginChild("Particle Emitter", ImVec2(0.0f, 29.0f), ImGuiChildFlags_Border, ImGuiWindowFlags_None);
			ImGui::Checkbox("Particle Emitter", &expand);
			ImGui::EndChild();
			ImGui::PopStyleVar();

			style.WindowPadding = oldWindowPadding;

			if (expand)
			{
				ImGui::Text("Rate             ");
				ImGui::SameLine(); ImGui::DragFloat("##rate", &component->mState.mEmissionState.mRate, 0.01f, 0.0f, FLT_MAX);

				ParticleEmissionState::Shape shape = component->mState.mEmissionState.mShape;
				static const char* emitterShapeItems[] = { "Sphere", "Hemisphere", "Cone", "Circle", "Box", "Line" };
				int emitterShapeItemCurrentIdx = static_cast<int>(shape);
				const char* emitterShapeComboPreviewValue = emitterShapeItems[emitterShapeItemCurrentIdx];
				ImGui::Text("Shape            ");
				ImGui::SameLine();
				if (ImGui::BeginCombo("##Shape", emitterShapeComboPreviewValue, 0))
				{
					for (int i = 0; i < IM_ARRAYSIZE(emitterShapeItems); i++)
					{
						const bool is_selected = (emitterShapeItemCurrentIdx == i);
						if (ImGui::Selectable(emitterShapeItems[i], is_selected))
							emitterShapeItemCurrentIdx = i;
					}
					ImGui::EndCombo();
				}
				ParticleEmissionState::Shape nShape = static_cast<ParticleEmissionState::Shape>(emitterShapeItemCurrentIdx);
				if (shape != nShape)
				{
					component->mState.mEmissionState.mShape = nShape;
				}

				ImGui::Text("Angle            ");
				ImGui::SameLine(); ImGui::DragFloat("##angle", &component->mState.mEmissionState.mAngle, 0.01f, 0.0f, 180.0f);

				ImGui::Text("Radius           ");
				ImGui::SameLine(); ImGui::DragFloat("##radius", &component->mState.mEmissionState.mRadius, 0.01f, 0.0f, FLT_MAX);

				ImGui::Text("Speed            ");
				ImGui::SameLine(); ImGui::DragFloat("##speed", &component->mState.mEmissionState.mSpeed, 0.01f, 0.0f, FLT_MAX);

				Vector3 dir = component->mState.mEmissionState.mRotation.ToEuler();
				ImGui::Text("Direction        ");
				ImGui::SameLine(); ImGui::DragFloat3("##dir", (float*)&dir, 0.01f, -FLT_MAX, FLT_MAX);

				Vector4 color = component->mState.mEmissionState.mColor;
				ImVec4 imColor = ImVec4(color.r, color.g, color.b, color.a);
				ImGui::Text("Color            ");
				ImGui::SameLine(); ImGui::ColorEdit4("##color", (float*)&imColor);
				Vector4 nColor = Vector4(imColor.x, imColor.y, imColor.z, imColor.w);
				if (color != nColor)
				{
					component->mState.mEmissionState.mColor = std::move(nColor);
				}

				ImGui::Text("Random Color     ");
				ImGui::SameLine(); ImGui::Checkbox("##randomColor", &component->mState.mEmissionState.mRandomColor);
			}
		}

		// Particle Evolver
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 oldWindowPadding = style.WindowPadding;
			style.WindowPadding = ImVec2(5.0f, 5.0f);

			static bool expand = true;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
			ImGui::BeginChild("Particle Evolver", ImVec2(0.0f, 29.0f), ImGuiChildFlags_Border, ImGuiWindowFlags_None);
			ImGui::Checkbox("Particle Evolver", &expand);
			ImGui::EndChild();
			ImGui::PopStyleVar();

			style.WindowPadding = oldWindowPadding;

			if (expand)
			{
				ImGui::Text("RGB Over Life    ");
				ImGui::BeginChild("RGB Over Life", ImVec2(0.0f, 33.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
				ImGui::PushItemWidth(36.0f);
				for (size_t i = 0; i < component->mEvolver.mRGBKeys.size(); i++)
				{
					auto& key = component->mEvolver.mRGBKeys[i];

					if (i > 0) ImGui::SameLine();

					string label = "##KeyT1_" + to_string(i);
					ImGui::DragFloat(label.c_str(), &key.t, 0.01f, 0.0f, 1.0f, "%.2f");

					label = "##KeyC1_" + to_string(i);
					ImGui::SameLine(); ImGui::ColorEdit3(label.c_str(), (float*)&key.value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				}
				ImGui::PopItemWidth();
				ImGui::EndChild();

				ImGui::Text("Alpha Over Life  ");
				ImGui::BeginChild("Alpha Over Life", ImVec2(0.0f, 33.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
				ImGui::PushItemWidth(36.0f);
				for (size_t i = 0; i < component->mEvolver.mAlphaKeys.size(); i++)
				{
					auto& key = component->mEvolver.mAlphaKeys[i];

					if (i > 0) ImGui::SameLine();

					string label = "##KeyT2_" + to_string(i);
					ImGui::DragFloat(label.c_str(), &key.t, 0.01f, 0.0f, 1.0f, "%.2f");

					label = "##KeyC2_" + to_string(i);
					ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, key.value);
					ImGui::SameLine(); ImGui::ColorEdit4(label.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview);
					key.value = color.w;
				}
				ImGui::PopItemWidth();
				ImGui::EndChild();
			}
		}

		// Particle Renderer
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 oldWindowPadding = style.WindowPadding;
			style.WindowPadding = ImVec2(5.0f, 5.0f);

			static bool expand = true;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
			ImGui::BeginChild("Particle Renderer", ImVec2(0.0f, 29.0f), ImGuiChildFlags_Border, ImGuiWindowFlags_None);
			ImGui::Checkbox("Particle Renderer", &expand);
			ImGui::EndChild();
			ImGui::PopStyleVar();

			style.WindowPadding = oldWindowPadding;

			if (expand)
			{
				auto ImTextureMgr = ImGuiTextureManager::GetInstance();
				uint32_t id = component->mRenderer.mTextureID;
				if (!ImTextureMgr->CheckExistenceByEngineID(id))
					ImTextureMgr->CreateFromEngineID(id);
				ImGui::Text("Texture          ");
				ImGui::SameLine(); ImGui::Text(component->mRenderer.mTexturePath.c_str());
				ImGui::Text("                 ");
				ImGui::SameLine(); ImGui::Image(ImTextureMgr->GetImTextureIDByEngineID(id), ImVec2(50.0f, 50.0f));
			}
		}
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
		if (info->meshRenderer == nullptr)
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

			ImGui::Text("More information is loading......");

			return;
		}

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
		ImGui::Text(to_string(info->meshRenderer->mVerticesNum).c_str());

		ImGui::Text("Triangles Num:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->mTrianglesNum).c_str());

		ImGui::Text("Bounds Size:");
		ImGui::Text("          X:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->mAABBSizeX).c_str());
		ImGui::Text("          Y:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->mAABBSizeY).c_str());
		ImGui::Text("          Z:");
		ImGui::SameLine(120);
		ImGui::Text(to_string(info->meshRenderer->mAABBSizeZ).c_str());

		ImGui::Text("Bone Num:        ");
		ImGui::SameLine(); ImGui::Text(to_string(info->boneNum).c_str());

		ImGui::Text("Animations Num:  ");
		ImGui::SameLine(); ImGui::Text(to_string(info->animBriefInfos.size()).c_str());
		for (auto& animBrief : info->animBriefInfos)
		{
			ImGui::Text("    Name:        ");
			ImGui::SameLine(); ImGui::Text(animBrief.name.c_str());
			ImGui::Text("    Duration:    ");
			ImGui::SameLine(); ImGui::Text(to_string(animBrief.duration).c_str());
		}
	}

	void EditorInspectorPanel::DrawAudio(AssetAudioInfo* info)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Audio"))
			return;

		ImGui::Text("Name:");
		ImGui::SameLine(120);
		ImGui::Text(info->name.c_str());

		ImGui::Text("Length:");
		ImGui::SameLine(120);
		ImGui::Text(info->lengthStr.c_str());

		ImGui::Text("Size:");
		ImGui::SameLine(120);
		ImGui::Text(info->sizeStr.c_str());

		static const ImVec2 audioBtnSize = ImVec2(60.0f, 20.0f);
		ImGui::SetCursorPosX(80);
		if (ImGui::Button("Play", audioBtnSize))
		{
			info->audioClip->Play2D();
		}
	}

	void EditorInspectorPanel::DrawBoxCollider(BoxCollider* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Box Collider"))
			return;

		// Friction
		float friction = component->mFriction;
		ImGui::Text("Friction         ");
		ImGui::SameLine(); ImGui::DragFloat("##friction", &friction, 0.01f, 0.0f, FLT_MAX);

		// Bounciness
		float bounciness = component->mBounciness;
		ImGui::Text("Bounciness       ");
		ImGui::SameLine(); ImGui::DragFloat("##bounciness", &bounciness, 0.01f, 0.0f, 1.0f);

		// Combine Type
		const char* items[] = { "Average", "Minimum", "Maximum", "Multiply" };

		// Friction Combine
		static ImGuiComboFlags frictionCombineFlags = 0;
		static int frictionCombineIdx = static_cast<int>(component->mFrictionCombine);
		const char* frictionPreviewValue = items[frictionCombineIdx];
		ImGui::Text("Friction Combine ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##FrictionType", frictionPreviewValue, frictionCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (frictionCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					frictionCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Bounce Combine
		static ImGuiComboFlags bounceCombineFlags = 0;
		static int bounceCombineIdx = static_cast<int>(component->mBounceCombine);
		const char* bouncePreviewValue = items[bounceCombineIdx];
		ImGui::Text("Bounce Combine   ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##BounceType", bouncePreviewValue, bounceCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (bounceCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					bounceCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Size
		Vector3 size = component->mCollider->mHalfSize * 2.0f;
		ImVec4 v = ImVec4(size.x, size.y, size.z, 1.0f);
		ImGui::Text("Size             ");
		ImGui::SameLine(); ImGui::DragFloat3("##size", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);
	}

	void EditorInspectorPanel::DrawPlaneCollider(PlaneCollider* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Plane Collider"))
			return;

		// Friction
		float friction = component->mFriction;
		ImGui::Text("Friction         ");
		ImGui::SameLine(); ImGui::DragFloat("##friction", &friction, 0.01f, 0.0f, FLT_MAX);

		// Bounciness
		float bounciness = component->mBounciness;
		ImGui::Text("Bounciness       ");
		ImGui::SameLine(); ImGui::DragFloat("##bounciness", &bounciness, 0.01f, 0.0f, 1.0f);

		// Combine Type
		const char* items[] = { "Average", "Minimum", "Maximum", "Multiply" };

		// Friction Combine
		static ImGuiComboFlags frictionCombineFlags = 0;
		static int frictionCombineIdx = static_cast<int>(component->mFrictionCombine);
		const char* frictionPreviewValue = items[frictionCombineIdx];
		ImGui::Text("Friction Combine ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##FrictionType", frictionPreviewValue, frictionCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (frictionCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					frictionCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Bounce Combine
		static ImGuiComboFlags bounceCombineFlags = 0;
		static int bounceCombineIdx = static_cast<int>(component->mBounceCombine);
		const char* bouncePreviewValue = items[bounceCombineIdx];
		ImGui::Text("Bounce Combine   ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##BounceType", bouncePreviewValue, bounceCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (bounceCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					bounceCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Normal
		Vector3 normal = component->mCollider->mNormal;
		ImVec4 v = ImVec4(normal.x, normal.y, normal.z, 1.0f);
		ImGui::Text("Normal           ");
		ImGui::SameLine(); ImGui::DragFloat3("##normal", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);

		// Distance
		float distance = component->mCollider->mDistance;
		ImGui::Text("Distance         ");
		ImGui::SameLine(); ImGui::DragFloat("##distance", &distance, 0.01f, -FLT_MAX, FLT_MAX);
	}

	void EditorInspectorPanel::DrawSphereCollider(SphereCollider* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Sphere Collider"))
			return;

		// Friction
		float friction = component->mFriction;
		ImGui::Text("Friction         ");
		ImGui::SameLine(); ImGui::DragFloat("##friction", &friction, 0.01f, 0.0f, FLT_MAX);

		// Bounciness
		float bounciness = component->mBounciness;
		ImGui::Text("Bounciness       ");
		ImGui::SameLine(); ImGui::DragFloat("##bounciness", &bounciness, 0.01f, 0.0f, 1.0f);

		// Combine Type
		const char* items[] = { "Average", "Minimum", "Maximum", "Multiply" };

		// Friction Combine
		static ImGuiComboFlags frictionCombineFlags = 0;
		static int frictionCombineIdx = static_cast<int>(component->mFrictionCombine);
		const char* frictionPreviewValue = items[frictionCombineIdx];
		ImGui::Text("Friction Combine ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##FrictionType", frictionPreviewValue, frictionCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (frictionCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					frictionCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Bounce Combine
		static ImGuiComboFlags bounceCombineFlags = 0;
		static int bounceCombineIdx = static_cast<int>(component->mBounceCombine);
		const char* bouncePreviewValue = items[bounceCombineIdx];
		ImGui::Text("Bounce Combine   ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##BounceType", bouncePreviewValue, bounceCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (bounceCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					bounceCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Radius
		float radius = component->mCollider->mRadius;
		ImGui::Text("Radius           ");
		ImGui::SameLine(); ImGui::DragFloat("##radius", &radius, 0.01f, 0.0f, FLT_MAX);
	}

	void EditorInspectorPanel::DrawCircle2DCollider(Circle2DCollider* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Circle 2D Collider"))
			return;

		// Friction
		float friction = component->mFriction;
		ImGui::Text("Friction         ");
		ImGui::SameLine(); ImGui::DragFloat("##friction", &friction, 0.01f, 0.0f, FLT_MAX);

		// Bounciness
		float bounciness = component->mBounciness;
		ImGui::Text("Bounciness       ");
		ImGui::SameLine(); ImGui::DragFloat("##bounciness", &bounciness, 0.01f, 0.0f, 1.0f);

		// Combine Type
		const char* items[] = { "Average", "Minimum", "Maximum", "Multiply" };

		// Friction Combine
		static ImGuiComboFlags frictionCombineFlags = 0;
		static int frictionCombineIdx = static_cast<int>(component->mFrictionCombine);
		const char* frictionPreviewValue = items[frictionCombineIdx];
		ImGui::Text("Friction Combine ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##FrictionType", frictionPreviewValue, frictionCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (frictionCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					frictionCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Bounce Combine
		static ImGuiComboFlags bounceCombineFlags = 0;
		static int bounceCombineIdx = static_cast<int>(component->mBounceCombine);
		const char* bouncePreviewValue = items[bounceCombineIdx];
		ImGui::Text("Bounce Combine   ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##BounceType", bouncePreviewValue, bounceCombineFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool is_selected = (bounceCombineIdx == i);
				if (ImGui::Selectable(items[i], is_selected))
					bounceCombineIdx = i;
			}
			ImGui::EndCombo();
		}

		// Normal
		Vector3 normal = component->mCollider->mWorldNormal;
		ImVec4 v = ImVec4(normal.x, normal.y, normal.z, 1.0f);
		ImGui::Text("Normal           ");
		ImGui::SameLine(); ImGui::DragFloat3("##normal", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);
	}

	void EditorInspectorPanel::DrawRigidBody(ZRigidBody* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("RigidBody"))
			return;

		// Mass
		float mass = component->mRigidBody->GetMass();
		ImGui::Text("Mass             ");
		ImGui::SameLine(); ImGui::DragFloat("##mass", &mass, 0.01f, 0.0f, FLT_MAX);

		// Linear Damping
		float linearDamping = 1.0f - component->mRigidBody->GetLinearDamping();
		ImGui::Text("Linear Damping   ");
		ImGui::SameLine(); ImGui::DragFloat("##linearDamping", &linearDamping, 0.01f, 0.0f, 1.0f);

		// Angular Damping
		float angularDamping = 1.0f - component->mRigidBody->GetAngularDamping();
		ImGui::Text("Angular Damping  ");
		ImGui::SameLine(); ImGui::DragFloat("##angularDamping", &angularDamping, 0.01f, 0.0f, 1.0f);

		// Use Gravity
		bool useGravity = component->mUseGravity;
		ImGui::Text("Use Gravity      ");
		ImGui::SameLine(); ImGui::Checkbox("##useGravity", &useGravity);
	}

	void EditorInspectorPanel::DrawAnimator(Animator* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Animator"))
			return;

		const string& avatarName = component->mAvatarName;
		ImGui::Text("Avatar           ");
		ImGui::SameLine(); ImGui::Text(avatarName.c_str());
	}

	void EditorInspectorPanel::DrawSpringJoint(SpringJoint* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Spring Joint"))
			return;

		// Connected Body
		string connectedBodyPath = component->mConnectedGOPath;
		ImGui::Text("Connected Body   ");
		ImGui::SameLine(); ImGui::Text(connectedBodyPath.c_str());

		// Anchor
		Vector3 anchor = component->mAnchor;
		ImVec4 v = ImVec4(anchor.x, anchor.y, anchor.z, 1.0f);
		ImGui::Text("Anchor           ");
		ImGui::SameLine(); ImGui::DragFloat3("##anchor", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);

		// Connected Anchor
		Vector3 connectedAnchor = component->mOtherAnchor;
		ImVec4 c = ImVec4(connectedAnchor.x, connectedAnchor.y, connectedAnchor.z, 1.0f);
		ImGui::Text("Connected Anchor ");
		ImGui::SameLine(); ImGui::DragFloat3("##connectedAnchor", (float*)&c, 0.01f, -FLT_MAX, FLT_MAX);

		// Rest Length
		float restLength = component->mRestLength;
		ImGui::Text("Rest Length      ");
		ImGui::SameLine(); ImGui::DragFloat("##restLength", &restLength, 0.01f, 0.0f, FLT_MAX);

		// Spring Constant
		float springConstant = component->mSpringConstant;
		ImGui::Text("Spring Constant  ");
		ImGui::SameLine(); ImGui::DragFloat("##springConstant", &springConstant, 0.01f, 0.0f, FLT_MAX);
	}

	void EditorInspectorPanel::DrawDistanceJoint(ZDistanceJoint* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Distance Joint"))
			return;

		// Connected Body
		string connectedBodyPath = component->mConnectedGOPath;
		ImGui::Text("Connected Body   ");
		ImGui::SameLine(); ImGui::Text(connectedBodyPath.c_str());

		// Anchor
		Vector3 anchor = component->mAnchor;
		ImVec4 v = ImVec4(anchor.x, anchor.y, anchor.z, 1.0f);
		ImGui::Text("Anchor           ");
		ImGui::SameLine(); ImGui::DragFloat3("##anchor", (float*)&v, 0.01f, -FLT_MAX, FLT_MAX);

		// Connected Anchor
		Vector3 connectedAnchor = component->mOtherAnchor;
		ImVec4 c = ImVec4(connectedAnchor.x, connectedAnchor.y, connectedAnchor.z, 1.0f);
		ImGui::Text("Connected Anchor ");
		ImGui::SameLine(); ImGui::DragFloat3("##connectedAnchor", (float*)&c, 0.01f, -FLT_MAX, FLT_MAX);

		// Distance
		float distance = component->mDistance;
		ImGui::Text("Distance         ");
		ImGui::SameLine(); ImGui::DragFloat("##distance", &distance, 0.01f, 0.0f, FLT_MAX);
	}

	void EditorInspectorPanel::DrawCloth(Cloth* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Cloth"))
			return;

		// Mass
		float mass = component->mMass;
		ImGui::Text("Mass             ");
		ImGui::SameLine(); ImGui::DragFloat("##mass", &mass, 0.01f, 0.0f, FLT_MAX);
		
		// Friction
		float friction = component->mFriction;
		ImGui::Text("Friction         ");
		ImGui::SameLine(); ImGui::DragFloat("##friction", &friction, 0.01f, 0.0f, FLT_MAX);

		// Bend Stiffness
		float bendStiffness = component->mBendStiffness;
		ImGui::Text("Bend Stiffness   ");
		ImGui::SameLine(); ImGui::DragFloat("##bendStiffness", &bendStiffness, 0.01f, 0.0f, FLT_MAX);

		// Stretch Stiffness
		float stretchStiffness = component->mStretchStiffness;
		ImGui::Text("Stretch Stiffness");
		ImGui::SameLine(); ImGui::DragFloat("##stretchStiffness", &stretchStiffness, 0.01f, 0.0f, FLT_MAX);

		// Use Gravity
		bool useGravity = component->mUseGravity;
		ImGui::Text("Use Gravity      ");
		ImGui::SameLine(); ImGui::Checkbox("##useGravity", &useGravity);
	}

	void EditorInspectorPanel::DrawAudioSource(AudioSource* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Audio Source"))
			return;

		// Clip
		string clipName = component->mName;
		ImGui::Text("Clip             ");
		ImGui::SameLine(); ImGui::Text(clipName.c_str());

		// Volume
		float volume = component->GetVolume();
		ImGui::Text("Volume           ");
		ImGui::SameLine(); ImGui::DragFloat("##volume", &volume, 0.01f, 0.0f, 1.0f);

		// Loop
		bool loop = component->GetLoop();
		ImGui::Text("Loop             ");
		ImGui::SameLine(); ImGui::Checkbox("##loop", &loop);

		// Play On Awake
		bool playOnAwake = component->mPlayOnAwake;
		ImGui::Text("Play On Awake    ");
		ImGui::SameLine(); ImGui::Checkbox("##playOnAwake", &playOnAwake);

		// 3D Sound
		bool is3DSound = component->GetIs3D();
		ImGui::Text("3D Sound         ");
		ImGui::SameLine(); ImGui::Checkbox("##3DSound", &is3DSound);

		// Min Distance
		float minDistance = component->GetMinDistance();
		ImGui::Text("Min Distance     ");
		ImGui::SameLine(); ImGui::DragFloat("##minDistance", &minDistance, 0.01f, 0.0f, FLT_MAX);

		// Max Distance
		float maxDistance = component->GetMaxDistance();
		ImGui::Text("Max Distance     ");
		ImGui::SameLine(); ImGui::DragFloat("##maxDistance", &maxDistance, 0.01f, 0.0f, FLT_MAX);
	}

	void EditorInspectorPanel::DrawAudioListener(AudioListener* component)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Audio Listener"))
			return;
	}
}