#pragma once
#include "RenderAPI.h"
#include "D3D12EnumStruct.h"

namespace ZXEngine
{
	class RenderAPID3D12 : public RenderAPI
	{
		/// <summary>
		/// ��׼RenderAPI�ӿ�
		/// </summary>
	public:
		RenderAPID3D12();
		~RenderAPID3D12() {};

		virtual void BeginFrame();
		virtual void EndFrame();


		/// <summary>
		/// ������ʱһ���Գ�ʼ���ĺ���D3D12�������ر���
		/// </summary>
	private:
		const UINT mSwapChainBufferCount = 2;

		// ��������������С
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		// ��Ļ��̨������ͼ���ʽ
		DXGI_FORMAT mPresentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 4X MSAA�����ȼ�
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
		/// D3D12��Դ���Լ���ش������ٽӿ�
		/// </summary>
	private:
	};
}