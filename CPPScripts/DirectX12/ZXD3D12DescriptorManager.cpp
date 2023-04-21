#include "ZXD3D12DescriptorManager.h"

namespace ZXEngine
{
	ZXD3D12DescriptorManager* ZXD3D12DescriptorManager::mInstance = nullptr;

	void ZXD3D12DescriptorManager::Creat()
	{
		mInstance = new ZXD3D12DescriptorManager();
	}

	ZXD3D12DescriptorManager* ZXD3D12DescriptorManager::GetInstance()
	{
		return mInstance;
	}

	ZXD3D12DescriptorManager::ZXD3D12DescriptorManager()
	{
		for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
			mDescriptorAllocators[i] = new ZXD3D12DescriptorAllocator(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}

	ZXD3D12DescriptorManager::~ZXD3D12DescriptorManager()
	{
		for (uint32_t i = 0; i < 4; i++)
			delete mDescriptorAllocators[i];
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorManager::CreateDescriptor(const D3D12_SAMPLER_DESC& desc)
	{
		return mDescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER]->CreateDescriptor(desc);
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorManager::CreateDescriptor(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
	{
		return mDescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->CreateDescriptor(desc);
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorManager::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc)
	{
		return mDescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->CreateDescriptor(resource, desc);
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorManager::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
	{
		return mDescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->CreateDescriptor(resource, desc);
	}

	ZXD3D12DescriptorHandle ZXD3D12DescriptorManager::CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
	{
		return mDescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->CreateDescriptor(resource, desc);
	}

	void ZXD3D12DescriptorManager::ReleaseDescriptor(ZXD3D12DescriptorHandle handle)
	{
		mDescriptorAllocators[handle.heapType]->ReleaseDescriptor(handle);
	}
}