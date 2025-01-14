#include "EditorMainBarPanel.h"
#include "EditorDataManager.h"
#include "EditorGUIManager.h"
#include "EditorDialogBoxManager.h"
#include "../GameLogicManager.h"
#include "../Time.h"
#include "../SceneManager.h"
#include "../Resources.h"
#include "../ParticleSystem/ParticleSystemManager.h"
#include "../Vulkan/SPIRVCompiler.h"
#include "../Component/Animator.h"
#include "../Audio/AudioEngine.h"

#ifdef ZX_PLATFORM_WINDOWS
#include "../DirectX12/ZXD3D12Util.h"
#endif

namespace ZXEngine
{
	EditorMainBarPanel::EditorMainBarPanel()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4 btnColor = style.Colors[ImGuiCol_Button];
		selectBtnColor = ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f);
		selectTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void EditorMainBarPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::mainBarWidth, (float)ProjectSetting::mainBarHeight));

		// 设置面板具体内容
		if (ImGui::Begin("ZXEngine", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Scene"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");
					if (ImGui::MenuItem("Open Scene"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");
					if (ImGui::MenuItem("Save Scene"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");

					ImGui::Separator();

					if (ImGui::MenuItem("New Project"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");
					if (ImGui::MenuItem("Open Project"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");
					if (ImGui::MenuItem("Save Project"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Project Settings"))
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is not implemented yet.");

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Assets"))
				{
					if (ImGui::MenuItem("Compile All Shader for Vulkan"))
					{
						std::thread t([]
						{
							SPIRVCompiler::CompileAllShader(Resources::GetAssetsPath());
							SPIRVCompiler::CompileAllShader(Resources::GetAssetFullPath("Shaders", true));
							Debug::Log("The compilation of all shaders is complete.");
						});
						t.detach();
					}

					if (ImGui::MenuItem("Compile All Shader for DirectX12"))
					{
#ifdef ZX_PLATFORM_WINDOWS
						std::thread t([]
						{
							ZXD3D12Util::CompileAllShader(Resources::GetAssetsPath());
							ZXD3D12Util::CompileAllShader(Resources::GetAssetFullPath("Shaders", true));
							Debug::Log("The compilation of all shaders is complete.");
						});
						t.detach();
#else
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is only available on Windows.");
#endif
					}

					if (ImGui::MenuItem("Generate HLSL for DirectX12"))
					{
#ifdef ZX_PLATFORM_WINDOWS
						std::thread t([]
						{
							ZXD3D12Util::TranslateAllShaderToHLSL(Resources::GetAssetsPath());
							ZXD3D12Util::TranslateAllShaderToHLSL(Resources::GetAssetFullPath("Shaders", true));
							Debug::Log("The translation of all shaders is complete.");
						});
						t.detach();
#else
						EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "This feature is only available on Windows.");
#endif
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// 3个按钮从左到右的宽度
			float threeButtonSize = 80.0f;
			float avail = ImGui::GetContentRegionAvail().x;
			float offset = (avail - threeButtonSize) * 0.5f;
			// 计算3个按钮要居中的话，第一个按钮的起始位置
			float midBtnPosX = ImGui::GetCursorPosX() + offset;

			bool isPop = false;
			// Translation
			if (EditorDataManager::GetInstance()->mCurTransType == TransformType::Position)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			if (ImGui::Button("T", buttonSize))
			{
				EditorDataManager::GetInstance()->mCurTransType = TransformType::Position;
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Rotation
			ImGui::SameLine();
			if (EditorDataManager::GetInstance()->mCurTransType == TransformType::Rotation)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			if (ImGui::Button("R", buttonSize))
			{
				EditorDataManager::GetInstance()->mCurTransType = TransformType::Rotation;
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Scale
			ImGui::SameLine();
			if (EditorDataManager::GetInstance()->mCurTransType == TransformType::Scale)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			if (ImGui::Button("S", buttonSize))
			{
				EditorDataManager::GetInstance()->mCurTransType = TransformType::Scale;
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Play
			ImGui::SameLine(midBtnPosX);
			if (EditorDataManager::isGameStart)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			if (ImGui::Button(">>", buttonSize))
			{
				// 关掉游戏运行的时候，重置暂停按钮
				if (EditorDataManager::isGameStart)
				{
					Time::curTime = 0.0f;
					EditorDataManager::isGamePause = false;
					EditorDataManager::GetInstance()->selectedGO = nullptr;
					EditorGUIManager::GetInstance()->ResetPanels();
					SceneManager::GetInstance()->ReloadScene();
				}
				EditorDataManager::isGameStart = !EditorDataManager::isGameStart;
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Pause
			if (EditorDataManager::isGamePause)
			{
				isPop = true;
				ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
			}
			ImGui::SameLine();
			if (ImGui::Button("||", buttonSize))
			{
				EditorDataManager::isGamePause = !EditorDataManager::isGamePause;
				AudioEngine::GetInstance()->SetAllPause(EditorDataManager::isGamePause);
			}
			if (isPop)
			{
				isPop = false;
				ImGui::PopStyleColor(2);
			}

			// Step
			ImGui::SameLine();
			if (ImGui::Button(">|", buttonSize))
			{
				if (EditorDataManager::isGameStart && EditorDataManager::isGamePause)
				{
					Time::UpdateCurTime();
					SceneManager::GetInstance()->GetCurScene()->Update();
				}
			}
		}
		ImGui::End();
	}
}