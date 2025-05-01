#include "ImGuiTextureManagerMetal.h"
#include "../RenderAPIMetal.h"

namespace ZXEngine
{
	bool ImGuiTextureManagerMetal::CheckExistenceByEngineID(uint32_t id)
	{
		if (allTextures.count(id))
			return true;
		else
			return false;
	}

	ImTextureID ImGuiTextureManagerMetal::GetImTextureIDByEngineID(uint32_t id)
	{
		return allTextures[id].ImGuiID;
	}

	ImGuiTextureIndex ImGuiTextureManagerMetal::LoadTexture(const string& path)
	{
		auto renderAPI = reinterpret_cast<RenderAPIMetal*>(RenderAPI::GetInstance());
		int width = 0, height = 0;
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = renderAPI->LoadTexture(path.c_str(), width, height);
		auto mtTexture = renderAPI->GetTextureByIndex(newImGuiTexture.EngineID);
		newImGuiTexture.ImGuiID = (ImTextureID)(intptr_t)mtTexture->texture;
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;
		return newImGuiTexture;
	}

	void ImGuiTextureManagerMetal::DeleteTexture(ImGuiTextureIndex textureIndex)
	{
		RenderAPI::GetInstance()->DeleteTexture(textureIndex.EngineID);
		allTextures.erase(textureIndex.EngineID);
	}

	ImGuiTextureIndex ImGuiTextureManagerMetal::CreateFromEngineID(uint32_t id)
	{
		auto renderAPI = reinterpret_cast<RenderAPIMetal*>(RenderAPI::GetInstance());
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = id;
		auto mtTexture = renderAPI->GetTextureByIndex(newImGuiTexture.EngineID);
		newImGuiTexture.ImGuiID = (ImTextureID)(intptr_t)mtTexture->texture;
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;
		return newImGuiTexture;
	}

	void ImGuiTextureManagerMetal::DeleteByEngineID(uint32_t id)
	{
		allTextures.erase(id);
	}
}