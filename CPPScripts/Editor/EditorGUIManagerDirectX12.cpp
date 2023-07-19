#include "EditorGUIManagerDirectX12.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorConsolePanel.h"
#include "EditorAssetPreviewer.h"
#include "../RenderAPID3D12.h"
#include "../Window/WindowManager.h"
#include "../DirectX12/ZXD3D12DescriptorManager.h"
#include "../External/ImGui/imgui_impl_dx12.h"
#include "../External/ImGui/imgui_impl_win32.h"

namespace ZXEngine
{
	EditorGUIManagerDirectX12::EditorGUIManagerDirectX12()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		InitForDirectX12();
	}

	EditorGUIManagerDirectX12::~EditorGUIManagerDirectX12()
	{
		auto renderAPI = reinterpret_cast<RenderAPID3D12*>(RenderAPI::GetInstance());

		renderAPI->WaitForRenderFinish();

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		descriptorHeap.Reset();
	}

	void EditorGUIManagerDirectX12::Init()
	{
		// Inspector的绘制要放在Hierarchy和Project后面，因为这两个面板会决定Inspector的内容
		allPanels.push_back(new EditorProjectPanel());
		allPanels.push_back(new EditorMainBarPanel());
		allPanels.push_back(new EditorHierarchyPanel());
		allPanels.push_back(new EditorInspectorPanel());
		allPanels.push_back(new EditorConsolePanel());
		assetPreviewer = new EditorAssetPreviewer();
	}

	void EditorGUIManagerDirectX12::BeginEditorRender()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManagerDirectX12::EditorRender()
	{
		if (assetPreviewer->Check())
			assetPreviewer->Draw();

		for (auto panel : allPanels)
		{
			panel->DrawPanel();
		}
		ImGui::Render();
	}

	void EditorGUIManagerDirectX12::EndEditorRender()
	{
		auto renderAPI = reinterpret_cast<RenderAPID3D12*>(RenderAPI::GetInstance());

		commandAllocators[renderAPI->mCurrentFrame]->Reset();
		commandList->Reset(commandAllocators[renderAPI->mCurrentFrame].Get(), NULL);

		auto presentFBO = renderAPI->GetFBOByIndex(renderAPI->mPresentFBOIdx);
		auto colorBuffer = renderAPI->GetTextureByIndex(renderAPI->GetRenderBufferByIndex(presentFBO->colorBufferIdx)->renderBuffers[renderAPI->mCurPresentIdx]);
		
		auto toRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &toRenderTarget);

		auto rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
		commandList->OMSetRenderTargets(1, &rtv, FALSE, NULL);

		ID3D12DescriptorHeap* curDescriptorHeaps[] = { descriptorHeap.Get() };
		commandList->SetDescriptorHeaps(1, curDescriptorHeaps);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &toPresent);

		commandList->Close();

		ID3D12CommandList* cmdsLists[] = { commandList.Get() };
		renderAPI->mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		renderAPI->SignalFence(renderAPI->mFrameFences[renderAPI->mCurrentFrame]);
	}

	void EditorGUIManagerDirectX12::ResetPanels()
	{
		for (auto panel : allPanels)
			panel->ResetPanel();
	}

	void EditorGUIManagerDirectX12::OnWindowSizeChange()
	{

	}

	void EditorGUIManagerDirectX12::InitForDirectX12()
	{
		auto renderAPI = reinterpret_cast<RenderAPID3D12*>(RenderAPI::GetInstance());

		ImGui_ImplWin32_Init(static_cast<HWND>(WindowManager::GetInstance()->GetWindow()));

		// 这里创建的描述符堆，第一个描述符是固定给ImGui的文本Texture用的，剩下的是给图片渲染用的
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = maxDescriptorNum;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (renderAPI->mD3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)) != S_OK)
			Debug::LogError("Create descriptor heap for ImGui failed!");
		descriptorUseState.resize(maxDescriptorNum, false);
		descriptorUseState[0] = true;
		descriptorSize = renderAPI->mCbvSrvUavDescriptorSize;

		ImGui_ImplDX12_Init(renderAPI->mD3D12Device.Get(), DX_MAX_FRAMES_IN_FLIGHT, renderAPI->mDefaultImageFormat, descriptorHeap.Get(),
			descriptorHeap->GetCPUDescriptorHandleForHeapStart(), descriptorHeap->GetGPUDescriptorHandleForHeapStart());

		commandAllocators.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (renderAPI->mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])) != S_OK)
				Debug::LogError("Create command allocator for ImGui failed!");
		}
		renderAPI->mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList));
		commandList->Close();
	}

	UINT EditorGUIManagerDirectX12::GetNextAvailablePos()
	{
		for (UINT i = 0; i < descriptorUseState.size(); i++)
			if (!descriptorUseState[i])
				return i;

		Debug::LogError("No available descriptor handle for ImGuiTextureManager!");
		return UINT32_MAX;
	}
}