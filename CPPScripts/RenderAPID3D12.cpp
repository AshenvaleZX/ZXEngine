#include "RenderAPID3D12.h"
#include <stb_image.h>
#include <D3Dcompiler.h>
#include "Texture.h"
#include "ZShader.h"
#include "Material.h"
#include "Resources.h"
#include "GlobalData.h"
#include "MaterialData.h"
#include "ShaderParser.h"
#include "ProjectSetting.h"
#include "Window/WindowManager.h"
#include "DirectX12/ZXD3D12DescriptorManager.h"

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
	map<BlendFactor, D3D12_BLEND> dxBlendFactorMap =
	{
		{ BlendFactor::ZERO,      D3D12_BLEND_ZERO       }, { BlendFactor::ONE,                 D3D12_BLEND_ONE            },
		{ BlendFactor::SRC_COLOR, D3D12_BLEND_SRC_COLOR  }, { BlendFactor::ONE_MINUS_SRC_COLOR, D3D12_BLEND_INV_SRC_COLOR  },
		{ BlendFactor::DST_COLOR, D3D12_BLEND_DEST_COLOR }, { BlendFactor::ONE_MINUS_DST_COLOR, D3D12_BLEND_INV_DEST_COLOR },
		{ BlendFactor::SRC_ALPHA, D3D12_BLEND_SRC_ALPHA  }, { BlendFactor::ONE_MINUS_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA  },
		{ BlendFactor::DST_ALPHA, D3D12_BLEND_DEST_ALPHA }, { BlendFactor::ONE_MINUS_DST_ALPHA, D3D12_BLEND_INV_DEST_ALPHA },
	};

	map<BlendOption, D3D12_BLEND_OP> dxBlendOptionMap =
	{
		{ BlendOption::ADD, D3D12_BLEND_OP_ADD }, { BlendOption::SUBTRACT, D3D12_BLEND_OP_SUBTRACT }, { BlendOption::REVERSE_SUBTRACT, D3D12_BLEND_OP_REV_SUBTRACT },
		{ BlendOption::MIN, D3D12_BLEND_OP_MIN }, { BlendOption::MAX,      D3D12_BLEND_OP_MAX      },
	};

	map<FaceCullOption, D3D12_CULL_MODE> dxFaceCullOptionMap =
	{
		{ FaceCullOption::Back, D3D12_CULL_MODE_BACK }, { FaceCullOption::Front, D3D12_CULL_MODE_FRONT },
		{ FaceCullOption::None, D3D12_CULL_MODE_NONE },
	};

	map<CompareOption, D3D12_COMPARISON_FUNC> dxCompareOptionMap =
	{
		{ CompareOption::NEVER,         D3D12_COMPARISON_FUNC_NEVER      }, { CompareOption::LESS,             D3D12_COMPARISON_FUNC_LESS          },
		{ CompareOption::ALWAYS,        D3D12_COMPARISON_FUNC_ALWAYS     }, { CompareOption::GREATER,          D3D12_COMPARISON_FUNC_GREATER       },
		{ CompareOption::EQUAL,         D3D12_COMPARISON_FUNC_EQUAL      }, { CompareOption::NOT_EQUAL,        D3D12_COMPARISON_FUNC_NOT_EQUAL     },
		{ CompareOption::LESS_OR_EQUAL, D3D12_COMPARISON_FUNC_LESS_EQUAL }, { CompareOption::GREATER_OR_EQUAL, D3D12_COMPARISON_FUNC_GREATER_EQUAL },
	};

	RenderAPID3D12::RenderAPID3D12()
	{
		InitD3D12();
		GetDeviceProperties();
		CreateSwapChain();

		InitImmediateExecution();

		ZXD3D12DescriptorManager::Creat();

		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			mFrameFences.push_back(CreateZXD3D12Fence());
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

		// 创建命令队列
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(mD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
		
		// 创建GPU描述符堆，Shader Visible描述符堆在硬件上有数量限制，见: https://learn.microsoft.com/en-us/windows/win32/direct3d12/shader-visible-descriptor-heaps
		// 微软文档说一般硬件Shader Visible描述符堆可用内存在96MB左右， A one million member descriptor heap, with 32byte descriptors, would use up 32MB, for example.
		// 所以一般来说，差不多能支持300万个描述符，不过这里先不用那么多
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 65536;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		mDynamicDescriptorHeaps.resize(DX_MAX_FRAMES_IN_FLIGHT);
		mDynamicDescriptorOffsets.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			mDynamicDescriptorOffsets[i] = 0;
			mD3D12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mDynamicDescriptorHeaps[i]));
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
		swapChainDesc.BufferCount = mPresentBufferCount;
		swapChainDesc.OutputWindow = static_cast<HWND>(WindowManager::GetInstance()->GetWindow());
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(mDXGIFactory->CreateSwapChain(mCommandQueue.Get(), &swapChainDesc, mSwapChain.GetAddressOf()));
	}

	void RenderAPID3D12::CreateSwapChainBuffers()
	{
		mPresentFBOIdx = GetNextFBOIndex();
		uint32_t colorBufferIdx = GetNextRenderBufferIndex();
		auto colorBuffer = GetRenderBufferByIndex(colorBufferIdx);
		colorBuffer->renderBuffers.clear();
		colorBuffer->renderBuffers.resize(mPresentBufferCount);
		colorBuffer->inUse = true;

		auto presentFBO = GetFBOByIndex(mPresentFBOIdx);
		presentFBO->bufferType = FrameBufferType::Present;
		presentFBO->colorBufferIdx = colorBufferIdx;

		mPresentBuffers.resize(mPresentBufferCount);
		for (UINT i = 0; i < mPresentBufferCount; i++)
		{
			ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mPresentBuffers[i])));

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = mPresentBufferFormat;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			colorBuffer->renderBuffers[i] = CreateZXD3D12Texture(mPresentBuffers[i], rtvDesc);
		}

		presentFBO->inUse = true;
	}

	void RenderAPID3D12::BeginFrame()
	{
		mDynamicDescriptorOffsets[mCurrentFrame] = 0;
	}

	void RenderAPID3D12::EndFrame()
	{

	}

	void RenderAPID3D12::SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset)
	{
		mViewPortInfo.width = width;
		mViewPortInfo.height = height;
		mViewPortInfo.xOffset = xOffset;

		// 传入的参数是按0点在左下角的标准来的，Vulkan的0点在左上角，如果有偏移(编辑器模式)的话，Y轴偏移量要重新计算一下
		if (xOffset == 0 && yOffset == 0)
			mViewPortInfo.yOffset = yOffset;
		else
			mViewPortInfo.yOffset = ProjectSetting::srcHeight - height - yOffset;
	}

	void RenderAPID3D12::SwitchFrameBuffer(uint32_t id)
	{
		if (id == UINT32_MAX)
			mCurFBOIdx = mPresentFBOIdx;
		else
			mCurFBOIdx = id;
	}

	void RenderAPID3D12::ClearFrameBuffer()
	{
		// D3D12不需要实现这个接口
	}

	FrameBufferObject* RenderAPID3D12::CreateFrameBufferObject(FrameBufferType type, unsigned int width, unsigned int height)
	{
		ClearInfo clearInfo = {};
		return CreateFrameBufferObject(type, clearInfo, width, height);
	}

	FrameBufferObject* RenderAPID3D12::CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width, unsigned int height)
	{
		FrameBufferObject* FBO = new FrameBufferObject(type);
		FBO->clearInfo = clearInfo;
		FBO->isFollowWindow = width == 0 || height == 0;

		width = width == 0 ? GlobalData::srcWidth : width;
		height = height == 0 ? GlobalData::srcHeight : height;

		if (type == FrameBufferType::Normal)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = GetNextRenderBufferIndex();
			auto colorBuffer = GetRenderBufferByIndex(FBO->ColorBuffer);
			colorBuffer->inUse = true;
			FBO->DepthBuffer = GetNextRenderBufferIndex();
			auto depthBuffer = GetRenderBufferByIndex(FBO->DepthBuffer);
			depthBuffer->inUse = true;

			auto D3D12FBO = GetFBOByIndex(FBO->ID);
			D3D12FBO->colorBufferIdx = FBO->ColorBuffer;
			D3D12FBO->depthBufferIdx = FBO->DepthBuffer;
			D3D12FBO->bufferType = FrameBufferType::Normal;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// 创建Color Buffer
				CD3DX12_HEAP_PROPERTIES colorBufferProps(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC colorBufferDesc(CD3DX12_RESOURCE_DESC::Tex2D(mDefaultImageFormat, width, height, 1, 1));
				ComPtr<ID3D12Resource> colorBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&colorBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&colorBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&colorBufferResource)
				));
				
				D3D12_SHADER_RESOURCE_VIEW_DESC colorSrvDesc = {};
				colorSrvDesc.Format = mDefaultImageFormat;
				colorSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				colorSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				colorSrvDesc.Texture2D.MipLevels = 1;
				colorSrvDesc.Texture2D.MostDetailedMip = 0;
				colorSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_RENDER_TARGET_VIEW_DESC colorRtvDesc = {};
				colorRtvDesc.Format = mDefaultImageFormat;
				colorRtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				colorRtvDesc.Texture2D.MipSlice = 0;

				colorBuffer->renderBuffers[i] = CreateZXD3D12Texture(colorBufferResource, colorSrvDesc, colorRtvDesc);

				// 创建Depth Buffer
				CD3DX12_HEAP_PROPERTIES depthBufferProps(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC depthBufferDesc(CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D16_UNORM, width, height, 1, 1));
				ComPtr<ID3D12Resource> depthBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&depthBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&depthBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
				depthSrvDesc.Format = DXGI_FORMAT_D16_UNORM;
				depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				depthSrvDesc.Texture2D.MipLevels = 1;
				depthSrvDesc.Texture2D.MostDetailedMip = 0;
				depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_RENDER_TARGET_VIEW_DESC depthRtvDesc = {};
				depthRtvDesc.Format = DXGI_FORMAT_D16_UNORM;
				depthRtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				depthRtvDesc.Texture2D.MipSlice = 0;

				depthBuffer->renderBuffers[i] = CreateZXD3D12Texture(depthBufferResource, depthSrvDesc, depthRtvDesc);
			}

			D3D12FBO->inUse = true;
		}
		else if (type == FrameBufferType::Color)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = GetNextRenderBufferIndex();
			auto colorBuffer = GetRenderBufferByIndex(FBO->ColorBuffer);
			colorBuffer->inUse = true;
			FBO->DepthBuffer = NULL;

			auto D3D12FBO = GetFBOByIndex(FBO->ID);
			D3D12FBO->colorBufferIdx = FBO->ColorBuffer;
			D3D12FBO->bufferType = FrameBufferType::Normal;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// 创建Color Buffer
				CD3DX12_HEAP_PROPERTIES colorBufferProps(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC colorBufferDesc(CD3DX12_RESOURCE_DESC::Tex2D(mDefaultImageFormat, width, height, 1, 1));
				ComPtr<ID3D12Resource> colorBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&colorBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&colorBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&colorBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC colorSrvDesc = {};
				colorSrvDesc.Format = mDefaultImageFormat;
				colorSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				colorSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				colorSrvDesc.Texture2D.MipLevels = 1;
				colorSrvDesc.Texture2D.MostDetailedMip = 0;
				colorSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_RENDER_TARGET_VIEW_DESC colorRtvDesc = {};
				colorRtvDesc.Format = mDefaultImageFormat;
				colorRtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				colorRtvDesc.Texture2D.MipSlice = 0;

				colorBuffer->renderBuffers[i] = CreateZXD3D12Texture(colorBufferResource, colorSrvDesc, colorRtvDesc);
			}

			D3D12FBO->inUse = true;
		}
		else if (type == FrameBufferType::ShadowCubeMap)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = NULL;
			FBO->DepthBuffer = GetNextRenderBufferIndex();
			auto depthBuffer = GetRenderBufferByIndex(FBO->DepthBuffer);
			depthBuffer->inUse = true;

			auto D3D12FBO = GetFBOByIndex(FBO->ID);
			D3D12FBO->depthBufferIdx = FBO->DepthBuffer;
			D3D12FBO->bufferType = FrameBufferType::Normal;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// 创建Depth Buffer
				CD3DX12_HEAP_PROPERTIES depthBufferProps(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC depthBufferDesc(CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D16_UNORM, width, height, 1, 6));
				ComPtr<ID3D12Resource> depthBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&depthBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&depthBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
				depthSrvDesc.Format = DXGI_FORMAT_D16_UNORM;
				depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				depthSrvDesc.Texture2D.MipLevels = 1;
				depthSrvDesc.Texture2D.MostDetailedMip = 0;
				depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_RENDER_TARGET_VIEW_DESC depthRtvDesc = {};
				depthRtvDesc.Format = DXGI_FORMAT_D16_UNORM;
				depthRtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				depthRtvDesc.Texture2DArray.MipSlice = 0;
				depthRtvDesc.Texture2DArray.ArraySize = 6;
				depthRtvDesc.Texture2DArray.FirstArraySlice = 0;

				depthBuffer->renderBuffers[i] = CreateZXD3D12Texture(depthBufferResource, depthSrvDesc, depthRtvDesc);
			}

			D3D12FBO->inUse = true;
		}
		else
		{
			Debug::LogError("Invalide frame buffer type.");
		}

		return FBO;
	}

	void RenderAPID3D12::DeleteFrameBufferObject(FrameBufferObject* FBO)
	{
		DestroyFBOByIndex(FBO->ID);
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

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = mDefaultImageFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		return CreateZXD3D12Texture(textureResource, srvDesc);
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

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = mDefaultImageFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		return CreateZXD3D12Texture(cubeMapResource, srvDesc);
	}

	unsigned int RenderAPID3D12::GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data)
	{
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
			subresourceData.pData = data;
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

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = mDefaultImageFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		return CreateZXD3D12Texture(textureResource, srvDesc);
	}

	void RenderAPID3D12::DeleteTexture(unsigned int id)
	{
		DestroyTextureByIndex(id);
	}

	ShaderReference* RenderAPID3D12::LoadAndSetUpShader(const char* path, FrameBufferType type)
	{
		string shaderCode = Resources::LoadTextFile(path);
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode);
		string hlslCode = ShaderParser::TranslateToD3D12(shaderCode, shaderInfo);

		UINT compileFlags = 0;
		if (ProjectSetting::enableValidationLayer)
			compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

		// 编译Vertex Shader
		ComPtr<ID3DBlob> vertCode = nullptr;
		if (shaderInfo.stages & ZX_SHADER_STAGE_VERTEX_BIT)
		{
			ComPtr<ID3DBlob> errors;
			ThrowIfFailed(D3DCompile(hlslCode.c_str(), hlslCode.length(), NULL, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"VS", "vs_5_1", compileFlags, 0, &vertCode, &errors));
			if (errors != nullptr)
				Debug::LogError((char*)errors->GetBufferPointer());
		}
		// 编译Geometry Shader
		ComPtr<ID3DBlob> geomCode = nullptr;
		if (shaderInfo.stages & ZX_SHADER_STAGE_GEOMETRY_BIT)
		{
			ComPtr<ID3DBlob> errors;
			ThrowIfFailed(D3DCompile(hlslCode.c_str(), hlslCode.length(), NULL, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"GS", "gs_5_1", compileFlags, 0, &geomCode, &errors));
			if (errors != nullptr)
				Debug::LogError((char*)errors->GetBufferPointer());
		}
		// 编译Fragment(Pixel) Shader
		ComPtr<ID3DBlob> fragCode = nullptr;
		if (shaderInfo.stages & ZX_SHADER_STAGE_FRAGMENT_BIT)
		{
			ComPtr<ID3DBlob> errors;
			ThrowIfFailed(D3DCompile(hlslCode.c_str(), hlslCode.length(), NULL, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"PS", "ps_5_1", compileFlags, 0, &fragCode, &errors));
			if (errors != nullptr)
				Debug::LogError((char*)errors->GetBufferPointer());
		}

		// 准备创建D3D12管线数据
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};

		// 创建根签名
		ComPtr<ID3D12RootSignature> rootSignature;
		{
			UINT textureNum = static_cast<UINT>(
				shaderInfo.vertProperties.textureProperties.size() + 
				shaderInfo.geomProperties.textureProperties.size() + 
				shaderInfo.fragProperties.textureProperties.size()
			);

			CD3DX12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, textureNum, 0, 0);

			CD3DX12_ROOT_PARAMETER rootParameters[2] = {};
			rootParameters[0].InitAsConstantBufferView(0);
			rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_ALL);

			auto samplers = GetStaticSamplersDesc();

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2, rootParameters,
				(UINT)samplers.size(), samplers.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> error;
			ComPtr<ID3DBlob> serializedRootSignature;
			HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &error);
			if (error != nullptr)
				Debug::LogError((char*)error->GetBufferPointer());
			ThrowIfFailed(hr);

			ThrowIfFailed(mD3D12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
			pipelineStateDesc.pRootSignature = rootSignature.Get();
		}

		// 填充Shader
		if (shaderInfo.stages & ZX_SHADER_STAGE_VERTEX_BIT)
			pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(vertCode->GetBufferPointer()), vertCode->GetBufferSize() };
		if (shaderInfo.stages & ZX_SHADER_STAGE_GEOMETRY_BIT)
			pipelineStateDesc.GS = { reinterpret_cast<BYTE*>(geomCode->GetBufferPointer()), geomCode->GetBufferSize() };
		if (shaderInfo.stages & ZX_SHADER_STAGE_FRAGMENT_BIT)
			pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(fragCode->GetBufferPointer()), fragCode->GetBufferSize() };

		// Input Layout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		pipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };

		// Blend Config
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE; // 多个Render Target要不要分开配置混合方式
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
		blendDesc.RenderTarget[0].SrcBlend = dxBlendFactorMap[shaderInfo.stateSet.srcFactor];
		blendDesc.RenderTarget[0].DestBlend = dxBlendFactorMap[shaderInfo.stateSet.dstFactor];
		blendDesc.RenderTarget[0].BlendOp = dxBlendOptionMap[shaderInfo.stateSet.blendOp];
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipelineStateDesc.BlendState = blendDesc;

		// 光栅化阶段
		D3D12_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode = dxFaceCullOptionMap[shaderInfo.stateSet.cull];
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		rasterizerDesc.ForcedSampleCount = 0;
		rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		pipelineStateDesc.RasterizerState = rasterizerDesc;

		// Depth Stencil Config
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = shaderInfo.stateSet.depthCompareOp == CompareOption::ALWAYS ? FALSE : TRUE;
		depthStencilDesc.DepthWriteMask = shaderInfo.stateSet.depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = dxCompareOptionMap[shaderInfo.stateSet.depthCompareOp];
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		};
		depthStencilDesc.FrontFace = defaultStencilOp;
		depthStencilDesc.BackFace = defaultStencilOp;
		pipelineStateDesc.DepthStencilState = depthStencilDesc;

		// Others
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDesc.SampleMask = UINT_MAX;
		pipelineStateDesc.NumRenderTargets = 1;
		pipelineStateDesc.RTVFormats[0] = mDefaultImageFormat;
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D16_UNORM;
		pipelineStateDesc.SampleDesc.Count = 1;
		pipelineStateDesc.SampleDesc.Quality = 0;
		pipelineStateDesc.NodeMask = 0; // 给多GPU用的，暂时不用管

		// 创建D3D12管线
		ComPtr<ID3D12PipelineState> PSO;
		ThrowIfFailed(mD3D12Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&PSO)));

		uint32_t pipelineID = GetNextPipelineIndex();
		auto pipeline = GetPipelineByIndex(pipelineID);
		pipeline->pipelineState = PSO;
		pipeline->rootSignature = rootSignature;
		pipeline->inUse = true;

		ShaderReference* reference = new ShaderReference();
		reference->ID = pipelineID;
		reference->shaderInfo = shaderInfo;

		return reference;
	}

	void RenderAPID3D12::DeleteShader(uint32_t id)
	{
		DestroyPipelineByIndex(id);
	}

	uint32_t RenderAPID3D12::CreateMaterialData()
	{
		uint32_t materialDataID = GetNextMaterialDataIndex();
		auto materialData = GetMaterialDataByIndex(materialDataID);

		materialData->inUse = true;

		return materialDataID;
	}

	void RenderAPID3D12::SetUpMaterial(ShaderReference* shaderReference, MaterialData* materialData)
	{
		auto materialDataZXD3D12 = GetMaterialDataByIndex(materialData->GetID());

		// 计算Constant Buffer大小
		UINT64 bufferSize = static_cast<UINT64>(shaderReference->shaderInfo.fragProperties.baseProperties.back().offset + shaderReference->shaderInfo.fragProperties.baseProperties.back().size);
		// 向上取256整数倍(不是必要操作)
		bufferSize = (bufferSize + 255) & ~255;
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			// 创建Constant Buffer
			materialDataZXD3D12->constantBuffers.push_back(CreateConstantBuffer(bufferSize));

			ZXD3D12MaterialTextureSet materialTextureSet;
			materialTextureSet.textureHandles.resize(
				shaderReference->shaderInfo.vertProperties.textureProperties.size() +
				shaderReference->shaderInfo.geomProperties.textureProperties.size() +
				shaderReference->shaderInfo.fragProperties.textureProperties.size()
			);

			for (auto& matTexture : materialData->textures)
			{
				uint32_t binding = UINT32_MAX;
				for (auto& textureProperty : shaderReference->shaderInfo.fragProperties.textureProperties)
					if (matTexture.first == textureProperty.name)
						binding = textureProperty.binding;

				if (binding == UINT32_MAX)
					for (auto& textureProperty : shaderReference->shaderInfo.vertProperties.textureProperties)
						if (matTexture.first == textureProperty.name)
							binding = textureProperty.binding;

				if (binding == UINT32_MAX)
					for (auto& textureProperty : shaderReference->shaderInfo.geomProperties.textureProperties)
						if (matTexture.first == textureProperty.name)
							binding = textureProperty.binding;

				if (binding == UINT32_MAX)
				{
					Debug::LogError("No texture named " + matTexture.first + " matched !");
					continue;
				}

				auto texture = GetTextureByIndex(matTexture.second->GetID());

				materialTextureSet.textureHandles[binding] = texture->handleSRV;
			}

			materialDataZXD3D12->textureSets.push_back(materialTextureSet);
		}
	}

	void RenderAPID3D12::UseMaterialData(uint32_t ID)
	{
		mCurMaterialDataIdx = ID;
	}

	void RenderAPID3D12::DeleteMaterialData(uint32_t id)
	{
		DestroyMaterialDataByIndex(id);
	}

	uint32_t RenderAPID3D12::AllocateDrawCommand(CommandType commandType)
	{
		uint32_t idx = GetNextDrawCommandIndex();
		auto drawCmd = GetDrawCommandByIndex(idx);

		drawCmd->allocators.resize(DX_MAX_FRAMES_IN_FLIGHT);
		drawCmd->commandLists.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			ThrowIfFailed(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(drawCmd->allocators[i].GetAddressOf())));

			ThrowIfFailed(mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, drawCmd->allocators[i].Get(), nullptr,
				IID_PPV_ARGS(drawCmd->commandLists[i].GetAddressOf())));

			// 使用CommandList的时候会先Reset，Reset时要求处于Close状态
			drawCmd->commandLists[i]->Close();
		}

		drawCmd->inUse = true;

		return idx;
	}

	void RenderAPID3D12::Draw(uint32_t VAO)
	{
		mDrawIndexes.push_back({ .VAO = VAO, .pipelineID = mCurPipeLineIdx, .materialDataID = mCurMaterialDataIdx });
	}

	void RenderAPID3D12::GenerateDrawCommand(uint32_t id)
	{
		auto drawCommand = GetDrawCommandByIndex(id);
		auto& allocator = drawCommand->allocators[mCurrentFrame];
		auto& drawCommandList = drawCommand->commandLists[mCurrentFrame];

		// 重置Command List
		ThrowIfFailed(allocator->Reset());
		ThrowIfFailed(drawCommandList->Reset(allocator.Get(), nullptr));

		auto curFBO = GetFBOByIndex(mCurFBOIdx);
		ZXD3D12Texture* colorBuffer = nullptr;
		ZXD3D12Texture* depthBuffer = nullptr;

		if (curFBO->bufferType == FrameBufferType::Normal)
		{
			// 获取渲染目标Buffer
			colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			depthBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->depthBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			
			// 切换为写入状态
			auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			drawCommandList->ResourceBarrier(1, &depthBufferTransition);

			// 切换目标Buffer
			auto rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
			auto dsv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(depthBuffer->handleDSV);
			drawCommandList->OMSetRenderTargets(1, &rtv, false, &dsv);

			// 清理缓冲区
			auto& clearInfo = curFBO->clearInfo;
			const float clearColor[] = { clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a };
			drawCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
			drawCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, clearInfo.depth, 0, 0, nullptr);
		}
		else if (curFBO->bufferType == FrameBufferType::Color || curFBO->bufferType == FrameBufferType::HigthPrecision)
		{
			colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);

			auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &colorBufferTransition);

			auto rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
			drawCommandList->OMSetRenderTargets(1, &rtv, false, nullptr);

			auto& clearInfo = curFBO->clearInfo;
			const float clearColor[] = { clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a };
			drawCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		}
		else if (curFBO->bufferType == FrameBufferType::ShadowMap || curFBO->bufferType == FrameBufferType::ShadowCubeMap)
		{
			depthBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->depthBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);

			auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			drawCommandList->ResourceBarrier(1, &depthBufferTransition);

			auto dsv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(depthBuffer->handleDSV);
			drawCommandList->OMSetRenderTargets(0, nullptr, false, &dsv);

			auto& clearInfo = curFBO->clearInfo;
			drawCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, clearInfo.depth, 0, 0, nullptr);
		}

		// 设置Viewport
		D3D12_VIEWPORT viewPort = {};
		viewPort.Width    = static_cast<FLOAT>(mViewPortInfo.width);
		viewPort.Height   = static_cast<FLOAT>(mViewPortInfo.height);
		viewPort.TopLeftX = static_cast<FLOAT>(mViewPortInfo.xOffset);
		viewPort.TopLeftY = static_cast<FLOAT>(mViewPortInfo.yOffset);
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		drawCommandList->RSSetViewports(1, &viewPort);
		// 设置Scissor
		D3D12_RECT scissor = {};
		scissor.left   = mViewPortInfo.xOffset;
		scissor.top    = mViewPortInfo.yOffset;
		scissor.right  = mViewPortInfo.xOffset + mViewPortInfo.width;
		scissor.bottom = mViewPortInfo.yOffset + mViewPortInfo.height;
		drawCommandList->RSSetScissorRects(1, &scissor);

		// 设置图元类型
		drawCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 设置当前帧的动态描述符堆
		ID3D12DescriptorHeap* curDescriptorHeaps[] = { mDynamicDescriptorHeaps[mCurrentFrame].Get() };
		drawCommandList->SetDescriptorHeaps(_countof(curDescriptorHeaps), curDescriptorHeaps);

		// 获取当前帧的动态描述符堆Handle
		CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicDescriptorHandle(mDynamicDescriptorHeaps[mCurrentFrame]->GetCPUDescriptorHandleForHeapStart());
		// 偏移到当前位置
		dynamicDescriptorHandle.Offset(mDynamicDescriptorOffsets[mCurrentFrame], mCbvSrvUavDescriptorSize);

		for (auto& iter : mDrawIndexes)
		{
			auto VAO = GetVAOByIndex(iter.VAO);
			auto pipeline = GetPipelineByIndex(iter.pipelineID);
			auto materialData = GetMaterialDataByIndex(iter.materialDataID);

			drawCommandList->SetGraphicsRootSignature(pipeline->rootSignature.Get());
			drawCommandList->SetPipelineState(pipeline->pipelineState.Get());
			drawCommandList->SetGraphicsRootConstantBufferView(0, materialData->constantBuffers[mCurrentFrame].constantBuffer->GetGPUVirtualAddress());

			// 当前绘制对象在动态描述符堆中的偏移起点
			UINT curDynamicDescriptorOffset = mDynamicDescriptorOffsets[mCurrentFrame];
			// 遍历纹理并拷贝到动态描述符堆
			for (auto& iter : materialData->textureSets[mCurrentFrame].textureHandles)
			{
				// 获取纹理的CPU Handle
				auto cpuHandle = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(iter);
				// 拷贝到动态描述符堆
				mD3D12Device->CopyDescriptorsSimple(1, dynamicDescriptorHandle, cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				// 动态描述符堆Handle后移一位
				dynamicDescriptorHandle.Offset(1, mCbvSrvUavDescriptorSize);
				mDynamicDescriptorOffsets[mCurrentFrame]++;
			}
			// 获取动态描述符堆的GPU Handle
			CD3DX12_GPU_DESCRIPTOR_HANDLE dynamicGPUHandle(mDynamicDescriptorHeaps[mCurrentFrame]->GetGPUDescriptorHandleForHeapStart());
			// 偏移到当前绘制对象的起始位置
			dynamicGPUHandle.Offset(curDynamicDescriptorOffset, mCbvSrvUavDescriptorSize);
			// 设置当前绘制对象的动态描述符堆
			drawCommandList->SetGraphicsRootDescriptorTable(1, dynamicGPUHandle);

			drawCommandList->IASetIndexBuffer(&VAO->indexBufferView);
			drawCommandList->IASetVertexBuffers(0, 1, &VAO->vertexBufferView);
			drawCommandList->DrawIndexedInstanced(VAO->size, 1, 0, 0, 0);
		}

		// 把状态切回去
		if (curFBO->bufferType == FrameBufferType::Normal)
		{
			if (colorBuffer != nullptr)
			{
				auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
				drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			}
			if (depthBuffer != nullptr)
			{
				auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
				drawCommandList->ResourceBarrier(1, &depthBufferTransition);
			}
		}
		else if (curFBO->bufferType == FrameBufferType::Color || curFBO->bufferType == FrameBufferType::HigthPrecision)
		{
			if (colorBuffer != nullptr)
			{
				auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
				drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			}
		}
		else if (curFBO->bufferType == FrameBufferType::ShadowMap || curFBO->bufferType == FrameBufferType::ShadowCubeMap)
		{
			if (depthBuffer != nullptr)
			{
				auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
				drawCommandList->ResourceBarrier(1, &depthBufferTransition);
			}
		}

		// 结束并提交Command List
		ThrowIfFailed(drawCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { drawCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
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

	void RenderAPID3D12::UseShader(unsigned int ID)
	{
		mCurPipeLineIdx = ID;
	}

	// Integer
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, int value, bool allBuffer)
	{
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);
			memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Boolean
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer)
	{
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);
			memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Float
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, float value, bool allBuffer)
	{
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);
			memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Vector2
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer)
	{
		SetShaderVector(material, name, value, 0, allBuffer);
	}
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[2];
		value.ToArray(array);
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 2);
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
			memcpy(valueAddress, array, sizeof(float) * 2);
		}
		delete[] array;
	}

	// Vector3
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer)
	{
		SetShaderVector(material, name, value, 0, allBuffer);
	}
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[3];
		value.ToArray(array);
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 3);
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
			memcpy(valueAddress, array, sizeof(float) * 3);
		}
		delete[] array;
	}

	// Vector4
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer)
	{
		SetShaderVector(material, name, value, 0, allBuffer);
	}
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[4];
		value.ToArray(array);
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 4);
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
			memcpy(valueAddress, array, sizeof(float) * 4);
		}
		delete[] array;
	}

	// Matrix3
	void RenderAPID3D12::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer)
	{
		SetShaderMatrix(material, name, value, 0, allBuffer);
	}
	void RenderAPID3D12::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[9];
		value.ToColumnMajorArray(array);
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 9);
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
			memcpy(valueAddress, array, sizeof(float) * 9);
		}
		delete[] array;
	}

	// Matrix4
	void RenderAPID3D12::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer)
	{
		SetShaderMatrix(material, name, value, 0, allBuffer);
	}
	void RenderAPID3D12::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[16];
		value.ToColumnMajorArray(array);
		if (allBuffer)
		{
			auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 16);
		}
		else
		{
			void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
			memcpy(valueAddress, array, sizeof(float) * 16);
		}
		delete[] array;
	}

	void RenderAPID3D12::SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());

		if (allBuffer)
		{
			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				uint32_t textureID = ID;
				if (isBuffer)
					textureID = GetRenderBufferByIndex(ID)->renderBuffers[i];

				auto texture = GetTextureByIndex(textureID);
				uint32_t binding = UINT32_MAX;

				for (auto& textureProperty : material->shader->reference->shaderInfo.fragProperties.textureProperties)
					if (name == textureProperty.name)
						binding = textureProperty.binding;

				if (binding == UINT32_MAX)
					for (auto& textureProperty : material->shader->reference->shaderInfo.vertProperties.textureProperties)
						if (name == textureProperty.name)
							binding = textureProperty.binding;

				if (binding == UINT32_MAX)
					for (auto& textureProperty : material->shader->reference->shaderInfo.geomProperties.textureProperties)
						if (name == textureProperty.name)
							binding = textureProperty.binding;

				if (binding == UINT32_MAX)
				{
					Debug::LogError("No texture found named: " + name);
					return;
				}

				materialData->textureSets[i].textureHandles[binding] = texture->handleSRV;
			}
		}
		else
		{
			uint32_t textureID = ID;
			if (isBuffer)
				textureID = GetRenderBufferByIndex(ID)->renderBuffers[mCurrentFrame];

			auto texture = GetTextureByIndex(textureID);
			uint32_t binding = UINT32_MAX;

			for (auto& textureProperty : material->shader->reference->shaderInfo.fragProperties.textureProperties)
				if (name == textureProperty.name)
					binding = textureProperty.binding;

			if (binding == UINT32_MAX)
				for (auto& textureProperty : material->shader->reference->shaderInfo.vertProperties.textureProperties)
					if (name == textureProperty.name)
						binding = textureProperty.binding;

			if (binding == UINT32_MAX)
				for (auto& textureProperty : material->shader->reference->shaderInfo.geomProperties.textureProperties)
					if (name == textureProperty.name)
						binding = textureProperty.binding;

			if (binding == UINT32_MAX)
			{
				Debug::LogError("No texture found named: " + name);
				return;
			}

			materialData->textureSets[mCurrentFrame].textureHandles[binding] = texture->handleSRV;
		}
	}

	void RenderAPID3D12::SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		SetShaderTexture(material, name, ID, idx, allBuffer, isBuffer);
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

	uint32_t RenderAPID3D12::GetNextFBOIndex()
	{
		uint32_t length = static_cast<uint32_t>(mFBOArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mFBOArray[i]->inUse)
				return i;
		}

		mFBOArray.push_back(new ZXD3D12FBO());

		return length;
	}

	ZXD3D12FBO* RenderAPID3D12::GetFBOByIndex(uint32_t idx)
	{
		return mFBOArray[idx];
	}

	void RenderAPID3D12::DestroyFBOByIndex(uint32_t idx)
	{
		auto FBO = mFBOArray[idx];

		if (FBO->colorBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(FBO->colorBufferIdx);
			FBO->colorBufferIdx = UINT32_MAX;
		}
		if (FBO->depthBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(FBO->depthBufferIdx);
			FBO->depthBufferIdx = UINT32_MAX;
		}

		FBO->bufferType = FrameBufferType::Normal;
		FBO->clearInfo = {};

		FBO->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextRenderBufferIndex()
	{
		uint32_t length = static_cast<uint32_t>(mRenderBufferArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mRenderBufferArray[i]->inUse)
				return i;
		}

		auto newRenderBuffer = new ZXD3D12RenderBuffer();
		newRenderBuffer->renderBuffers.resize(DX_MAX_FRAMES_IN_FLIGHT);
		mRenderBufferArray.push_back(newRenderBuffer);

		return length;
	}

	ZXD3D12RenderBuffer* RenderAPID3D12::GetRenderBufferByIndex(uint32_t idx)
	{
		return mRenderBufferArray[idx];
	}

	void RenderAPID3D12::DestroyRenderBufferByIndex(uint32_t idx)
	{
		auto frameBuffer = mRenderBufferArray[idx];

		for (auto iter : frameBuffer->renderBuffers)
			DestroyTextureByIndex(iter);

		frameBuffer->renderBuffers.clear();
		frameBuffer->renderBuffers.resize(DX_MAX_FRAMES_IN_FLIGHT);

		frameBuffer->inUse = false;
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

		if (texture->usageFlags & ZX_D3D12_TEXTURE_USAGE_SRV_BIT)
		{
			ZXD3D12DescriptorManager::GetInstance()->ReleaseDescriptor(texture->handleSRV);
			texture->handleSRV = {};
		}
		if (texture->usageFlags & ZX_D3D12_TEXTURE_USAGE_RTV_BIT)
		{
			ZXD3D12DescriptorManager::GetInstance()->ReleaseDescriptor(texture->handleRTV);
			texture->handleRTV = {};
		}
		if (texture->usageFlags & ZX_D3D12_TEXTURE_USAGE_DSV_BIT)
		{
			ZXD3D12DescriptorManager::GetInstance()->ReleaseDescriptor(texture->handleDSV);
			texture->handleDSV = {};
		}
		texture->usageFlags = ZX_D3D12_TEXTURE_USAGE_NONE_BIT;

		// 不调这个也行，因为ComPtr<ID3D12Resource>有类似智能指针一样的内存管理，在适当的时候会自动释放
		texture->texture.Reset();

		texture->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextPipelineIndex()
	{
		uint32_t length = static_cast<uint32_t>(mPipelineArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mPipelineArray[i]->inUse)
				return i;
		}

		mPipelineArray.push_back(new ZXD3D12Pipeline());

		return length;
	}

	ZXD3D12Pipeline* RenderAPID3D12::GetPipelineByIndex(uint32_t idx)
	{
		return mPipelineArray[idx];
	}

	void RenderAPID3D12::DestroyPipelineByIndex(uint32_t idx)
	{
		auto pipeline = mPipelineArray[idx];

		pipeline->pipelineState.Reset();
		pipeline->rootSignature.Reset();

		pipeline->inUse = false;
	}
	
	uint32_t RenderAPID3D12::GetNextMaterialDataIndex()
	{
		uint32_t length = static_cast<uint32_t>(mMaterialDataArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mMaterialDataArray[i]->inUse)
				return i;
		}

		mMaterialDataArray.push_back(new ZXD3D12MaterialData());

		return length;
	}

	ZXD3D12MaterialData* RenderAPID3D12::GetMaterialDataByIndex(uint32_t idx)
	{
		return mMaterialDataArray[idx];
	}

	void RenderAPID3D12::DestroyMaterialDataByIndex(uint32_t idx)
	{
		auto materialData = mMaterialDataArray[idx];

		for (auto& iter : materialData->constantBuffers)
		{
			iter.constantBuffer->Unmap(0, nullptr);
			iter.constantBuffer.Reset();
		}
		materialData->constantBuffers.clear();

		for (auto& iter : materialData->textureSets)
			iter.textureHandles.clear();
		materialData->textureSets.clear();

		materialData->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextDrawCommandIndex()
	{
		uint32_t length = static_cast<uint32_t>(mDrawCommandArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mDrawCommandArray[i]->inUse)
				return i;
		}

		mDrawCommandArray.push_back(new ZXD3D12DrawCommand());

		return length;
	}

	ZXD3D12DrawCommand* RenderAPID3D12::GetDrawCommandByIndex(uint32_t idx)
	{
		return mDrawCommandArray[idx];
	}

	uint32_t RenderAPID3D12::CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc)
	{
		uint32_t textureID = GetNextTextureIndex();
		auto texture = GetTextureByIndex(textureID);

		texture->inUse = true;
		texture->texture = textureResource;
		texture->usageFlags = ZX_D3D12_TEXTURE_USAGE_RTV_BIT;
		texture->handleRTV = ZXD3D12DescriptorManager::GetInstance()->CreateDescriptor(texture->texture, rtvDesc);

		return textureID;
	}

	uint32_t RenderAPID3D12::CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc)
	{
		uint32_t textureID = GetNextTextureIndex();
		auto texture = GetTextureByIndex(textureID);

		texture->inUse = true;
		texture->texture = textureResource;
		texture->usageFlags = ZX_D3D12_TEXTURE_USAGE_SRV_BIT;
		texture->handleSRV = ZXD3D12DescriptorManager::GetInstance()->CreateDescriptor(texture->texture, srvDesc);

		return textureID;
	}

	uint32_t RenderAPID3D12::CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc)
	{
		uint32_t textureID = GetNextTextureIndex();
		auto texture = GetTextureByIndex(textureID);

		texture->inUse = true;
		texture->texture = textureResource;
		texture->usageFlags = ZX_D3D12_TEXTURE_USAGE_SRV_BIT | ZX_D3D12_TEXTURE_USAGE_RTV_BIT;
		texture->handleSRV = ZXD3D12DescriptorManager::GetInstance()->CreateDescriptor(texture->texture, srvDesc);
		texture->handleRTV = ZXD3D12DescriptorManager::GetInstance()->CreateDescriptor(texture->texture, rtvDesc);

		return textureID;
	}

	uint32_t RenderAPID3D12::CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, const D3D12_DEPTH_STENCIL_VIEW_DESC& dsvDesc)
	{
		uint32_t textureID = GetNextTextureIndex();
		auto texture = GetTextureByIndex(textureID);

		texture->inUse = true;
		texture->texture = textureResource;
		texture->usageFlags = ZX_D3D12_TEXTURE_USAGE_SRV_BIT | ZX_D3D12_TEXTURE_USAGE_DSV_BIT;
		texture->handleSRV = ZXD3D12DescriptorManager::GetInstance()->CreateDescriptor(texture->texture, srvDesc);
		texture->handleDSV = ZXD3D12DescriptorManager::GetInstance()->CreateDescriptor(texture->texture, dsvDesc);

		return textureID;
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

	ZXD3D12ConstantBuffer RenderAPID3D12::CreateConstantBuffer(UINT64 byteSize)
	{
		ZXD3D12ConstantBuffer constantBuffer;

		CD3DX12_HEAP_PROPERTIES constantBufferProps(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&constantBufferProps,
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer.constantBuffer))
		);

		ThrowIfFailed(constantBuffer.constantBuffer->Map(0, nullptr, static_cast<void**>(&constantBuffer.constantBufferAddress)));

		return constantBuffer;
	}


	uint32_t RenderAPID3D12::GetCurFrameBufferIndex()
	{
		if (mCurFBOIdx == mPresentFBOIdx)
			return mCurPresentIdx;
		else
			return mCurrentFrame;
	}

	ZXD3D12Fence* RenderAPID3D12::CreateZXD3D12Fence()
	{
		auto fence = new ZXD3D12Fence();
		ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence->fence)));
		return fence;
	}

	void RenderAPID3D12::SignalFence(ZXD3D12Fence* fence)
	{
		fence->currentFence++;
		ThrowIfFailed(mCommandQueue->Signal(fence->fence.Get(), fence->currentFence));
	}

	void RenderAPID3D12::WaitForFence(ZXD3D12Fence* fence)
	{
		// 如果Fence的进度还没到上一次设置的进度值
		if (fence->fence->GetCompletedValue() < fence->currentFence)
		{
			// 创建一个"进度抵达刚刚设置的信号量值"的事件
			auto event = CreateEventEx(nullptr, NULL, NULL, EVENT_ALL_ACCESS);
			if (event)
			{
				ThrowIfFailed(fence->fence->SetEventOnCompletion(fence->currentFence, event));
				// 等待事件
				WaitForSingleObject(event, INFINITE);
				CloseHandle(event);
			}
			else
			{
				Debug::LogError("Create event failed !");
			}
		}
	}

	void* RenderAPID3D12::GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx)
	{
		auto materialData = GetMaterialDataByIndex(materialDataID);

		for (auto& property : reference->shaderInfo.vertProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame].constantBufferAddress) + property.offset + property.arrayOffset * idx);

		for (auto& property : reference->shaderInfo.geomProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame].constantBufferAddress) + property.offset + property.arrayOffset * idx);

		for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame].constantBufferAddress) + property.offset + property.arrayOffset * idx);

		Debug::LogError("Could not find shader property named " + name);

		return nullptr;
	}

	vector<void*> RenderAPID3D12::GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx)
	{
		vector<void*> addresses;
		auto materialData = GetMaterialDataByIndex(materialDataID);

		for (auto& property : reference->shaderInfo.vertProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i].constantBufferAddress) + addressOffset));
				return addresses;
			}
		}

		for (auto& property : reference->shaderInfo.geomProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i].constantBufferAddress) + addressOffset));
				return addresses;
			}
		}

		for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i].constantBufferAddress) + addressOffset));
				return addresses;
			}
		}

		Debug::LogError("Could not find shader property named " + name);

		return addresses;
	}

	array<const CD3DX12_STATIC_SAMPLER_DESC, 4> RenderAPID3D12::GetStaticSamplersDesc()
	{
		// 线性插值采样，边缘重复
		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(0,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // W

		// 线性插值采样，边缘截断
		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(1,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // W

		// 各向异性采样，边缘重复
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(2,
			D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // W

		// 各向异性采样，边缘截断
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(3,
			D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // W

		return { linearWrap, linearClamp, anisotropicWrap, anisotropicClamp };
	}

	void RenderAPID3D12::InitImmediateExecution()
	{
		mImmediateExeFence = CreateZXD3D12Fence();

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

		// 设置Fence
		SignalFence(mImmediateExeFence);

		// 等待Fence
		WaitForFence(mImmediateExeFence);
	}
}