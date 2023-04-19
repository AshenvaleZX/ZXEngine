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

		// 资源加载相关
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual void DeleteTexture(unsigned int id);
		virtual unsigned int LoadCubeMap(const vector<string>& faces);

		// Mesh设置
		virtual void DeleteMesh(unsigned int VAO);
		virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
		virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void GenerateParticleMesh(unsigned int& VAO);


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
		// 默认的纹理和FrameBuffer色彩空间
		const DXGI_FORMAT mDefaultImageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 4X MSAA质量等级
		UINT m4xMSAAQuality = 0;
		UINT msaaSamplesCount = 4;

		UINT64 mCurrentFence = 0;
		ComPtr<ID3D12Fence> mFence;

		ComPtr<IDXGIFactory4> mDXGIFactory;
		ComPtr<ID3D12Device> mD3D12Device;
		ComPtr<IDXGISwapChain> mSwapChain;

		vector<ZXD3D12Command> mCommands;
		ComPtr<ID3D12CommandQueue> mCommandQueue;

		void InitD3D12();
		void GetDeviceProperties();
		void CreateCommandResources();
		void CreateSwapChain();


		/// <summary>
		/// D3D12资源，以及相关创建销毁接口
		/// </summary>
	private:
		vector<ZXD3D12Fence*> mFenceArray;
		vector<ZXD3D12VAO*> mVAOArray;
		vector<ZXD3D12Texture*> mTextureArray;

		uint32_t GetNextFenceIndex();
		ZXD3D12Fence* GetFenceByIndex(uint32_t idx);
		void DestroyFenceByIndex(uint32_t idx);
		uint32_t GetNextVAOIndex();
		ZXD3D12VAO* GetVAOByIndex(uint32_t idx);
		void DestroyVAOByIndex(uint32_t idx);
		uint32_t GetNextTextureIndex();
		ZXD3D12Texture* GetTextureByIndex(uint32_t idx);
		void DestroyTextureByIndex(uint32_t idx);

		ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize);


		/// <summary>
		/// 其它辅助接口
		/// </summary>
	private:
		UINT64 mImmediateExeFenceValue = 0;
		ComPtr<ID3D12Fence> mImmediateExeFence;
		ComPtr<ID3D12CommandAllocator> mImmediateExeAllocator;
		ComPtr<ID3D12GraphicsCommandList> mImmediateExeCommandList;

		void InitImmediateExecution();
		void ImmediatelyExecute(std::function<void(ComPtr<ID3D12GraphicsCommandList> cmdList)>&& function);
	};
}