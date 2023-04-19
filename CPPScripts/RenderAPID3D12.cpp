#include "RenderAPID3D12.h"
#include <stb_image.h>
#include "ProjectSetting.h"
#include "Window/WindowManager.h"

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{}

	std::wstring ToString() const
	{
		// Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};
inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}

namespace ZXEngine
{
	RenderAPID3D12::RenderAPID3D12()
	{
		InitD3D12();
		GetDeviceProperties();
		CreateCommandResources();

		ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	}

	void RenderAPID3D12::InitD3D12()
	{
		// Debug
		if (ProjectSetting::enableValidationLayer)
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}

		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));

		// 优先使用硬件设备
		HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3D12Device));
		// 如果硬件条件不支持，使用WARP设备(Windows Advanced Rasterization Platform，微软用软件模拟的光栅化硬件显卡)
		if (FAILED(hardwareResult))
		{
			ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowIfFailed(mDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
			ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3D12Device)));
		}
	}

	void RenderAPID3D12::GetDeviceProperties()
	{
		mRtvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDsvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mCbvSrvUavDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// 获取4X MSAA质量等级
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {};
		msQualityLevels.Format = mPresentBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
		m4xMSAAQuality = msQualityLevels.NumQualityLevels;
		assert(m4xMSAAQuality > 0 && "Unexpected MSAA quality level.");
	}

	void RenderAPID3D12::CreateCommandResources()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(mD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

		mCommands.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < mCommands.size(); i++)
		{
			ThrowIfFailed(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
				IID_PPV_ARGS(mCommands[i].allocator.GetAddressOf())));

			ThrowIfFailed(mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommands[i].allocator.Get(), nullptr, 
				IID_PPV_ARGS(mCommands[i].commandList.GetAddressOf())));

			// 使用CommandList的时候会先Reset，Reset时要求处于Close状态
			mCommands[i].commandList->Close();
		}
	}

	void RenderAPID3D12::CreateSwapChain()
	{
		// 重新创建交换链也会调这个函数，所以先Reset
		mSwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = ProjectSetting::srcWidth;
		swapChainDesc.BufferDesc.Height = ProjectSetting::srcHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = mPresentBufferFormat;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = msaaSamplesCount;
		swapChainDesc.SampleDesc.Quality = msaaSamplesCount == 1 ? 0 : (m4xMSAAQuality - 1);
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = mSwapChainBufferCount;
		swapChainDesc.OutputWindow = static_cast<HWND>(WindowManager::GetInstance()->GetWindow());
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(mDXGIFactory->CreateSwapChain(mCommandQueue.Get(), &swapChainDesc, mSwapChain.GetAddressOf()));
	}

	void RenderAPID3D12::BeginFrame()
	{

	}

	void RenderAPID3D12::EndFrame()
	{

	}

	unsigned int RenderAPID3D12::LoadTexture(const char* path, int& width, int& height)
	{
		int nrComponents;
		stbi_uc* pixels = stbi_load(path, &width, &height, &nrComponents, STBI_rgb_alpha);

		// 创建纹理资源
		CD3DX12_HEAP_PROPERTIES textureProps(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC textureDesc(CD3DX12_RESOURCE_DESC::Tex2D(mDefaultImageFormat, width, height, 1, 1));
		ComPtr<ID3D12Resource> textureResource;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&textureProps,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&textureResource)
		));

		// 创建纹理上传堆
		UINT64 uploadHeapSize;
		mD3D12Device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadHeapSize);
		CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC uploadHeapDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadHeapSize);
		ComPtr<ID3D12Resource> uploadHeap;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&uploadHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadHeapDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadHeap)
		));

		// 上传纹理数据
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList> cmdList)
		{
			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pixels;
			subresourceData.RowPitch = static_cast<LONG_PTR>(width * 4);
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(cmdList.Get(),
				textureResource.Get(),
				uploadHeap.Get(),
				0, 0, 1, &subresourceData);

			// 转换纹理状态
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				textureResource.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);

			cmdList->ResourceBarrier(1, &barrier);
		});

		stbi_image_free(pixels);

		uint32_t textureID = GetNextTextureIndex();
		auto texture = GetTextureByIndex(textureID);
		texture->inUse = true;
		texture->texture = textureResource;

		return textureID;
	}

	unsigned int RenderAPID3D12::LoadCubeMap(const vector<string>& faces)
	{
		int width, height, nrChannels;
		stbi_uc* imageData[6] = {};

		for (int i = 0; i < 6; ++i) 
		{
			imageData[i] = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (!imageData[i])
				Debug::LogError("Failed to load texture file: " + faces[i]);
		}

		// 创建CubeMap资源
		CD3DX12_HEAP_PROPERTIES cubeMapHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC cubeMapDesc = {};
		cubeMapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		cubeMapDesc.Alignment = 0;
		cubeMapDesc.Width = width;
		cubeMapDesc.Height = height;
		cubeMapDesc.DepthOrArraySize = 6;
		cubeMapDesc.MipLevels = 1;
		cubeMapDesc.Format = mDefaultImageFormat;
		cubeMapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		cubeMapDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		cubeMapDesc.SampleDesc.Count = 1;
		cubeMapDesc.SampleDesc.Quality = 0;

		ComPtr<ID3D12Resource> cubeMapResource;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&cubeMapHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&cubeMapDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&cubeMapResource)
		));

		// 创建CubeMap上传堆
		UINT64 uploadHeapSize;
		mD3D12Device->GetCopyableFootprints(&cubeMapDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadHeapSize);
		CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC uploadHeapDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadHeapSize);
		ComPtr<ID3D12Resource> uploadHeap;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&uploadHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadHeapDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadHeap)
		));

		// 上传纹理数据
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList> cmdList)
		{
			D3D12_SUBRESOURCE_DATA cubeMapData[6] = {};
			for (int i = 0; i < 6; ++i)
			{
				cubeMapData[i].pData = imageData[i];
				cubeMapData[i].RowPitch = static_cast<LONG_PTR>(width * nrChannels);
				cubeMapData[i].SlicePitch = cubeMapData[i].RowPitch * height;
			}

			UpdateSubresources(cmdList.Get(),
				cubeMapResource.Get(),
				uploadHeap.Get(),
				0, 0, 6, cubeMapData);

			// 转换纹理状态
			CD3DX12_RESOURCE_BARRIER copyFinishBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				cubeMapResource.Get(), 
				D3D12_RESOURCE_STATE_COPY_DEST, 
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);

			cmdList->ResourceBarrier(1, &copyFinishBarrier);
		});

		for (int i = 0; i < 6; ++i)
			if (imageData[i])
				stbi_image_free(imageData[i]);

		uint32_t textureID = GetNextTextureIndex();
		auto texture = GetTextureByIndex(textureID);
		texture->inUse = true;
		texture->texture = cubeMapResource;

		return textureID;
	}

	void RenderAPID3D12::DeleteTexture(unsigned int id)
	{
		DestroyTextureByIndex(id);
	}

	void RenderAPID3D12::SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->size = static_cast<UINT>(indices.size());

		// 创建Vertex Buffer
		UINT vertexBufferSize = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		meshBuffer->vertexBuffer = CreateDefaultBuffer(vertices.data(), vertexBufferSize);
		meshBuffer->vertexBufferView.SizeInBytes = vertexBufferSize;
		meshBuffer->vertexBufferView.StrideInBytes = sizeof(Vertex);
		meshBuffer->vertexBufferView.BufferLocation = meshBuffer->vertexBuffer->GetGPUVirtualAddress();

		// 创建Index Buffer
		UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indices.size());
		meshBuffer->indexBuffer = CreateDefaultBuffer(indices.data(), indexBufferSize);
		meshBuffer->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		meshBuffer->indexBufferView.SizeInBytes = indexBufferSize;
		meshBuffer->indexBufferView.BufferLocation = meshBuffer->indexBuffer->GetGPUVirtualAddress();

		meshBuffer->inUse = true;
	}

	void RenderAPID3D12::SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->size = static_cast<UINT>(indexSize);

		// 创建动态Vertex Buffer
		UINT vertexBufferSize = static_cast<UINT>(sizeof(Vertex) * vertexSize);
		CD3DX12_HEAP_PROPERTIES vertexBufferProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&vertexBufferProps,
			D3D12_HEAP_FLAG_NONE,
			&vertexBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(meshBuffer->vertexBuffer.GetAddressOf())));

		meshBuffer->vertexBuffer->Map(0, nullptr, &meshBuffer->vertexBufferAddress);

		// 创建动态Index Buffer
		UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indexSize);
		CD3DX12_HEAP_PROPERTIES indexBufferProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&indexBufferProps,
			D3D12_HEAP_FLAG_NONE,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(meshBuffer->indexBuffer.GetAddressOf())));

		meshBuffer->indexBuffer->Map(0, nullptr, &meshBuffer->indexBufferAddress);

		meshBuffer->inUse = true;
	}

	void RenderAPID3D12::UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		memcpy(meshBuffer->vertexBufferAddress, vertices.data(), vertices.size() * sizeof(Vertex));
		memcpy(meshBuffer->indexBufferAddress, indices.data(), indices.size() * sizeof(uint32_t));
	}

	void RenderAPID3D12::GenerateParticleMesh(unsigned int& VAO)
	{
		vector<Vertex> vertices =
		{
			{.Position = {  0.5f,  0.5f, 0.0f }, .TexCoords = { 1.0f, 0.0f } },
			{.Position = {  0.5f, -0.5f, 0.0f }, .TexCoords = { 1.0f, 1.0f } },
			{.Position = { -0.5f,  0.5f, 0.0f }, .TexCoords = { 0.0f, 0.0f } },
			{.Position = { -0.5f, -0.5f, 0.0f }, .TexCoords = { 0.0f, 1.0f } },
		};

		vector<uint32_t> indices =
		{
			2, 1, 3,
			2, 0, 1,
		};

		SetUpStaticMesh(VAO, vertices, indices);
	}

	void RenderAPID3D12::DeleteMesh(unsigned int VAO)
	{
		DestroyVAOByIndex(VAO);
	}


	uint32_t RenderAPID3D12::GetNextFenceIndex()
	{
		uint32_t length = static_cast<uint32_t>(mFenceArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mFenceArray[i]->inUse)
				return i;
		}

		auto fence = new ZXD3D12Fence();
		ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence->fence)));
		mFenceArray.push_back(fence);

		return length;
	}

	ZXD3D12Fence* RenderAPID3D12::GetFenceByIndex(uint32_t idx)
	{
		return mFenceArray[idx];
	}

	void RenderAPID3D12::DestroyFenceByIndex(uint32_t idx)
	{
		auto fence = mFenceArray[idx];

		fence->currentFence = 0;
		fence->fence->Release();

		fence->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextVAOIndex()
	{
		uint32_t length = static_cast<uint32_t>(mVAOArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mVAOArray[i]->inUse)
				return i;
		}

		mVAOArray.push_back(new ZXD3D12VAO());

		return length;
	}

	ZXD3D12VAO* RenderAPID3D12::GetVAOByIndex(uint32_t idx)
	{
		return mVAOArray[idx];
	}

	void RenderAPID3D12::DestroyVAOByIndex(uint32_t idx)
	{
		auto VAO = mVAOArray[idx];

		if (VAO->indexBufferAddress != nullptr)
		{
			VAO->indexBuffer->Unmap(0, nullptr);
			VAO->indexBufferAddress = nullptr;
		}
		if (VAO->vertexBufferAddress != nullptr)
		{
			VAO->vertexBuffer->Unmap(0, nullptr);
			VAO->vertexBufferAddress = nullptr;
		}

		// 可能不需要手动调这个
		VAO->indexBuffer.Reset();
		VAO->vertexBuffer.Reset();

		VAO->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextTextureIndex()
	{
		uint32_t length = static_cast<uint32_t>(mTextureArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mTextureArray[i]->inUse)
				return i;
		}

		mTextureArray.push_back(new ZXD3D12Texture());

		return length;
	}

	ZXD3D12Texture* RenderAPID3D12::GetTextureByIndex(uint32_t idx)
	{
		return mTextureArray[idx];
	}

	void RenderAPID3D12::DestroyTextureByIndex(uint32_t idx)
	{
		auto texture = mTextureArray[idx];

		// 不调这个也行，因为ComPtr<ID3D12Resource>有类似智能指针一样的内存管理，在适当的时候会自动释放
		texture->texture.Reset();

		texture->inUse = false;
	}

	ComPtr<ID3D12Resource> RenderAPID3D12::CreateDefaultBuffer(const void* data, UINT64 size)
	{
		CD3DX12_HEAP_PROPERTIES defaultBufferProps(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC defaultBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
		ComPtr<ID3D12Resource> defaultBuffer;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&defaultBufferProps,
			D3D12_HEAP_FLAG_NONE,
			&defaultBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

		CD3DX12_HEAP_PROPERTIES uploadBufferProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
		ComPtr<ID3D12Resource> uploadBuffer;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&uploadBufferProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = data;
		subResourceData.RowPitch = size;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList> cmdList)
		{
			UpdateSubresources<1>(cmdList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_GENERIC_READ
			);

			cmdList->ResourceBarrier(1, &barrier);
		});

		return defaultBuffer;
	}


	void RenderAPID3D12::InitImmediateExecution()
	{
		ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mImmediateExeFence)));

		ThrowIfFailed(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mImmediateExeAllocator.GetAddressOf())));

		ThrowIfFailed(mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mImmediateExeAllocator.Get(), nullptr,
			IID_PPV_ARGS(mImmediateExeCommandList.GetAddressOf())));

		mImmediateExeCommandList->Close();
	}

	void RenderAPID3D12::ImmediatelyExecute(std::function<void(ComPtr<ID3D12GraphicsCommandList> cmdList)>&& function)
	{
		// 重置命令
		ThrowIfFailed(mImmediateExeAllocator->Reset());
		ThrowIfFailed(mImmediateExeCommandList->Reset(mImmediateExeAllocator.Get(), nullptr));

		// 记录命令
		function(mImmediateExeCommandList);

		// 立刻执行
		ThrowIfFailed(mImmediateExeCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { mImmediateExeCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// 设置信号量
		mImmediateExeFenceValue++;
		ThrowIfFailed(mCommandQueue->Signal(mImmediateExeFence.Get(), mImmediateExeFenceValue));

		// 如果Fence的进度没到刚刚设置的值
		if (mImmediateExeFence->GetCompletedValue() < mImmediateExeFenceValue)
		{
			// 创建一个"进度抵达刚刚设置的信号量值"的事件
			HANDLE eventHandle = CreateEventEx(nullptr, NULL, NULL, EVENT_ALL_ACCESS);
			ThrowIfFailed(mFence->SetEventOnCompletion(mImmediateExeFenceValue, eventHandle));
			// 等待事件
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
}