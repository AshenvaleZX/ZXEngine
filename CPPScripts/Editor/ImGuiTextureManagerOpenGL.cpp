#include "ImGuiTextureManagerOpenGL.h"
#include "../RenderAPI.h"

namespace ZXEngine
{
	bool ImGuiTextureManagerOpenGL::CheckExistenceByEngineID(uint32_t id)
	{
		if (allTextures.count(id))
			return true;
		else
			return false;
	}

	ImTextureID ImGuiTextureManagerOpenGL::GetImTextureIDByEngineID(uint32_t id)
	{
		return allTextures[id].ImGuiID;
	}

	ImGuiTextureIndex ImGuiTextureManagerOpenGL::LoadTexture(const string& path)
	{
		int width = 0, height = 0;
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = RenderAPI::GetInstance()->LoadTexture(path.c_str(), width, height);
		newImGuiTexture.ImGuiID = (ImTextureID)(intptr_t)newImGuiTexture.EngineID;
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;
		return newImGuiTexture;
	}

	void ImGuiTextureManagerOpenGL::DeleteTexture(ImGuiTextureIndex textureIndex)
	{
		RenderAPI::GetInstance()->DeleteTexture(textureIndex.EngineID);
		allTextures.erase(textureIndex.EngineID);
	}

	ImGuiTextureIndex ImGuiTextureManagerOpenGL::CreateFromEngineID(uint32_t id)
	{
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = id;
		newImGuiTexture.ImGuiID = (ImTextureID)(intptr_t)id;
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;
		return newImGuiTexture;
	}

	void ImGuiTextureManagerOpenGL::DeleteByEngineID(uint32_t id)
	{
		allTextures.erase(id);
	}
}