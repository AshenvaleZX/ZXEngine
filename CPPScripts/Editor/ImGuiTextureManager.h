#pragma once
#include "../pubh.h"
#include "../External/ImGui/imgui.h"
// ImGui图像渲染: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples

namespace ZXEngine
{
	struct ImGuiTextureIndex
	{
		uint32_t idx = UINT32_MAX;
		uint32_t EngineID = UINT32_MAX;
		ImTextureID ImGuiID = nullptr;
	};

	class ImGuiTextureManager
	{
	public:
		static void Creat();
		static ImGuiTextureManager* GetInstance();
	private:
		static ImGuiTextureManager* mInstance;

	public:
		virtual bool CheckExistenceByEngineID(uint32_t id) = 0;
		virtual ImTextureID GetImTextureIDByEngineID(uint32_t id) = 0;

		virtual ImGuiTextureIndex LoadTexture(const string& path) = 0;
		virtual void DeleteTexture(ImGuiTextureIndex textureIndex) = 0;

		virtual ImGuiTextureIndex CreateFromEngineID(uint32_t id) = 0;
		virtual void DeleteByEngineID(uint32_t id) = 0;
	};
}