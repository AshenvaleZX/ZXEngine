#include "ImGuiTextureManagerD3D12.h"
#include "EditorGUIManagerDirectX12.h"
#include "../RenderAPI.h"
#include "../RenderAPID3D12.h"
#include "../DirectX12/ZXD3D12DescriptorManager.h"

namespace ZXEngine
{
	bool ImGuiTextureManagerD3D12::CheckExistenceByEngineID(uint32_t id)
	{
		if (allTextures.count(id))
			return true;
		else
			return false;
	}

	ImTextureID ImGuiTextureManagerD3D12::GetImTextureIDByEngineID(uint32_t id)
	{
		return allTextures[id].ImGuiID;
	}

	ImGuiTextureIndex ImGuiTextureManagerD3D12::LoadTexture(const string& path)
	{
		auto renderAPI = reinterpret_cast<RenderAPID3D12*>(RenderAPI::GetInstance());
		auto editorGUIMgr = reinterpret_cast<EditorGUIManagerDirectX12*>(EditorGUIManager::GetInstance());

		int width = 0, height = 0;
		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = renderAPI->LoadTexture(path.c_str(), width, height);

		auto zxD3D12Texture = renderAPI->GetTextureByIndex(newImGuiTexture.EngineID);
		auto cpuHandle = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(zxD3D12Texture->handleSRV);

		UINT curOffset = editorGUIMgr->GetNextAvailablePos();
		CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicCPUHandle(editorGUIMgr->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		dynamicCPUHandle.Offset(curOffset, editorGUIMgr->descriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE dynamicGPUHandle(editorGUIMgr->descriptorHeap->GetGPUDescriptorHandleForHeapStart());
		dynamicGPUHandle.Offset(curOffset, editorGUIMgr->descriptorSize);

		renderAPI->mD3D12Device->CopyDescriptorsSimple(1, dynamicCPUHandle, cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		editorGUIMgr->descriptorUseState[curOffset] = true;

		newImGuiTexture.idx = curOffset;
		newImGuiTexture.ImGuiID = (ImTextureID)dynamicGPUHandle.ptr;
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;

		return newImGuiTexture;
	}

	void ImGuiTextureManagerD3D12::DeleteTexture(ImGuiTextureIndex textureIndex)
	{
		auto editorGUIMgr = reinterpret_cast<EditorGUIManagerDirectX12*>(EditorGUIManager::GetInstance());

		editorGUIMgr->descriptorUseState[textureIndex.idx] = false;
		RenderAPI::GetInstance()->DeleteTexture(textureIndex.EngineID);
		allTextures.erase(textureIndex.EngineID);
	}

	ImGuiTextureIndex ImGuiTextureManagerD3D12::CreateFromEngineID(uint32_t id)
	{
		auto renderAPI = reinterpret_cast<RenderAPID3D12*>(RenderAPI::GetInstance());
		auto editorGUIMgr = reinterpret_cast<EditorGUIManagerDirectX12*>(EditorGUIManager::GetInstance());

		ImGuiTextureIndex newImGuiTexture = {};
		newImGuiTexture.EngineID = id;

		auto zxD3D12Texture = renderAPI->GetTextureByIndex(newImGuiTexture.EngineID);
		auto cpuHandle = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(zxD3D12Texture->handleSRV);

		UINT curOffset = editorGUIMgr->GetNextAvailablePos();
		CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicCPUHandle(editorGUIMgr->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		dynamicCPUHandle.Offset(curOffset, editorGUIMgr->descriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE dynamicGPUHandle(editorGUIMgr->descriptorHeap->GetGPUDescriptorHandleForHeapStart());
		dynamicGPUHandle.Offset(curOffset, editorGUIMgr->descriptorSize);

		renderAPI->mD3D12Device->CopyDescriptorsSimple(1, dynamicCPUHandle, cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		editorGUIMgr->descriptorUseState[curOffset] = true;

		newImGuiTexture.idx = curOffset;
		newImGuiTexture.ImGuiID = (ImTextureID)dynamicGPUHandle.ptr;
		allTextures[newImGuiTexture.EngineID] = newImGuiTexture;

		return newImGuiTexture;
	}

	void ImGuiTextureManagerD3D12::DeleteByEngineID(uint32_t id)
	{
		if (!CheckExistenceByEngineID(id))
			return;

		auto editorGUIMgr = reinterpret_cast<EditorGUIManagerDirectX12*>(EditorGUIManager::GetInstance());

		editorGUIMgr->descriptorUseState[allTextures[id].idx] = false;
		allTextures.erase(id);
	}
}