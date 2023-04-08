#include "ImGuiTextureManagerVulkan.h"
#include "../RenderAPI.h"
#include "../RenderAPIVulkan.h"
#include "../ImGui/imgui_impl_vulkan.h"

namespace ZXEngine
{
	bool ImGuiTextureManagerVulkan::CheckExistenceByEngineID(uint32_t id)
	{
		if (allTextures.count(id))
			return true;
		else
			return false;
	}

	ImTextureID ImGuiTextureManagerVulkan::GetImTextureIDByEngineID(uint32_t id)
	{
		return allTextures[id].ImGuiID;
	}

	ImGuiTextureIndex ImGuiTextureManagerVulkan::LoadTexture(const string & path)
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());
		int width = 0, height = 0;
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = renderAPI->LoadTexture(path.c_str(), width, height);
		auto vulkanTexture = renderAPI->GetTextureByIndex(newImGuiTexture.EngineID);
		newImGuiTexture.ImGuiID = ImGui_ImplVulkan_AddTexture(vulkanTexture->sampler, vulkanTexture->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;
		return newImGuiTexture;
	}

	void ImGuiTextureManagerVulkan::DeleteTexture(ImGuiTextureIndex textureIndex)
	{
		ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(textureIndex.ImGuiID));
		RenderAPI::GetInstance()->DeleteTexture(textureIndex.EngineID);
		allTextures.erase(textureIndex.EngineID);
	}

	ImGuiTextureIndex ImGuiTextureManagerVulkan::CreateFromEngineID(uint32_t id)
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = id;
		auto vulkanTexture = renderAPI->GetTextureByIndex(newImGuiTexture.EngineID);
		newImGuiTexture.ImGuiID = ImGui_ImplVulkan_AddTexture(vulkanTexture->sampler, vulkanTexture->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;
		return newImGuiTexture;
	}

	void ImGuiTextureManagerVulkan::DeleteByEngineID(uint32_t id)
	{
		ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(allTextures[id].ImGuiID));
		allTextures.erase(id);
	}
}