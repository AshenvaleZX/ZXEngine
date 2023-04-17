#pragma once
#include "RenderAPI.h"
#include "D3D12EnumStruct.h"

namespace ZXEngine
{
	class RenderAPID3D12 : public RenderAPI
	{
		/// <summary>
		/// 标准RenderAPI接口
		/// </summary>
	public:
		RenderAPID3D12();
		~RenderAPID3D12() {};

		virtual void BeginFrame();
		virtual void EndFrame();


		/// <summary>
		/// 仅启动时一次性初始化的核心D3D12组件及相关变量
		/// </summary>
	private:
		const UINT mSwapChainBufferCount = 2;

		// 各类型描述符大小
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		// 屏幕后台缓冲区图像格式
		DXGI_FORMAT mPresentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 4X MSAA质量等级
		UINT m4xMSAAQuality = 0;
		UINT msaaSamplesCount = 4;

		UINT64 mCurrentFence = 0;
		ComPtr<ID3D12Fence> mFence;

		ComPtr<IDXGIFactory4> mDXGIFactory;
		ComPtr<ID3D12Device> mD3D12Device;
		ComPtr<IDXGISwapChain> mSwapChain;

		vector<D3D12Command> mCommands;
		ComPtr<ID3D12CommandQueue> mCommandQueue;

		void InitD3D12();
		void GetDeviceProperties();
		void CreateCommandResources();
		void CreateSwapChain();


		/// <summary>
		/// D3D12资源，以及相关创建销毁接口
		/// </summary>
	private:
	};
}