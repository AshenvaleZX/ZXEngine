#pragma once
#include "../pubh.h"
#include "D3D12EnumStruct.h"
#include "ZXD3D12DescriptorAllocator.h"

namespace ZXEngine
{
	/// <summary>
	/// ���ฺ������ṩ�������������ͷŽӿڣ��ڲ������˷ֱ��Ӧ4�������������͵�4��ZXD3D12DescriptorAllocator
	/// �ڲ����������е�CPU��������GPU������ֻ����Ҫ�õ���ʱ��������CPU������Copy��ȥ
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

		void ReleaseDescriptor(ZXD3D12DescriptorHandle handle);

	private:
		ZXD3D12DescriptorAllocator* mDescriptorAllocators[4];
	};
}