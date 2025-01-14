#pragma once
#include "../pubh.h"
#include "D3D12EnumStruct.h"
#include "ZXD3D12DescriptorAllocator.h"

namespace ZXEngine
{
	/// <summary>
	/// 此类负责对外提供描述符创建和释放接口，内部管理了分别对应4种描述符堆类型的4个ZXD3D12DescriptorAllocator
	/// 内部管理了所有的CPU描述符，GPU描述符只在需要用到的时候从这里的CPU描述符Copy过去
	/// </summary>
	class ZXD3D12DescriptorManager
	{
	public:
		static void Creat();
		static ZXD3D12DescriptorManager* GetInstance();
	private:
		static ZXD3D12DescriptorManager* mInstance;

	public:
		ZXD3D12DescriptorManager();
		~ZXD3D12DescriptorManager();

		ZXD3D12DescriptorHandle CreateDescriptor(const D3D12_SAMPLER_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
		ZXD3D12DescriptorHandle CreateDescriptor(ComPtr<ID3D12Resource> resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ZXD3D12DescriptorHandle handle);

		void ReleaseDescriptor(ZXD3D12DescriptorHandle handle);

	private:
		ZXD3D12DescriptorAllocator* mDescriptorAllocators[4];
	};
}