#include "ZXD3D12DescriptorAllocator.h"
#include "../RenderAPID3D12.h"

namespace ZXEngine
{
	ZXD3D12DescriptorAllocator::ZXD3D12DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptorNumPerHeap)
	{
		mHeapType = type;
		mDescriptorNumPerHeap = descriptorNumPerHeap;

		mD3D12Device = static_cast<RenderAPID3D12*>(RenderAPI::GetInstance())->mD3D12Device;
		mDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(mHeapType);

		CreateNewHeap();
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::GetNextDescriptorHeapAndHandle(ZXD3D12DescriptorHeap*& zxDescriptorHeap)
	{
		ZXD3D12DescriptorHandle handle;
		handle.heapType = mHeapType;

		// 找到第一个有空余位置的描述符堆，同时录这个空余位置
		for (size_t i = 0; i < mDescriptorHeaps.size(); i++)
		{
			if (mDescriptorHeaps[i]->availableNum > 0)
			{
				zxDescriptorHeap = mDescriptorHeaps[i];
				handle.descriptorIdx = GetNextAvailablePos(zxDescriptorHeap->inUseState);
				handle.descriptorHeapIdx = static_cast<uint32_t>(i);
				break;
			}
		}

		// 如果没有找到，就创建一个新的描述符堆
		if (zxDescriptorHeap == nullptr)
		{
			CreateNewHeap();
			zxDescriptorHeap = mDescriptorHeaps.back();
			handle.descriptorIdx = 0;
			handle.descriptorHeapIdx = static_cast<uint32_t>(mDescriptorHeaps.size() - 1);
		}

		return handle;
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::CreateDescriptor(const D3D12_SAMPLER_DESC& desc)
	{
		ZXD3D12DescriptorHeap* zxDescriptorHeap = nullptr;
		auto handle = GetNextDescriptorHeapAndHandle(zxDescriptorHeap);

		// 创建Sampler
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		mD3D12Device->CreateSampler(&desc, descriptorHandle);

		zxDescriptorHeap->availableNum--;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = true;

		return handle;
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::CreateDescriptor(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
	{
		ZXD3D12DescriptorHeap* zxDescriptorHeap = nullptr;
		auto handle = GetNextDescriptorHeapAndHandle(zxDescriptorHeap);

		// 创建CBV
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		mD3D12Device->CreateConstantBufferView(&desc, descriptorHandle);

		zxDescriptorHeap->availableNum--;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = true;

		return handle;
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc)
	{
		ZXD3D12DescriptorHeap* zxDescriptorHeap = nullptr;
		auto handle = GetNextDescriptorHeapAndHandle(zxDescriptorHeap);

		// 创建RTV
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		mD3D12Device->CreateRenderTargetView(resource.Get(), &desc, descriptorHandle);

		zxDescriptorHeap->availableNum--;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = true;

		return handle;
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
	{
		ZXD3D12DescriptorHeap* zxDescriptorHeap = nullptr;
		auto handle = GetNextDescriptorHeapAndHandle(zxDescriptorHeap);

		// 创建DSV
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		mD3D12Device->CreateDepthStencilView(resource.Get(), &desc, descriptorHandle);

		zxDescriptorHeap->availableNum--;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = true;

		return handle;
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
	{
		ZXD3D12DescriptorHeap* zxDescriptorHeap = nullptr;
		auto handle = GetNextDescriptorHeapAndHandle(zxDescriptorHeap);

		// 创建SRV
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		mD3D12Device->CreateShaderResourceView(resource.Get(), &desc, descriptorHandle);

		zxDescriptorHeap->availableNum--;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = true;

		return handle;
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorAllocator::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
	{
		ZXD3D12DescriptorHeap* zxDescriptorHeap = nullptr;
		auto handle = GetNextDescriptorHeapAndHandle(zxDescriptorHeap);

		// 创建UAV
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		mD3D12Device->CreateUnorderedAccessView(resource.Get(), nullptr, &desc, descriptorHandle);

		zxDescriptorHeap->availableNum--;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = true;

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE ZXD3D12DescriptorAllocator::GetCPUDescriptorHandle(ZXD3D12DescriptorHandle handle)
	{
		auto zxDescriptorHeap = mDescriptorHeaps[handle.descriptorHeapIdx];
		CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(zxDescriptorHeap->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		heapHandle.Offset(handle.descriptorIdx, mDescriptorSize);
		return heapHandle;
	}

	void ZXD3D12DescriptorAllocator::ReleaseDescriptor(ZXD3D12DescriptorHandle handle)
	{
		auto zxDescriptorHeap = mDescriptorHeaps[handle.descriptorHeapIdx];
		zxDescriptorHeap->availableNum++;
		zxDescriptorHeap->inUseState[handle.descriptorIdx] = false;
	}

	void ZXD3D12DescriptorAllocator::CreateNewHeap()
	{
		auto newHeap = new ZXD3D12DescriptorHeap();
		newHeap->availableNum = mDescriptorNumPerHeap;
		newHeap->descriptorNum = mDescriptorNumPerHeap;
		newHeap->inUseState.resize(mDescriptorNumPerHeap, false);

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = mDescriptorNumPerHeap;
		heapDesc.Type = mHeapType;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		mD3D12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&newHeap->descriptorHeap));

		mDescriptorHeaps.push_back(newHeap);
	}

	uint32_t ZXD3D12DescriptorAllocator::GetNextAvailablePos(const vector<bool>& inUseState)
	{
		for (uint32_t i = 0; i < inUseState.size(); i++)
			if (!inUseState[i])
				return i;

		return UINT32_MAX;
	}
}