#pragma once
#include "../pubh.h"
#include "D3D12EnumStruct.h"

namespace ZXEngine
{
	struct ZXD3D12DescriptorHeap
	{
		// ��ǰʣ���������������
		UINT availableNum = 0;
		// ���������е�����������
		UINT descriptorNum = 0;
		// ÿ���������Ƿ�����ʹ��
		vector<bool> inUseState;
		// ��������
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	};

	/// <summary>
	/// ���ฺ���������ΪmHeapType��CPU�������ѣ����д����͵�CPU��������������ͳһ���������
	/// </summary>
	class ZXD3D12DescriptorAllocator
	{
	public:
		ZXD3D12DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptorNumPerHeap = 256);

		ZXD3D12DescriptorHandle CreateDescriptor(const D3D12_SAMPLER_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ZXD3D12DescriptorHandle handle);

		void ReleaseDescriptor(ZXD3D12DescriptorHandle handle);

	private:
		UINT mDescriptorSize;
		UINT mDescriptorNumPerHeap;
		ComPtr<ID3D12Device> mD3D12Device;
		D3D12_DESCRIPTOR_HEAP_TYPE mHeapType;
		vector<ZXD3D12DescriptorHeap*> mDescriptorHeaps;

		void CreateNewHeap();
		uint32_t GetNextAvailablePos(const vector<bool>& inUseState);
		ZXD3D12DescriptorHandle GetNextDescriptorHeapAndHandle(ZXD3D12DescriptorHeap*& zxDescriptorHeap);
	};
}