#pragma once
#include "ImGuiTextureManager.h"

namespace ZXEngine
{
	class ImGuiTextureManagerVulkan : public ImGuiTextureManager
	{
	public:
		virtual bool CheckExistenceByEngineID(uint32_t id);
		virtual ImTextureID GetImTextureIDByEngineID(uint32_t id);

		virtual ImGuiTextureIndex LoadTexture(const string& path);
		virtual void DeleteTexture(ImGuiTextureIndex textureIndex);

		virtual ImGuiTextureIndex CreateFromEngineID(uint32_t id);
		virtual void DeleteByEngineID(uint32_t id);

	private:
		unordered_map<uint32_t, ImGuiTextureIndex> allTextures;
	};
}