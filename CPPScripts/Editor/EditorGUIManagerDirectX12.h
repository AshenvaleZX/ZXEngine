#pragma once
#include "EditorGUIManager.h"
#include "../DirectX12/D3D12EnumStruct.h"

namespace ZXEngine
{
	class EditorGUIManagerDirectX12 : public EditorGUIManager
	{
		friend class ImGuiTextureManagerD3D12;
	public:
		EditorGUIManagerDirectX12();
		~EditorGUIManagerDirectX12();

		virtual void Init();
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();
		virtual void ResetPanels();
		virtual void OnWindowSizeChange();

	private:
		vector<EditorPanel*> allPanels;

		const uint32_t maxDescriptorNum = 256;

		UINT descriptorSize;
		vector<bool> descriptorUseState;
		ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
		ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		vector<ComPtr<ID3D12CommandAllocator>> commandAllocators;

		void InitForDirectX12();
		UINT GetNextAvailablePos();
	};
}