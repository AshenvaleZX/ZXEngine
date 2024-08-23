#include "RenderAPID3D12.h"
#include <stb_image.h>
#include <D3Dcompiler.h>
#include "Texture.h"
#include "ZShader.h"
#include "Material.h"
#include "Resources.h"
#include "GlobalData.h"
#include "FBOManager.h"
#include "MaterialData.h"
#include "ShaderParser.h"
#include "ProjectSetting.h"
#include "Window/WindowManager.h"
#include "DirectX12/ZXD3D12DescriptorManager.h"
#include "EventManager.h"
#ifdef ZX_EDITOR
#include "Editor/EditorGUIManager.h"
#include "Editor/ImGuiTextureManager.h"
#endif

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

		mEndRenderFence = CreateZXD3D12Fence();
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			mFrameFences.push_back(CreateZXD3D12Fence());
	}

	void RenderAPID3D12::InitD3D12()
	{
		UINT dxgiFactoryFlags = 0;

		// Debug
		if (ProjectSetting::enableGraphicsDebug)
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();

			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mDXGIFactory)));

		// ��һ�����ʵ��Կ�Ӳ��
		UINT adapterIndex = 0;
		UINT bestAdapterIndex = UINT_MAX;
		IDXGIAdapter1* pAdapter = nullptr;
		while (mDXGIFactory->EnumAdapters1(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 adapterDesc;
			pAdapter->GetDesc1(&adapterDesc);

			// 0x8086����Intel������ж���������û�ж����Կ�
			if (adapterDesc.VendorId != 0x8086 && !(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
				bestAdapterIndex = adapterIndex;

			pAdapter->Release();
			pAdapter = nullptr;

			adapterIndex++;
		}

		// ���û�ҵ����ʵ��Կ�����������if��pAdapter�ᱣ��Ϊ��
		if (bestAdapterIndex != UINT_MAX)
			mDXGIFactory->EnumAdapters1(bestAdapterIndex, &pAdapter);

		// ����ʹ��Ӳ���豸�����û�ҵ����ʵ��Կ���pAdapter��ʱΪnullptr�����������ָ���������ʾ�Զ���һ�����ʵ�
		HRESULT hardwareResult = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mD3D12Device));

		// ����ҵ��˺��ʵ��Կ���Ҫ�ֶ��ͷ�һ��Adapter
		if (bestAdapterIndex != UINT_MAX)
		{
			pAdapter->Release();
			pAdapter = nullptr;
		}

		// ���Ӳ��������֧�֣�ʹ��WARP�豸(Windows Advanced Rasterization Platform��΢�������ģ��Ĺ�դ��Ӳ���Կ�)
		if (FAILED(hardwareResult))
		{
			ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowIfFailed(mDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
			ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mD3D12Device)));
		}

		// ����Debug���ɸѡ
		if (ProjectSetting::enableGraphicsDebug)
		{
			// ��ȡ�豸����Ϣ����
			ComPtr<ID3D12InfoQueue> pInfoQueue;
			mD3D12Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
			
			// ����Ҫ�������Ϣ���
			D3D12_MESSAGE_CATEGORY categories[] = 
			{ 
				D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED,
				D3D12_MESSAGE_CATEGORY_MISCELLANEOUS,
				D3D12_MESSAGE_CATEGORY_INITIALIZATION,
				D3D12_MESSAGE_CATEGORY_CLEANUP,
				D3D12_MESSAGE_CATEGORY_COMPILATION,
				D3D12_MESSAGE_CATEGORY_STATE_CREATION, 
				D3D12_MESSAGE_CATEGORY_STATE_SETTING, 
				D3D12_MESSAGE_CATEGORY_STATE_GETTING, 
				D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION, 
				D3D12_MESSAGE_CATEGORY_EXECUTION, 
				D3D12_MESSAGE_CATEGORY_SHADER
			};

			// ����Ҫ�������Ϣ������
			D3D12_MESSAGE_SEVERITY severities[] = 
			{ 
				D3D12_MESSAGE_SEVERITY_CORRUPTION,
				D3D12_MESSAGE_SEVERITY_ERROR,
				D3D12_MESSAGE_SEVERITY_WARNING,
				D3D12_MESSAGE_SEVERITY_INFO, 
				D3D12_MESSAGE_SEVERITY_MESSAGE
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.AllowList.NumCategories = _countof(categories);
			filter.AllowList.pCategoryList = categories;
			filter.AllowList.NumSeverities = _countof(severities);
			filter.AllowList.pSeverityList = severities;

			pInfoQueue->PushStorageFilter(&filter);

			// �д���ʱֱ��Break
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		}

		// �����������
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(mD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
		
		// ����GPU�������ѣ�Shader Visible����������Ӳ�������������ƣ���: https://learn.microsoft.com/en-us/windows/win32/direct3d12/shader-visible-descriptor-heaps
		// ΢���ĵ�˵һ��Ӳ��Shader Visible�������ѿ����ڴ���96MB���ң� A one million member descriptor heap, with 32byte descriptors, would use up 32MB, for example.
		// ����һ����˵�������֧��300��������������������Ȳ�����ô��
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

		// ��ʼ����׷��ض���
		InitDXR();
	}

	void RenderAPID3D12::GetDeviceProperties()
	{
		mRtvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDsvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mCbvSrvUavDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// ��ȡ4X MSAA�����ȼ�
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
		// ���´���������Ҳ������������������Reset
		mSwapChain.Reset();

		// ����Ĵ�����ʽ�ο���Dear ImGui DX12��Demo�� https://www.3dgep.com/ �Ľ̳�
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = ProjectSetting::srcWidth;
		swapChainDesc.Height = ProjectSetting::srcHeight;
		swapChainDesc.Format = mPresentBufferFormat;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = mPresentBufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain1> dxgiSwapChain1;
		ThrowIfFailed(mDXGIFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), static_cast<HWND>(WindowManager::GetInstance()->GetWindow()), 
			&swapChainDesc, nullptr, nullptr, dxgiSwapChain1.GetAddressOf()));
		// Cast to SwapChain4
		ThrowIfFailed(dxgiSwapChain1.As(&mSwapChain));
	}

	void RenderAPID3D12::CreateSwapChainBuffers()
	{
		// �´���ʱ���¹��㣬Ҫ�ӵ�һ��Present Buffer��ʼʹ��
		mCurPresentIdx = 0;
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

	void RenderAPID3D12::InitAfterConstructor()
	{
		ZXD3D12DescriptorManager::Creat();
		CreateSwapChainBuffers();
	}

	void RenderAPID3D12::BeginFrame()
	{
		WaitForFence(mFrameFences[mCurrentFrame]);

		CheckDeleteData();

		if (mWindowResized)
			DoWindowSizeChange();

		mDynamicDescriptorOffsets[mCurrentFrame] = 0;
	}

	void RenderAPID3D12::EndFrame()
	{
		ThrowIfFailed(mSwapChain->Present(0, 0));

		mCurPresentIdx = (mCurPresentIdx + 1) % mPresentBufferCount;
		mCurrentFrame = (mCurrentFrame + 1) % DX_MAX_FRAMES_IN_FLIGHT;
	}

	void RenderAPID3D12::OnWindowSizeChange(uint32_t width, uint32_t height)
	{
		mNewWindowWidth = width;
		mNewWindowHeight = height;
		mWindowResized = true;
	}

	void RenderAPID3D12::SetRenderState(RenderStateSetting* state)
	{
		// D3D12����Ҫʵ������ӿ�
	}

	void RenderAPID3D12::SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset)
	{
		mViewPortInfo.width = width;
		mViewPortInfo.height = height;
		mViewPortInfo.xOffset = xOffset;

		// ����Ĳ����ǰ�0�������½ǵı�׼���ģ�Vulkan��0�������Ͻǣ������ƫ��(�༭��ģʽ)�Ļ���Y��ƫ����Ҫ���¼���һ��
		if (xOffset == 0 && yOffset == 0)
			mViewPortInfo.yOffset = yOffset;
		else
			mViewPortInfo.yOffset = ProjectSetting::srcHeight - height - yOffset;
	}

	void RenderAPID3D12::WaitForRenderFinish()
	{
		SignalFence(mEndRenderFence);
		WaitForFence(mEndRenderFence);
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
		// D3D12����Ҫʵ������ӿ�
	}

	void RenderAPID3D12::BlitFrameBuffer(uint32_t cmd, const string& src, const string& dst, FrameBufferPieceFlags flags)
	{
		auto drawCommand = GetDrawCommandByIndex(cmd);
		auto& allocator = drawCommand->allocators[mCurrentFrame];
		auto& drawCommandList = drawCommand->commandLists[mCurrentFrame];

		// ����Command List
		ThrowIfFailed(allocator->Reset());
		ThrowIfFailed(drawCommandList->Reset(allocator.Get(), nullptr));

		auto sFBO = FBOManager::GetInstance()->GetFBO(src);
		auto dFBO = FBOManager::GetInstance()->GetFBO(dst);

		if (flags & ZX_FRAME_BUFFER_PIECE_COLOR)
		{
			auto sColorBuffer = GetRenderBufferByIndex(sFBO->ColorBuffer);
			auto dColorBuffer = GetRenderBufferByIndex(dFBO->ColorBuffer);

			auto sColorTexture = GetTextureByIndex(sColorBuffer->renderBuffers[mCurrentFrame]);
			auto dColorTexture = GetTextureByIndex(dColorBuffer->renderBuffers[mCurrentFrame]);

			auto sColorTextureTransition = CD3DX12_RESOURCE_BARRIER::Transition(sColorTexture->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE);
			drawCommandList->ResourceBarrier(1, &sColorTextureTransition);
			auto dColorTextureTransition = CD3DX12_RESOURCE_BARRIER::Transition(dColorTexture->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
			drawCommandList->ResourceBarrier(1, &dColorTextureTransition);

			drawCommandList->CopyResource(dColorTexture->texture.Get(), sColorTexture->texture.Get());

			auto sColorTextureTransitionBack = CD3DX12_RESOURCE_BARRIER::Transition(sColorTexture->texture.Get(),
				D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ);
			drawCommandList->ResourceBarrier(1, &sColorTextureTransitionBack);
			auto dColorTextureTransitionBack = CD3DX12_RESOURCE_BARRIER::Transition(dColorTexture->texture.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
			drawCommandList->ResourceBarrier(1, &dColorTextureTransitionBack);
		}

		if (flags & ZX_FRAME_BUFFER_PIECE_DEPTH)
		{
			auto sDepthBuffer = GetRenderBufferByIndex(sFBO->DepthBuffer);
			auto dDepthBuffer = GetRenderBufferByIndex(dFBO->DepthBuffer);

			auto sDepthTexture = GetTextureByIndex(sDepthBuffer->renderBuffers[mCurrentFrame]);
			auto dDepthTexture = GetTextureByIndex(dDepthBuffer->renderBuffers[mCurrentFrame]);
			
			auto sDepthTextureTransition = CD3DX12_RESOURCE_BARRIER::Transition(sDepthTexture->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE);
			drawCommandList->ResourceBarrier(1, &sDepthTextureTransition);
			auto dDepthTextureTransition = CD3DX12_RESOURCE_BARRIER::Transition(dDepthTexture->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
			drawCommandList->ResourceBarrier(1, &dDepthTextureTransition);

			drawCommandList->CopyResource(dDepthTexture->texture.Get(), sDepthTexture->texture.Get());

			auto sDepthTextureTransitionBack = CD3DX12_RESOURCE_BARRIER::Transition(sDepthTexture->texture.Get(),
				D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ);
			drawCommandList->ResourceBarrier(1, &sDepthTextureTransitionBack);
			auto dDepthTextureTransitionBack = CD3DX12_RESOURCE_BARRIER::Transition(dDepthTexture->texture.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
			drawCommandList->ResourceBarrier(1, &dDepthTextureTransitionBack);
		}

		// �������ύCommand List
		ThrowIfFailed(drawCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { drawCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
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

		FBO->width = width;
		FBO->height = height;

		if (type == FrameBufferType::Normal || type == FrameBufferType::Deferred)
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
			D3D12FBO->bufferType = type;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// ����Color Buffer
				CD3DX12_HEAP_PROPERTIES colorBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC colorBufferDesc = {};
				colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				colorBufferDesc.Alignment = 0;
				colorBufferDesc.Width = width;
				colorBufferDesc.Height = height;
				colorBufferDesc.DepthOrArraySize = 1;
				colorBufferDesc.MipLevels = 1;
				colorBufferDesc.Format = mDefaultImageFormat;
				colorBufferDesc.SampleDesc.Count = 1;
				colorBufferDesc.SampleDesc.Quality = 0;
				colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				D3D12_CLEAR_VALUE optColorClear = {};
				optColorClear.Format = mDefaultImageFormat;
				optColorClear.Color[0] = clearInfo.color.r;
				optColorClear.Color[1] = clearInfo.color.g;
				optColorClear.Color[2] = clearInfo.color.b;
				optColorClear.Color[3] = clearInfo.color.a;

				ComPtr<ID3D12Resource> colorBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&colorBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&colorBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optColorClear,
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

				// ����Depth Buffer
				CD3DX12_HEAP_PROPERTIES depthBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC depthBufferDesc = {};
				depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				depthBufferDesc.Alignment = 0;
				depthBufferDesc.Width = width;
				depthBufferDesc.Height = height;
				depthBufferDesc.DepthOrArraySize = 1;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				depthBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				D3D12_CLEAR_VALUE optDepthClear = {};
				optDepthClear.Format = DXGI_FORMAT_D32_FLOAT;
				optDepthClear.DepthStencil.Depth = clearInfo.depth;
				optDepthClear.DepthStencil.Stencil = clearInfo.stencil;

				ComPtr<ID3D12Resource> depthBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&depthBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optDepthClear,
					IID_PPV_ARGS(&depthBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
				depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				depthSrvDesc.Texture2D.MipLevels = 1;
				depthSrvDesc.Texture2D.MostDetailedMip = 0;
				depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_DEPTH_STENCIL_VIEW_DESC depthDsvDesc = {};
				depthDsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				depthDsvDesc.Texture2D.MipSlice = 0;

				depthBuffer->renderBuffers[i] = CreateZXD3D12Texture(depthBufferResource, depthSrvDesc, depthDsvDesc);
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
			D3D12FBO->bufferType = FrameBufferType::Color;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// ����Color Buffer
				CD3DX12_HEAP_PROPERTIES colorBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC colorBufferDesc = {};
				colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				colorBufferDesc.Alignment = 0;
				colorBufferDesc.Width = width;
				colorBufferDesc.Height = height;
				colorBufferDesc.DepthOrArraySize = 1;
				colorBufferDesc.MipLevels = 1;
				colorBufferDesc.Format = mDefaultImageFormat;
				colorBufferDesc.SampleDesc.Count = 1;
				colorBufferDesc.SampleDesc.Quality = 0;
				colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				D3D12_CLEAR_VALUE optColorClear = {};
				optColorClear.Format = mDefaultImageFormat;
				optColorClear.Color[0] = clearInfo.color.r;
				optColorClear.Color[1] = clearInfo.color.g;
				optColorClear.Color[2] = clearInfo.color.b;
				optColorClear.Color[3] = clearInfo.color.a;

				ComPtr<ID3D12Resource> colorBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&colorBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&colorBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optColorClear,
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
		else if (type == FrameBufferType::ShadowMap)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = NULL;
			FBO->DepthBuffer = GetNextRenderBufferIndex();
			auto depthBuffer = GetRenderBufferByIndex(FBO->DepthBuffer);
			depthBuffer->inUse = true;

			auto D3D12FBO = GetFBOByIndex(FBO->ID);
			D3D12FBO->depthBufferIdx = FBO->DepthBuffer;
			D3D12FBO->bufferType = FrameBufferType::ShadowMap;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// ����Depth Buffer
				CD3DX12_HEAP_PROPERTIES depthBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC depthBufferDesc = {};
				depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				depthBufferDesc.Alignment = 0;
				depthBufferDesc.Width = width;
				depthBufferDesc.Height = height;
				depthBufferDesc.DepthOrArraySize = 1;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				depthBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				D3D12_CLEAR_VALUE optDepthClear = {};
				optDepthClear.Format = DXGI_FORMAT_D32_FLOAT;
				optDepthClear.DepthStencil.Depth = clearInfo.depth;
				optDepthClear.DepthStencil.Stencil = clearInfo.stencil;

				ComPtr<ID3D12Resource> depthBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&depthBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optDepthClear,
					IID_PPV_ARGS(&depthBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
				depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				depthSrvDesc.Texture2D.MipLevels = 1;
				depthSrvDesc.Texture2D.MostDetailedMip = 0;
				depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_DEPTH_STENCIL_VIEW_DESC depthDsvDesc = {};
				depthDsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				depthDsvDesc.Texture2D.MipSlice = 0;

				depthBuffer->renderBuffers[i] = CreateZXD3D12Texture(depthBufferResource, depthSrvDesc, depthDsvDesc);
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
			D3D12FBO->bufferType = FrameBufferType::ShadowCubeMap;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// ����Depth Buffer
				CD3DX12_HEAP_PROPERTIES depthBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC depthBufferDesc = {};
				depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				depthBufferDesc.Alignment = 0;
				depthBufferDesc.Width = width;
				depthBufferDesc.Height = height;
				depthBufferDesc.DepthOrArraySize = 6;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				depthBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				D3D12_CLEAR_VALUE optDepthClear = {};
				optDepthClear.Format = DXGI_FORMAT_D32_FLOAT;
				optDepthClear.DepthStencil.Depth = clearInfo.depth;
				optDepthClear.DepthStencil.Stencil = clearInfo.stencil;

				ComPtr<ID3D12Resource> depthBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&depthBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optDepthClear,
					IID_PPV_ARGS(&depthBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
				depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				depthSrvDesc.Texture2D.MipLevels = 1;
				depthSrvDesc.Texture2D.MostDetailedMip = 0;
				depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_DEPTH_STENCIL_VIEW_DESC depthDsvDesc = {};
				depthDsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				depthDsvDesc.Texture2DArray.MipSlice = 0;
				depthDsvDesc.Texture2DArray.ArraySize = 6;
				depthDsvDesc.Texture2DArray.FirstArraySlice = 0;

				depthBuffer->renderBuffers[i] = CreateZXD3D12Texture(depthBufferResource, depthSrvDesc, depthDsvDesc);
			}

			D3D12FBO->inUse = true;
		}
		else if (type == FrameBufferType::GBuffer)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = GetNextRenderBufferIndex();
			auto colorBuffer = GetRenderBufferByIndex(FBO->ColorBuffer);
			colorBuffer->inUse = true;
			FBO->DepthBuffer = GetNextRenderBufferIndex();
			auto depthBuffer = GetRenderBufferByIndex(FBO->DepthBuffer);
			depthBuffer->inUse = true;
			FBO->PositionBuffer = GetNextRenderBufferIndex();
			auto positionBuffer = GetRenderBufferByIndex(FBO->PositionBuffer);
			positionBuffer->inUse = true;
			FBO->NormalBuffer = GetNextRenderBufferIndex();
			auto normalBuffer = GetRenderBufferByIndex(FBO->NormalBuffer);
			normalBuffer->inUse = true;

			auto D3D12FBO = GetFBOByIndex(FBO->ID);
			D3D12FBO->colorBufferIdx = FBO->ColorBuffer;
			D3D12FBO->depthBufferIdx = FBO->DepthBuffer;
			D3D12FBO->positionBufferIdx = FBO->PositionBuffer;
			D3D12FBO->normalBufferIdx = FBO->NormalBuffer;
			D3D12FBO->bufferType = FrameBufferType::GBuffer;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// Position Buffer
				CD3DX12_HEAP_PROPERTIES posBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC posBufferDesc = {};
				posBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				posBufferDesc.Alignment = 0;
				posBufferDesc.Width = width;
				posBufferDesc.Height = height;
				posBufferDesc.DepthOrArraySize = 1;
				posBufferDesc.MipLevels = 1;
				posBufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				posBufferDesc.SampleDesc.Count = 1;
				posBufferDesc.SampleDesc.Quality = 0;
				posBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				posBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
				
				D3D12_CLEAR_VALUE optPosClear = {};
				optPosClear.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				optPosClear.Color[0] = clearInfo.color.r;
				optPosClear.Color[1] = clearInfo.color.g;
				optPosClear.Color[2] = clearInfo.color.b;
				optPosClear.Color[3] = clearInfo.color.a;

				ComPtr<ID3D12Resource> posBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&posBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&posBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optPosClear,
					IID_PPV_ARGS(&posBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC posSrvDesc = {};
				posSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				posSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				posSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				posSrvDesc.Texture2D.MipLevels = 1;
				posSrvDesc.Texture2D.MostDetailedMip = 0;
				posSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_RENDER_TARGET_VIEW_DESC posRtvDesc = {};
				posRtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				posRtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				posRtvDesc.Texture2D.MipSlice = 0;

				positionBuffer->renderBuffers[i] = CreateZXD3D12Texture(posBufferResource, posSrvDesc, posRtvDesc);

				// Normal Buffer
				CD3DX12_HEAP_PROPERTIES normalBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC normalBufferDesc = {};
				normalBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				normalBufferDesc.Alignment = 0;
				normalBufferDesc.Width = width;
				normalBufferDesc.Height = height;
				normalBufferDesc.DepthOrArraySize = 1;
				normalBufferDesc.MipLevels = 1;
				normalBufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				normalBufferDesc.SampleDesc.Count = 1;
				normalBufferDesc.SampleDesc.Quality = 0;
				normalBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				normalBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				D3D12_CLEAR_VALUE optNormalClear = {};
				optNormalClear.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				optNormalClear.Color[0] = clearInfo.color.r;
				optNormalClear.Color[1] = clearInfo.color.g;
				optNormalClear.Color[2] = clearInfo.color.b;
				optNormalClear.Color[3] = clearInfo.color.a;

				ComPtr<ID3D12Resource> normalBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&normalBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&normalBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optNormalClear,
					IID_PPV_ARGS(&normalBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC normalSrvDesc = {};
				normalSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				normalSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				normalSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				normalSrvDesc.Texture2D.MipLevels = 1;
				normalSrvDesc.Texture2D.MostDetailedMip = 0;
				normalSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_RENDER_TARGET_VIEW_DESC normalRtvDesc = {};
				normalRtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				normalRtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				normalRtvDesc.Texture2D.MipSlice = 0;

				normalBuffer->renderBuffers[i] = CreateZXD3D12Texture(normalBufferResource, normalSrvDesc, normalRtvDesc);

				// ����Color Buffer
				CD3DX12_HEAP_PROPERTIES colorBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC colorBufferDesc = {};
				colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				colorBufferDesc.Alignment = 0;
				colorBufferDesc.Width = width;
				colorBufferDesc.Height = height;
				colorBufferDesc.DepthOrArraySize = 1;
				colorBufferDesc.MipLevels = 1;
				colorBufferDesc.Format = mDefaultImageFormat;
				colorBufferDesc.SampleDesc.Count = 1;
				colorBufferDesc.SampleDesc.Quality = 0;
				colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				D3D12_CLEAR_VALUE optColorClear = {};
				optColorClear.Format = mDefaultImageFormat;
				optColorClear.Color[0] = clearInfo.color.r;
				optColorClear.Color[1] = clearInfo.color.g;
				optColorClear.Color[2] = clearInfo.color.b;
				optColorClear.Color[3] = clearInfo.color.a;

				ComPtr<ID3D12Resource> colorBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&colorBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&colorBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optColorClear,
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

				// ����Depth Buffer
				CD3DX12_HEAP_PROPERTIES depthBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC depthBufferDesc = {};
				depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				depthBufferDesc.Alignment = 0;
				depthBufferDesc.Width = width;
				depthBufferDesc.Height = height;
				depthBufferDesc.DepthOrArraySize = 1;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				depthBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				D3D12_CLEAR_VALUE optDepthClear = {};
				optDepthClear.Format = DXGI_FORMAT_D32_FLOAT;
				optDepthClear.DepthStencil.Depth = clearInfo.depth;
				optDepthClear.DepthStencil.Stencil = clearInfo.stencil;

				ComPtr<ID3D12Resource> depthBufferResource;
				ThrowIfFailed(mD3D12Device->CreateCommittedResource(
					&depthBufferProps,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					&optDepthClear,
					IID_PPV_ARGS(&depthBufferResource)
				));

				D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
				depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				depthSrvDesc.Texture2D.MipLevels = 1;
				depthSrvDesc.Texture2D.MostDetailedMip = 0;
				depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				D3D12_DEPTH_STENCIL_VIEW_DESC depthDsvDesc = {};
				depthDsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				depthDsvDesc.Texture2D.MipSlice = 0;

				depthBuffer->renderBuffers[i] = CreateZXD3D12Texture(depthBufferResource, depthSrvDesc, depthDsvDesc);
			}

			D3D12FBO->inUse = true;
		}
		else if (type == FrameBufferType::RayTracing)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = GetNextRenderBufferIndex();
			auto colorBuffer = GetRenderBufferByIndex(FBO->ColorBuffer);
			colorBuffer->inUse = true;
			FBO->DepthBuffer = NULL;

			auto D3D12FBO = GetFBOByIndex(FBO->ID);
			D3D12FBO->colorBufferIdx = FBO->ColorBuffer;
			D3D12FBO->bufferType = FrameBufferType::RayTracing;
			D3D12FBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// ����Color Buffer
				CD3DX12_HEAP_PROPERTIES colorBufferProps(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC colorBufferDesc = {};
				colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				colorBufferDesc.Alignment = 0;
				colorBufferDesc.Width = width;
				colorBufferDesc.Height = height;
				colorBufferDesc.DepthOrArraySize = 1;
				colorBufferDesc.MipLevels = 1;
				colorBufferDesc.Format = mDefaultImageFormat;
				colorBufferDesc.SampleDesc.Count = 1;
				colorBufferDesc.SampleDesc.Quality = 0;
				colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

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

				colorBuffer->renderBuffers[i] = CreateZXD3D12Texture(colorBufferResource, colorSrvDesc);
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

	uint32_t RenderAPID3D12::CreateStaticInstanceBuffer(uint32_t size, uint32_t num, const void* data)
	{
		uint32_t idx = GetNextInstanceBufferIndex();
		auto instanceBuffer = GetInstanceBufferByIndex(idx);
		instanceBuffer->inUse = true;

		UINT64 stride = static_cast<UINT64>(size) * sizeof(Vector4);
		UINT64 bufferSize = static_cast<UINT64>(num) * stride;

		instanceBuffer->buffer = CreateBuffer(bufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_HEAP_TYPE_DEFAULT, false, true, data);

		instanceBuffer->view.SizeInBytes = static_cast<UINT>(bufferSize);
		instanceBuffer->view.StrideInBytes = static_cast<UINT>(stride);
		instanceBuffer->view.BufferLocation = instanceBuffer->buffer.gpuAddress;

		return idx;
	}

	uint32_t RenderAPID3D12::CreateDynamicInstanceBuffer(uint32_t size, uint32_t num)
	{
		uint32_t idx = GetNextInstanceBufferIndex();
		auto instanceBuffer = GetInstanceBufferByIndex(idx);
		instanceBuffer->inUse = true;

		UINT64 stride = static_cast<UINT64>(size) * sizeof(Vector4);
		UINT64 bufferSize = static_cast<UINT64>(num) * stride;

		instanceBuffer->buffer = CreateBuffer(bufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_HEAP_TYPE_UPLOAD, true, true);
		
		instanceBuffer->view.SizeInBytes = static_cast<UINT>(bufferSize);
		instanceBuffer->view.StrideInBytes = static_cast<UINT>(stride);
		instanceBuffer->view.BufferLocation = instanceBuffer->buffer.gpuAddress;

		return idx;
	}

	void RenderAPID3D12::UpdateDynamicInstanceBuffer(uint32_t id, uint32_t size, uint32_t num, const void* data)
	{
		auto instanceBuffer = GetInstanceBufferByIndex(id);

		UINT64 bufferSize = static_cast<UINT64>(static_cast<size_t>(num * size) * sizeof(Vector4));

		memcpy(instanceBuffer->buffer.cpuAddress, data, bufferSize);
	}

	void RenderAPID3D12::SetUpInstanceBufferAttribute(uint32_t VAO, uint32_t instanceBuffer, uint32_t size, uint32_t offset)
	{
		// D3D12����Ҫʵ������ӿ�
		return;
	}

	void RenderAPID3D12::DeleteInstanceBuffer(uint32_t id)
	{
		mInstanceBuffersToDelete.insert(pair(id, DX_MAX_FRAMES_IN_FLIGHT));
	}

	unsigned int RenderAPID3D12::LoadTexture(const char* path, int& width, int& height)
	{
		int nrComponents;
		stbi_uc* pixels = stbi_load(path, &width, &height, &nrComponents, STBI_rgb_alpha);

		// ����������Դ
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

		// ���������ϴ���
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

		// �ϴ���������
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
		{
			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pixels;
			subresourceData.RowPitch = static_cast<LONG_PTR>(width * 4);
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(cmdList.Get(),
				textureResource.Get(),
				uploadHeap.Get(),
				0, 0, 1, &subresourceData);

			// ת������״̬
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
			imageData[i] = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
			if (!imageData[i])
				Debug::LogError("Failed to load texture file: " + faces[i]);
		}

		// ����CubeMap��Դ
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

		// ����CubeMap�ϴ���
		UINT64 uploadHeapSize;
		mD3D12Device->GetCopyableFootprints(&cubeMapDesc, 0, 6, 0, nullptr, nullptr, nullptr, &uploadHeapSize);
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

		// �ϴ���������
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
		{
			D3D12_SUBRESOURCE_DATA cubeMapData[6] = {};
			for (int i = 0; i < 6; ++i)
			{
				cubeMapData[i].pData = imageData[i];
				cubeMapData[i].RowPitch = static_cast<LONG_PTR>(width * 4);
				cubeMapData[i].SlicePitch = cubeMapData[i].RowPitch * static_cast<LONG_PTR>(height);
			}

			UpdateSubresources(cmdList.Get(),
				cubeMapResource.Get(),
				uploadHeap.Get(),
				0, 0, 6, cubeMapData);

			// ת������״̬
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
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		return CreateZXD3D12Texture(cubeMapResource, srvDesc);
	}

	unsigned int RenderAPID3D12::CreateTexture(TextureFullData* data)
	{
		// ����������Դ
		CD3DX12_HEAP_PROPERTIES textureProps(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC textureDesc(CD3DX12_RESOURCE_DESC::Tex2D(mDefaultImageFormat, data->width, data->height, 1, 1));
		ComPtr<ID3D12Resource> textureResource;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&textureProps,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&textureResource)
		));

		// ���������ϴ���
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

		// �ϴ���������
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
		{
			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = data->data;
			subresourceData.RowPitch = static_cast<LONG_PTR>(data->width * 4);
			subresourceData.SlicePitch = subresourceData.RowPitch * data->height;

			UpdateSubresources(cmdList.Get(),
				textureResource.Get(),
				uploadHeap.Get(),
				0, 0, 1, &subresourceData);

			// ת������״̬
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

	unsigned int RenderAPID3D12::CreateCubeMap(CubeMapFullData* data)
	{
		// ����CubeMap��Դ
		CD3DX12_HEAP_PROPERTIES cubeMapHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC cubeMapDesc = {};
		cubeMapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		cubeMapDesc.Alignment = 0;
		cubeMapDesc.Width = data->width;
		cubeMapDesc.Height = data->height;
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

		// ����CubeMap�ϴ���
		UINT64 uploadHeapSize;
		mD3D12Device->GetCopyableFootprints(&cubeMapDesc, 0, 6, 0, nullptr, nullptr, nullptr, &uploadHeapSize);
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

		// �ϴ���������
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
		{
			D3D12_SUBRESOURCE_DATA cubeMapData[6] = {};
			for (int i = 0; i < 6; ++i)
			{
				cubeMapData[i].pData = data->data[i];
				cubeMapData[i].RowPitch = static_cast<LONG_PTR>(data->width * 4);
				cubeMapData[i].SlicePitch = cubeMapData[i].RowPitch * static_cast<LONG_PTR>(data->height);
			}

			UpdateSubresources(cmdList.Get(),
				cubeMapResource.Get(),
				uploadHeap.Get(),
				0, 0, 6, cubeMapData);

			// ת������״̬
			CD3DX12_RESOURCE_BARRIER copyFinishBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				cubeMapResource.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);

			cmdList->ResourceBarrier(1, &copyFinishBarrier);
		});

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = mDefaultImageFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		return CreateZXD3D12Texture(cubeMapResource, srvDesc);
	}

	unsigned int RenderAPID3D12::GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data)
	{
		// ����������Դ
		CD3DX12_HEAP_PROPERTIES textureProps(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC textureDesc(CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8_UNORM, width, height, 1, 1));
		ComPtr<ID3D12Resource> textureResource;
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&textureProps,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&textureResource)
		));

		// ���������ϴ���
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

		// �ϴ���������
		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
		{
			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = data;
			subresourceData.RowPitch = static_cast<LONG_PTR>(width);
			subresourceData.SlicePitch = subresourceData.RowPitch * static_cast<LONG_PTR>(height);

			UpdateSubresources(cmdList.Get(),
				textureResource.Get(),
				uploadHeap.Get(),
				0, 0, 1, &subresourceData);

			// ת������״̬
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				textureResource.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);

			cmdList->ResourceBarrier(1, &barrier);
		});

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		return CreateZXD3D12Texture(textureResource, srvDesc);
	}

	void RenderAPID3D12::DeleteTexture(unsigned int id)
	{
		mTexturesToDelete.insert(pair(id, DX_MAX_FRAMES_IN_FLIGHT));
	}

	ShaderReference* RenderAPID3D12::LoadAndSetUpShader(const string& path, FrameBufferType type)
	{
		string shaderCode = Resources::LoadTextFile(path);
		return SetUpShader(path, shaderCode, type);
	}

	ShaderReference* RenderAPID3D12::SetUpShader(const string& path, const string& shaderCode, FrameBufferType type)
	{
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::D3D12);
		string hlslCode = ShaderParser::TranslateToD3D12(shaderCode, shaderInfo);

		UINT compileFlags = 0;
		if (ProjectSetting::enableGraphicsDebug)
			compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

		// ����Vertex Shader
		ComPtr<ID3DBlob> vertCode = nullptr;
		if (shaderInfo.stages & ZX_SHADER_STAGE_VERTEX_BIT)
		{
			ComPtr<ID3DBlob> errors;
			HRESULT res = D3DCompile(hlslCode.c_str(), hlslCode.length(), NULL, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"VS", "vs_5_1", compileFlags, 0, &vertCode, &errors);
			if (errors != nullptr)
				Debug::LogError((char*)errors->GetBufferPointer());
			ThrowIfFailed(res);
		}
		// ����Geometry Shader
		ComPtr<ID3DBlob> geomCode = nullptr;
		if (shaderInfo.stages & ZX_SHADER_STAGE_GEOMETRY_BIT)
		{
			ComPtr<ID3DBlob> errors;
			HRESULT res = D3DCompile(hlslCode.c_str(), hlslCode.length(), NULL, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"GS", "gs_5_1", compileFlags, 0, &geomCode, &errors);
			if (errors != nullptr)
				Debug::LogError((char*)errors->GetBufferPointer());
			ThrowIfFailed(res);
		}
		// ����Fragment(Pixel) Shader
		ComPtr<ID3DBlob> fragCode = nullptr;
		if (shaderInfo.stages & ZX_SHADER_STAGE_FRAGMENT_BIT)
		{
			ComPtr<ID3DBlob> errors;
			HRESULT res = D3DCompile(hlslCode.c_str(), hlslCode.length(), NULL, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"PS", "ps_5_1", compileFlags, 0, &fragCode, &errors);
			if (errors != nullptr)
				Debug::LogError((char*)errors->GetBufferPointer());
			ThrowIfFailed(res);
		}

		// ׼������D3D12��������
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};

		// ͳ����������
		UINT textureNum = static_cast<UINT>(
			shaderInfo.vertProperties.textureProperties.size() +
			shaderInfo.geomProperties.textureProperties.size() +
			shaderInfo.fragProperties.textureProperties.size());

		// ������ǩ��
		ComPtr<ID3D12RootSignature> rootSignature;
		{
			// �����ŵ����棬��������������
			CD3DX12_DESCRIPTOR_RANGE descriptorRange = {};

			vector<CD3DX12_ROOT_PARAMETER> rootParameters = {};
			if (textureNum > 0)
			{
				rootParameters.resize(2);
				rootParameters[0].InitAsConstantBufferView(0);
				
				descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, textureNum, 0, 0);
				rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_ALL);
			}
			else
			{
				rootParameters.resize(1);
				rootParameters[0].InitAsConstantBufferView(0);
			}

			auto samplers = GetStaticSamplersDesc();

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(static_cast<UINT>(rootParameters.size()), rootParameters.data(),
				static_cast<UINT>(samplers.size()), samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> error;
			ComPtr<ID3DBlob> serializedRootSignature;
			HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &error);
			if (error != nullptr)
				Debug::LogError((char*)error->GetBufferPointer());
			ThrowIfFailed(hr);

			ThrowIfFailed(mD3D12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
			pipelineStateDesc.pRootSignature = rootSignature.Get();
		}

		// ���Shader
		if (shaderInfo.stages & ZX_SHADER_STAGE_VERTEX_BIT)
			pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(vertCode->GetBufferPointer()), vertCode->GetBufferSize() };
		if (shaderInfo.stages & ZX_SHADER_STAGE_GEOMETRY_BIT)
			pipelineStateDesc.GS = { reinterpret_cast<BYTE*>(geomCode->GetBufferPointer()), geomCode->GetBufferSize() };
		if (shaderInfo.stages & ZX_SHADER_STAGE_FRAGMENT_BIT)
			pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(fragCode->GetBufferPointer()), fragCode->GetBufferSize() };

		// Input Layout
		vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, Position),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(Vertex, TexCoords), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, Normal),    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, Tangent),   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "WEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, Weights),   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEID",   0, DXGI_FORMAT_R32G32B32A32_UINT,  0, offsetof(Vertex, BoneIDs),   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		if (shaderInfo.instanceInfo.size > 0)
		{
			UINT offset = 0;
			for (auto& iter : shaderInfo.instanceInfo.attributes)
			{
				if (iter.first == ShaderPropertyType::VEC4)
				{
					inputElementDescs.emplace_back(iter.second.c_str(), 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offset, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1);
					offset += 16;
				}
				else if (iter.first == ShaderPropertyType::MAT4)
				{
					for (UINT i = 0; i < 4; ++i)
					{
						inputElementDescs.emplace_back(iter.second.c_str(), i, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offset, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1);
						offset += 16;
					}
				}
			}
		}

		pipelineStateDesc.InputLayout = { inputElementDescs.data(), static_cast<UINT>(inputElementDescs.size()) };

		// Blend Config
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE; // ���Render TargetҪ��Ҫ�ֿ����û�Ϸ�ʽ
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

		// ��դ���׶�
		D3D12_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode = dxFaceCullOptionMap[shaderInfo.stateSet.cull];
		rasterizerDesc.FrontCounterClockwise = TRUE;
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
		if (shaderInfo.stateSet.depthCompareOp == CompareOption::ALWAYS || type == FrameBufferType::Present || type == FrameBufferType::Color)
			depthStencilDesc.DepthEnable = FALSE;
		else
			depthStencilDesc.DepthEnable = TRUE;
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
		pipelineStateDesc.SampleDesc.Count = 1;
		pipelineStateDesc.SampleDesc.Quality = 0;
		pipelineStateDesc.NodeMask = 0; // ����GPU�õģ���ʱ���ù�
		if (type == FrameBufferType::GBuffer)
		{
			pipelineStateDesc.NumRenderTargets = 3;
			pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			pipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			pipelineStateDesc.RTVFormats[2] = mDefaultImageFormat;
		}
		else
		{
			pipelineStateDesc.NumRenderTargets = 1;
			pipelineStateDesc.RTVFormats[0] = mDefaultImageFormat;
		}

		// �������DSV����ʽ��Ҫ����ΪUNKNOWN
		if (type == FrameBufferType::Present || type == FrameBufferType::Color)
			pipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		else
			pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		// ����D3D12����
		ComPtr<ID3D12PipelineState> PSO;
		ThrowIfFailed(mD3D12Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&PSO)));

		uint32_t pipelineID = GetNextPipelineIndex();
		auto pipeline = GetPipelineByIndex(pipelineID);
		pipeline->textureNum = textureNum;
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
		mShadersToDelete.insert(pair(id, DX_MAX_FRAMES_IN_FLIGHT));
	}

	uint32_t RenderAPID3D12::CreateMaterialData()
	{
		uint32_t materialDataID = GetNextMaterialDataIndex();
		auto materialData = GetMaterialDataByIndex(materialDataID);

		materialData->inUse = true;

		return materialDataID;
	}

	void RenderAPID3D12::SetUpMaterial(Material* material)
	{
		auto shaderReference = material->shader->reference;
		auto materialDataZXD3D12 = GetMaterialDataByIndex(material->data->GetID());

		// ����Constant Buffer��С
		UINT64 bufferSize = 0;
		if (shaderReference->shaderInfo.fragProperties.baseProperties.size() > 0)
			bufferSize = static_cast<UINT64>(shaderReference->shaderInfo.fragProperties.baseProperties.back().offset + shaderReference->shaderInfo.fragProperties.baseProperties.back().size);
		else if (shaderReference->shaderInfo.geomProperties.baseProperties.size() > 0)
			bufferSize = static_cast<UINT64>(shaderReference->shaderInfo.geomProperties.baseProperties.back().offset + shaderReference->shaderInfo.geomProperties.baseProperties.back().size);
		else if (shaderReference->shaderInfo.vertProperties.baseProperties.size() > 0)
			bufferSize = static_cast<UINT64>(shaderReference->shaderInfo.vertProperties.baseProperties.back().offset + shaderReference->shaderInfo.vertProperties.baseProperties.back().size);

		// ����ȡ256������(���Ǳ�Ҫ����)
		bufferSize = (bufferSize + 255) & ~255;
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			// ����Constant Buffer
			if (bufferSize > 0)
				materialDataZXD3D12->constantBuffers.push_back(CreateBuffer(bufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, true, true));

			ZXD3D12MaterialTextureSet materialTextureSet;
			materialTextureSet.textureHandles.resize(
				shaderReference->shaderInfo.vertProperties.textureProperties.size() +
				shaderReference->shaderInfo.geomProperties.textureProperties.size() +
				shaderReference->shaderInfo.fragProperties.textureProperties.size()
			);

			for (auto& matTexture : material->data->textures)
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

		// ���ò�������
		for (auto& property : material->data->vec2Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->vec3Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->vec4Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->floatDatas)
			SetShaderScalar(material, property.first, property.second, true);
		for (auto& property : material->data->uintDatas)
			SetShaderScalar(material, property.first, property.second, true);
		for (auto& property : material->data->colorDatas)
			SetShaderVector(material, property.first, property.second, true);

		material->data->initialized = true;
	}

	void RenderAPID3D12::UseMaterialData(uint32_t ID)
	{
		mCurMaterialDataIdx = ID;
	}

	void RenderAPID3D12::DeleteMaterialData(uint32_t id)
	{
		mMaterialDatasToDelete.insert(pair(id, DX_MAX_FRAMES_IN_FLIGHT));
	}

	uint32_t RenderAPID3D12::AllocateDrawCommand(CommandType commandType)
	{
		uint32_t idx = GetNextDrawCommandIndex();
		auto drawCmd = GetDrawCommandByIndex(idx);
		drawCmd->commandType = commandType;

		drawCmd->allocators.resize(DX_MAX_FRAMES_IN_FLIGHT);
		drawCmd->commandLists.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			ThrowIfFailed(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(drawCmd->allocators[i].GetAddressOf())));

			ThrowIfFailed(mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, drawCmd->allocators[i].Get(), nullptr,
				IID_PPV_ARGS(drawCmd->commandLists[i].GetAddressOf())));

			// ʹ��CommandList��ʱ�����Reset��ResetʱҪ����Close״̬
			drawCmd->commandLists[i]->Close();
		}

		drawCmd->inUse = true;

		return idx;
	}

	void RenderAPID3D12::Draw(uint32_t VAO)
	{
		mDrawRecords.emplace_back(VAO, mCurPipeLineIdx, mCurMaterialDataIdx, 0, UINT32_MAX);
	}

	void RenderAPID3D12::DrawInstanced(uint32_t VAO, uint32_t instanceNum, uint32_t instanceBuffer)
	{
		mDrawRecords.emplace_back(VAO, mCurPipeLineIdx, mCurMaterialDataIdx, instanceNum, instanceBuffer);
	}

	void RenderAPID3D12::GenerateDrawCommand(uint32_t id)
	{
		auto drawCommand = GetDrawCommandByIndex(id);
		auto& allocator = drawCommand->allocators[mCurrentFrame];
		auto& drawCommandList = drawCommand->commandLists[mCurrentFrame];

		// ����Command List
		ThrowIfFailed(allocator->Reset());
		ThrowIfFailed(drawCommandList->Reset(allocator.Get(), nullptr));

		auto curFBO = GetFBOByIndex(mCurFBOIdx);
		ZXD3D12Texture* colorBuffer = nullptr;
		ZXD3D12Texture* depthBuffer = nullptr;
		ZXD3D12Texture* positionBuffer = nullptr;
		ZXD3D12Texture* normalBuffer = nullptr;

		if (curFBO->bufferType == FrameBufferType::Normal)
		{
			// ��ȡ��ȾĿ��Buffer
			colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			depthBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->depthBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			
			// �л�Ϊд��״̬
			auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			drawCommandList->ResourceBarrier(1, &depthBufferTransition);

			// �л�Ŀ��Buffer
			auto rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
			auto dsv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(depthBuffer->handleDSV);
			drawCommandList->OMSetRenderTargets(1, &rtv, false, &dsv);

			// ��������
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
		else if (curFBO->bufferType == FrameBufferType::GBuffer)
		{
			positionBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->positionBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			normalBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->normalBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			depthBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->depthBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);

			auto positionBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(positionBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &positionBufferTransition);
			auto normalBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(normalBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &normalBufferTransition);
			auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			drawCommandList->ResourceBarrier(1, &depthBufferTransition);

			auto pos_rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(positionBuffer->handleRTV);
			auto normal_rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(normalBuffer->handleRTV);
			auto color_rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
			array<D3D12_CPU_DESCRIPTOR_HANDLE, 3> rtvs = { pos_rtv, normal_rtv, color_rtv };

			auto dsv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(depthBuffer->handleDSV);
			drawCommandList->OMSetRenderTargets(3, rtvs.data(), false, &dsv);

			auto& clearInfo = curFBO->clearInfo;
			const float clearColor[] = { clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a };
			drawCommandList->ClearRenderTargetView(pos_rtv, clearColor, 0, nullptr);
			drawCommandList->ClearRenderTargetView(normal_rtv, clearColor, 0, nullptr);
			drawCommandList->ClearRenderTargetView(color_rtv, clearColor, 0, nullptr);
			drawCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, clearInfo.depth, 0, 0, nullptr);
		}
		else if (curFBO->bufferType == FrameBufferType::Deferred)
		{
			colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);
			depthBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->depthBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);

			auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			auto depthBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(depthBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			drawCommandList->ResourceBarrier(1, &depthBufferTransition);

			auto rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
			auto dsv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(depthBuffer->handleDSV);
			drawCommandList->OMSetRenderTargets(1, &rtv, false, &dsv);

			// Deferred Buffer����Ȼ�������G-Buffer��������
			auto& clearInfo = curFBO->clearInfo;
			const float clearColor[] = { clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a };
			drawCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		}
		else if (curFBO->bufferType == FrameBufferType::Present)
		{
			colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);

			auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			drawCommandList->ResourceBarrier(1, &colorBufferTransition);

			auto rtv = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(colorBuffer->handleRTV);
			drawCommandList->OMSetRenderTargets(1, &rtv, false, nullptr);
		}

		// ����Viewport
		D3D12_VIEWPORT viewPort = {};
		viewPort.Width    = static_cast<FLOAT>(mViewPortInfo.width);
		viewPort.Height   = static_cast<FLOAT>(mViewPortInfo.height);
		viewPort.TopLeftX = static_cast<FLOAT>(mViewPortInfo.xOffset);
		viewPort.TopLeftY = static_cast<FLOAT>(mViewPortInfo.yOffset);
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		drawCommandList->RSSetViewports(1, &viewPort);
		// ����Scissor
		D3D12_RECT scissor = {};
		scissor.left   = mViewPortInfo.xOffset;
		scissor.top    = mViewPortInfo.yOffset;
		scissor.right  = mViewPortInfo.xOffset + mViewPortInfo.width;
		scissor.bottom = mViewPortInfo.yOffset + mViewPortInfo.height;
		drawCommandList->RSSetScissorRects(1, &scissor);

		// ����ͼԪ����
		drawCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ���õ�ǰ֡�Ķ�̬��������
		ID3D12DescriptorHeap* curDescriptorHeaps[] = { mDynamicDescriptorHeaps[mCurrentFrame].Get() };
		drawCommandList->SetDescriptorHeaps(_countof(curDescriptorHeaps), curDescriptorHeaps);

		// ��ȡ��ǰ֡�Ķ�̬��������Handle
		CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicDescriptorHandle(mDynamicDescriptorHeaps[mCurrentFrame]->GetCPUDescriptorHandleForHeapStart());
		// ƫ�Ƶ���ǰλ��
		dynamicDescriptorHandle.Offset(mDynamicDescriptorOffsets[mCurrentFrame], mCbvSrvUavDescriptorSize);

		for (auto& iter : mDrawRecords)
		{
			auto VAO = GetVAOByIndex(iter.VAO);
			auto pipeline = GetPipelineByIndex(iter.pipelineID);
			auto materialData = GetMaterialDataByIndex(iter.materialDataID);

			drawCommandList->SetGraphicsRootSignature(pipeline->rootSignature.Get());
			drawCommandList->SetPipelineState(pipeline->pipelineState.Get());

			if (!materialData->constantBuffers.empty())
				drawCommandList->SetGraphicsRootConstantBufferView(0, materialData->constantBuffers[mCurrentFrame].gpuAddress);

			// ���Shader��������������Դ
			if (pipeline->textureNum > 0)
			{
				// ��ǰ���ƶ����ڶ�̬���������е�ƫ�����
				UINT curDynamicDescriptorOffset = mDynamicDescriptorOffsets[mCurrentFrame];
				// ����������������̬��������
				for (auto& iter : materialData->textureSets[mCurrentFrame].textureHandles)
				{
					// ��ȡ�����CPU Handle
					auto cpuHandle = ZXD3D12DescriptorManager::GetInstance()->GetCPUDescriptorHandle(iter);
					// ��������̬��������
					mD3D12Device->CopyDescriptorsSimple(1, dynamicDescriptorHandle, cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					// ��̬��������Handle����һλ
					dynamicDescriptorHandle.Offset(1, mCbvSrvUavDescriptorSize);
					mDynamicDescriptorOffsets[mCurrentFrame]++;
				}
				// ��ȡ��̬�������ѵ�GPU Handle
				CD3DX12_GPU_DESCRIPTOR_HANDLE dynamicGPUHandle(mDynamicDescriptorHeaps[mCurrentFrame]->GetGPUDescriptorHandleForHeapStart());
				// ƫ�Ƶ���ǰ���ƶ������ʼλ��
				dynamicGPUHandle.Offset(curDynamicDescriptorOffset, mCbvSrvUavDescriptorSize);
				// ���õ�ǰ���ƶ���Ķ�̬��������
				drawCommandList->SetGraphicsRootDescriptorTable(1, dynamicGPUHandle);
			}

			vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews = { VAO->vertexBufferView };
			if (iter.instanceBuffer != UINT32_MAX)
			{
				vertexBufferViews.push_back(GetInstanceBufferByIndex(iter.instanceBuffer)->view);
			}

			drawCommandList->IASetIndexBuffer(&VAO->indexBufferView);
			drawCommandList->IASetVertexBuffers(0, static_cast<UINT>(vertexBufferViews.size()), vertexBufferViews.data());

			if (iter.instanceBuffer == UINT32_MAX)
				drawCommandList->DrawIndexedInstanced(VAO->indexCount, 1, 0, 0, 0);
			else
				drawCommandList->DrawIndexedInstanced(VAO->indexCount, iter.instanceNum, 0, 0, 0);
		}

		// ��״̬�л�ȥ
		if (curFBO->bufferType == FrameBufferType::Normal || curFBO->bufferType == FrameBufferType::Deferred)
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
		else if (curFBO->bufferType == FrameBufferType::GBuffer)
		{
			if (positionBuffer != nullptr)
			{
				auto positionBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(positionBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
				drawCommandList->ResourceBarrier(1, &positionBufferTransition);
			}
			if (normalBuffer != nullptr)
			{
				auto normalBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(normalBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
				drawCommandList->ResourceBarrier(1, &normalBufferTransition);
			}
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
		else if (curFBO->bufferType == FrameBufferType::Present)
		{
			if (colorBuffer != nullptr)
			{
				auto colorBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(colorBuffer->texture.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
				drawCommandList->ResourceBarrier(1, &colorBufferTransition);
			}
		}

		// �������ύCommand List
		ThrowIfFailed(drawCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { drawCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		mDrawRecords.clear();

#ifndef ZX_EDITOR
		if (drawCommand->commandType == CommandType::UIRendering)
			SignalFence(mFrameFences[mCurrentFrame]);
#endif
	}

	void RenderAPID3D12::SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->indexCount = static_cast<UINT>(indices.size());
		meshBuffer->vertexCount = static_cast<UINT>(vertices.size());

		// ����Vertex Buffer
		UINT vertexBufferSize = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		meshBuffer->vertexBuffer = CreateBuffer(vertexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_DEFAULT, false, true, vertices.data());
		meshBuffer->vertexBufferView.SizeInBytes = vertexBufferSize;
		meshBuffer->vertexBufferView.StrideInBytes = sizeof(Vertex);
		meshBuffer->vertexBufferView.BufferLocation = meshBuffer->vertexBuffer.gpuAddress;

		// ����Index Buffer
		UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indices.size());
		meshBuffer->indexBuffer = CreateBuffer(indexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_DEFAULT, false, true, indices.data());
		meshBuffer->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		meshBuffer->indexBufferView.SizeInBytes = indexBufferSize;
		meshBuffer->indexBufferView.BufferLocation = meshBuffer->indexBuffer.gpuAddress;

		// ����ǹ�׷���ߣ���Ҫ����һ��BLAS( Bottom Level Acceleration Structure )
		if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
		{
			BuildBottomLevelAccelerationStructure(VAO, true);
		}

		meshBuffer->inUse = true;
	}

	void RenderAPID3D12::SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->indexCount = static_cast<UINT>(indexSize);
		meshBuffer->vertexCount = static_cast<UINT>(vertexSize);

		// ������̬Vertex Buffer
		UINT vertexBufferSize = static_cast<UINT>(sizeof(Vertex) * vertexSize);
		meshBuffer->vertexBuffer = CreateBuffer(vertexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, true, true, nullptr);
		meshBuffer->vertexBufferView.SizeInBytes = vertexBufferSize;
		meshBuffer->vertexBufferView.StrideInBytes = sizeof(Vertex);
		meshBuffer->vertexBufferView.BufferLocation = meshBuffer->vertexBuffer.gpuAddress;

		// ������̬Index Buffer
		UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indexSize);
		meshBuffer->indexBuffer = CreateBuffer(indexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, true, true, nullptr);
		meshBuffer->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		meshBuffer->indexBufferView.SizeInBytes = indexBufferSize;
		meshBuffer->indexBufferView.BufferLocation = meshBuffer->indexBuffer.gpuAddress;

		meshBuffer->inUse = true;
	}

	void RenderAPID3D12::UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		memcpy(meshBuffer->vertexBuffer.cpuAddress, vertices.data(), vertices.size() * sizeof(Vertex));
		memcpy(meshBuffer->indexBuffer.cpuAddress, indices.data(), indices.size() * sizeof(uint32_t));
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
		mMeshsToDelete.insert(pair(VAO, DX_MAX_FRAMES_IN_FLIGHT));
	}

	void RenderAPID3D12::UseShader(unsigned int ID)
	{
		mCurPipeLineIdx = ID;
	}

	// Boolean
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Float
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, float value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Integer
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, int32_t value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Unsigned Integer
	void RenderAPID3D12::SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
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
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 2);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
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
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 3);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
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
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 4);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, array, sizeof(float) * 4);
		}
		delete[] array;
	}
	void RenderAPID3D12::SetShaderVector(Material* material, const string& name, const Vector4* value, uint32_t count, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, 0);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, 0);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value, sizeof(Vector4) * count);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, 0);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, 0);

			if (valueAddress != nullptr)
				memcpy(valueAddress, value, sizeof(Vector4) * count);
		}
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
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 9);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
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
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 16);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, array, sizeof(float) * 16);
		}
		delete[] array;
	}
	void RenderAPID3D12::SetShaderMatrix(Material* material, const string& name, const Matrix4* value, uint32_t count, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			if (material->type == MaterialType::RayTracing)
				valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, 0);
			else
				valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, 0);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value, sizeof(Matrix4) * count);
		}
		else
		{
			void* valueAddress = nullptr;

			if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, 0);
			else
				valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, 0);

			if (valueAddress != nullptr)
			memcpy(valueAddress, value, sizeof(Matrix4) * count);
		}
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


	uint32_t RenderAPID3D12::CreateRayTracingPipeline(const RayTracingShaderPathGroup& rtShaderPathGroup)
	{
		ZXD3D12RTPipeline* rtPipeline = new ZXD3D12RTPipeline();
		mRTPipelines.push_back(rtPipeline);
		uint32_t rtPipelineID = static_cast<uint32_t>(mRTPipelines.size() - 1);

		// ׼����ǩ������
		vector<D3D12_DESCRIPTOR_RANGE> dTableParams(ZX_D3D12_RT_ROOT_PARAMETER_NUM);
		// register(t0, space0) TLAS
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_TLAS]				= { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, mRTRootParamOffsetInDescriptorHeapTLAS };
		// register(u0, space0) ���ͼ��
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_OUTPUT_IMAGE]		= { D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, mRTRootParamOffsetInDescriptorHeapOutputImage };
		// register(t0, space1) ��������Buffer
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_INDEX_BUFFER]		= { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mRTSceneRenderObjectNum, 0, 1, mRTRootParamOffsetInDescriptorHeapIndexBuffer };
		// register(t0, space2) ����Buffer
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_VERTEX_BUFFER]		= { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mRTSceneRenderObjectNum, 0, 2, mRTRootParamOffsetInDescriptorHeapVertexBuffer };
		// register(t0, space3) ��������Buffer
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_MATERIAL_DATA]		= { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mRTSceneRenderObjectNum, 0, 3, mRTRootParamOffsetInDescriptorHeapMaterialData };
		// register(t0, space4) 2D��������
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_TEXTURE_2D]			= { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mRTSceneTextureNum, 0, 4, mRTRootParamOffsetInDescriptorHeapTexture2DArray };
		// register(t0, space5) CubeMap��������
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_TEXTURE_CUBE]		= { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mRTSceneCubeMapNum, 0, 5, mRTRootParamOffsetInDescriptorHeapTextureCubeArray };
		// register(b0, space0) ����Buffer (Vulkan PushConstants)
		dTableParams[ZX_D3D12_RT_ROOT_PARAMETER_CONSTANT_BUFFER]	= { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, mRTRootParamOffsetInDescriptorHeapConstantBuffer };
		
		vector<CD3DX12_ROOT_PARAMETER> rootParameters(1);
		rootParameters[0].InitAsDescriptorTable(static_cast<UINT>(dTableParams.size()), dTableParams.data(), D3D12_SHADER_VISIBILITY_ALL);

		auto samplers = GetStaticSamplersDesc();
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(
			static_cast<UINT>(rootParameters.size()), rootParameters.data(),
			static_cast<UINT>(samplers.size()), samplers.data(),
			// ��׷Shader�ĸ�ǩ��һ��Ĭ�϶�Ҫ����LOCAL��־������Ҫ�͹�դ���߼�����߻��ʹ��
			D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE
		);

		// ���л���ǩ��
		ComPtr<ID3DBlob> error;
		ComPtr<ID3DBlob> serializedRootSignature;
		HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &error);
		if (error != nullptr)
			Debug::LogError((char*)error->GetBufferPointer());
		ThrowIfFailed(hr);

		// ������ǩ��(����ֻ������һ����ǩ��������Shader��������Ҳ���Ը�ÿ��Shader���������ĸ�ǩ������Shader����������ܴ������µ����������ܿ��ܸ���)
		ThrowIfFailed(mD3D12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rtPipeline->rootSignature)));

		// ��HLSL�����ﵼ����Shader
		vector<ZXD3D12ShaderBlob> shaderBlobs;
		// Hit Groups
		vector<ZXD3D12HitGroupDesc> hitGroups;
		// ������Ray Gen��Miss��Hit Group������
		vector<wstring> allExportNames;
		// ����DXIL Library
		// һ��HLSL�����ļ�������һ��DXIL Library��һ��DXIL Library���԰������Shader
		// ����һ��RayGen Shader��һ��Miss Shader�����߶��Hit Shader�ȵ�
		// �������ߵ�ʱ����Ҫ��Ҫ�õ���Shader��DXIL Library���浼�����ŵ�D3D12_DXIL_LIBRARY_DESC��D3D12_EXPORT_DESC* pExports������
		// ����������������׷���ߵ����д������д��һ��HLSL�ļ����棬Ȼ������һ��DXIL Library����������Shader
		vector<ZXD3D12DXILLibraryDesc> dxilLibraries;
		for (size_t i = 0; i < rtShaderPathGroup.rGenPaths.size(); i++)
		{
			ZXD3D12ShaderBlob shaderBlob;

			wstring exportName = L"RayGen" + to_wstring(i);
			allExportNames.push_back(exportName);
			shaderBlob.exportNames.push_back(exportName);
			shaderBlob.blob = CompileRTShader(rtShaderPathGroup.rGenPaths[i]);
			shaderBlobs.push_back(shaderBlob);

			dxilLibraries.push_back(CreateDXILLibrary(shaderBlobs.back().blob, shaderBlobs.back().exportNames));
		}
		for (size_t i = 0; i < rtShaderPathGroup.rMissPaths.size(); i++)
		{
			ZXD3D12ShaderBlob shaderBlob;

			wstring exportName = L"Miss" + to_wstring(i);
			allExportNames.push_back(exportName);
			shaderBlob.exportNames.push_back(exportName);
			shaderBlob.blob = CompileRTShader(rtShaderPathGroup.rMissPaths[i]);
			shaderBlobs.push_back(shaderBlob);

			dxilLibraries.push_back(CreateDXILLibrary(shaderBlobs.back().blob, shaderBlobs.back().exportNames));
		}
		for (size_t i = 0; i < rtShaderPathGroup.rHitGroupPaths.size(); i++)
		{
			ZXD3D12HitGroupDesc hitGroup;
			hitGroup.desc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;

			hitGroup.hitGroupName = L"HitGroup" + to_wstring(i);
			allExportNames.push_back(hitGroup.hitGroupName);

			ZXD3D12ShaderBlob closestHitShaderBlob;
			wstring closestHitExportName = L"ClosestHit" + to_wstring(i);
			closestHitShaderBlob.exportNames.push_back(closestHitExportName);
			closestHitShaderBlob.blob = CompileRTShader(rtShaderPathGroup.rHitGroupPaths[i].rClosestHitPath);
			shaderBlobs.push_back(closestHitShaderBlob);
			dxilLibraries.push_back(CreateDXILLibrary(shaderBlobs.back().blob, shaderBlobs.back().exportNames));

			hitGroup.closestHitShaderName = closestHitExportName;

			if (!rtShaderPathGroup.rHitGroupPaths[i].rAnyHitPath.empty())
			{
				ZXD3D12ShaderBlob anyHitShaderBlob;
				wstring anyHitExportName = L"AnyHit" + to_wstring(i);
				anyHitShaderBlob.exportNames.push_back(anyHitExportName);
				anyHitShaderBlob.blob = CompileRTShader(rtShaderPathGroup.rHitGroupPaths[i].rAnyHitPath);
				shaderBlobs.push_back(anyHitShaderBlob);
				dxilLibraries.push_back(CreateDXILLibrary(shaderBlobs.back().blob, shaderBlobs.back().exportNames));

				hitGroup.anyHitShaderName = anyHitExportName;
			}

			if (!rtShaderPathGroup.rHitGroupPaths[i].rIntersectionPath.empty())
			{
				ZXD3D12ShaderBlob intersectionShaderBlob;
				wstring intersectionExportName = L"Intersection" + to_wstring(i);
				intersectionShaderBlob.exportNames.push_back(intersectionExportName);
				intersectionShaderBlob.blob = CompileRTShader(rtShaderPathGroup.rHitGroupPaths[i].rIntersectionPath);
				shaderBlobs.push_back(intersectionShaderBlob);
				dxilLibraries.push_back(CreateDXILLibrary(shaderBlobs.back().blob, shaderBlobs.back().exportNames));

				hitGroup.intersectionShaderName = intersectionExportName;
			}

			hitGroups.push_back(hitGroup);
			hitGroups.back().desc.HitGroupExport = hitGroups.back().hitGroupName.c_str();
			hitGroups.back().desc.ClosestHitShaderImport = hitGroups.back().closestHitShaderName.c_str();
			if (!hitGroups.back().anyHitShaderName.empty())
				hitGroups.back().desc.AnyHitShaderImport = hitGroups.back().anyHitShaderName.c_str();
			if (!hitGroups.back().intersectionShaderName.empty())
				hitGroups.back().desc.IntersectionShaderImport = hitGroups.back().intersectionShaderName.c_str();
		}

		// D3D12��RayTracing��������State SubObject���ɵģ�����DXIL Library��Hit Group��Shader Config��Root Signature�ȵ�
		UINT64 subObjectCount = 
			dxilLibraries.size() +	// DXIL Library
			hitGroups.size() +		// Hit Group����
			1 +						// Shader Config
			1 +						// Associate the set of shaders with config
			2 +						// ��ǩ�������Ӧ��Exports Association(����ж����ǩ��������Ϊ2*����)
			2 +						// Empty Global and Local Root Signature
			1;						// Pipeline Config

		// ����State SubObject
		vector<D3D12_STATE_SUBOBJECT> subObjects(subObjectCount);
		UINT64 index = 0;

		// ------------------------------------- DXIL Library -------------------------------------
		for (auto& dxilLibrary : dxilLibraries)
		{
			subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
			subObjects[index].pDesc = &dxilLibrary.desc;
			index++;
		}

		// -------------------------------------- Hit Groups --------------------------------------
		for (auto& hitGroup : hitGroups)
		{
			subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
			subObjects[index].pDesc = &hitGroup.desc;
			index++;
		}

		// ------------------------------------ Shader Config -------------------------------------
		D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
		shaderConfig.MaxPayloadSizeInBytes = 32 * sizeof(float);
		shaderConfig.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;

		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		subObjects[index].pDesc = &shaderConfig;
		index++;

		// ---------------------------- Associate the set of shaders with config ------------------
		vector<LPCWSTR> shaderExports;
		for (auto& exportName : allExportNames)
			shaderExports.push_back(exportName.c_str());

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION shaderPayloadAssociation = {};
		shaderPayloadAssociation.NumExports = static_cast<UINT>(shaderExports.size());
		shaderPayloadAssociation.pExports = shaderExports.data();
		shaderPayloadAssociation.pSubobjectToAssociate = &subObjects[index - 1];

		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		subObjects[index].pDesc = &shaderPayloadAssociation;
		index++;

		// ------------------------------------ Root Signature ------------------------------------
		ID3D12RootSignature* rSig = rtPipeline->rootSignature.Get();

		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		subObjects[index].pDesc = &rSig;
		index++;

		// ------------------------------ Root Signature Association ------------------------------
		// ǰ��Shader Config����������������������е�Shader������������ǵ�����ǩ����
		// ������ʱֻ��һ��ȫ�ָ�ǰ�棬���������������һ����
		vector<wstring> allExportNamesCopy = allExportNames;
		vector<LPCWSTR> rootSignatureExports;
		for (auto& exportName : allExportNamesCopy)
			rootSignatureExports.push_back(exportName.c_str());

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION rootSignatureAssociation = {};
		rootSignatureAssociation.NumExports = static_cast<UINT>(rootSignatureExports.size());
		rootSignatureAssociation.pExports = rootSignatureExports.data();
		rootSignatureAssociation.pSubobjectToAssociate = &subObjects[index - 1];

		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		subObjects[index].pDesc = &rootSignatureAssociation;
		index++;

		// ------------------------------ Empty Local Root Signature ------------------------------
		ID3D12RootSignature* lSig = mEmptyLocalRootSignature.Get();
		
		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		subObjects[index].pDesc = &lSig;
		index++;

		// ----------------------------- Empty Global Root Signature ------------------------------
		ID3D12RootSignature* gSig = mEmptyGlobalRootSignature.Get();
		
		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		subObjects[index].pDesc = &gSig;
		index++;

		// ----------------------------------- Pipeline Config ------------------------------------
		D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
		pipelineConfig.MaxTraceRecursionDepth = 2;

		subObjects[index].Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		subObjects[index].pDesc = &pipelineConfig;

		// ��������
		D3D12_STATE_OBJECT_DESC stateObjectDesc = {};
		stateObjectDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		stateObjectDesc.NumSubobjects = static_cast<UINT>(subObjects.size());
		stateObjectDesc.pSubobjects = subObjects.data();
		ThrowIfFailed(mD3D12Device->CreateStateObject(&stateObjectDesc, IID_PPV_ARGS(&rtPipeline->pipeline)));

		// ��ȡ���ߵ�����
		ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		ThrowIfFailed(rtPipeline->pipeline->QueryInterface(IID_PPV_ARGS(&stateObjectProperties)));

		// ������׷�������ݵ���������
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.NumDescriptors = mRTRootParamOffsetInDescriptorHeapSize;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		rtPipeline->descriptorHeaps.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			ThrowIfFailed(mD3D12Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&rtPipeline->descriptorHeaps[i])));

		// ����SBT��С
		// SBT��Entry�����˹̶�һ��Shader Identifier��Root Arguments
		// ��ǰ��������Shader���ǹ̶�rootArgumentsNum��������SBT��Ҫ�������64λ��ַ������һ��������С��8�ֽڣ��������rootArgumentsNum * 8
		static const UINT64 rootArgumentsNum = 1;
		UINT64 rootArgumentsSize = static_cast<UINT64>(rootArgumentsNum * 8);
		// ����ж��Ray Generation������Ĳ�����С��������Ray Generation������������
		rtPipeline->SBT.rayGenEntrySize = Math::AlignUpPOT(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + rootArgumentsSize, (UINT64)D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
		rtPipeline->SBT.rayGenSectionSize = Math::AlignUpPOT(rtPipeline->SBT.rayGenEntrySize * rtShaderPathGroup.rGenPaths.size(), (UINT64)D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
		// Miss��HitGroup���㷽ʽͬ�����������ﶼ��һ�����
		rtPipeline->SBT.missEntrySize = Math::AlignUpPOT(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + rootArgumentsSize, (UINT64)D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
		rtPipeline->SBT.missSectionSize = Math::AlignUpPOT(rtPipeline->SBT.missEntrySize * rtShaderPathGroup.rMissPaths.size(), (UINT64)D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
		rtPipeline->SBT.hitGroupEntrySize = Math::AlignUpPOT(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + rootArgumentsSize, (UINT64)D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
		rtPipeline->SBT.hitGroupSectionSize = Math::AlignUpPOT(rtPipeline->SBT.hitGroupEntrySize * rtShaderPathGroup.rHitGroupPaths.size(), (UINT64)D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
		UINT64 sbtSize = Math::AlignUpPOT(rtPipeline->SBT.rayGenSectionSize + rtPipeline->SBT.missSectionSize + rtPipeline->SBT.hitGroupSectionSize, (UINT64)256);

		// ����SBT Buffer
		rtPipeline->SBT.buffers.resize(DX_MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			rtPipeline->SBT.buffers[i] = CreateBuffer(sbtSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, true, true);
		}

		// ���SBT Buffer����
		// ��Щ���ݲ���Ҫÿ֡���£�����Root Arguments�����Ҫ���¸�ǩ��������ֻ��Ҫ�������������϶�Ӧ����������
		// SBT�ϵ���������Щ�������ĵ�ַ������SBT�ϵ���������ַ���ݱ�����Ҫ���£��൱��ֻ��Ҫ����SBT�ϴ�ĵ�ַָ�������
		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			// Root Arguments GPU Handle
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = rtPipeline->descriptorHeaps[i]->GetGPUDescriptorHandleForHeapStart();

			// Root Arguments
			vector<void*> rootArguments(rootArgumentsNum);
			// һ��Descriptor Table��һ������
			rootArguments[0] = reinterpret_cast<void*>(gpuHandle.ptr);

			// ����д��ָ��
			char* pSBT = static_cast<char*>(rtPipeline->SBT.buffers[i].cpuAddress);

			char* pRayGen = pSBT;
			for (size_t j = 0; j < rtShaderPathGroup.rGenPaths.size(); j++)
			{
				wstring exportName = L"RayGen" + to_wstring(j);
				void* id = stateObjectProperties->GetShaderIdentifier(exportName.c_str());

				// RayGen Shader Identifier
				memcpy(pRayGen, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
				// RayGen Root Arguments
				memcpy(pRayGen + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, rootArguments.data(), rootArgumentsSize);

				pRayGen += rtPipeline->SBT.rayGenEntrySize;
			}

			char* pMiss = pSBT + rtPipeline->SBT.rayGenSectionSize;
			for (size_t j = 0; j < rtShaderPathGroup.rMissPaths.size(); j++)
			{
				wstring exportName = L"Miss" + to_wstring(j);
				void* id = stateObjectProperties->GetShaderIdentifier(exportName.c_str());

				// Miss Shader Identifier
				memcpy(pMiss, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
				// Miss Root Arguments
				memcpy(pMiss + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, rootArguments.data(), rootArgumentsSize);

				pMiss += rtPipeline->SBT.missEntrySize;
			}

			char* pHitGroup = pSBT + rtPipeline->SBT.rayGenSectionSize + rtPipeline->SBT.missSectionSize;
			for (size_t j = 0; j < rtShaderPathGroup.rHitGroupPaths.size(); j++)
			{
				wstring exportName = L"HitGroup" + to_wstring(j);
				void* id = stateObjectProperties->GetShaderIdentifier(exportName.c_str());

				// HitGroup Shader Identifier
				memcpy(pHitGroup, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
				// HitGroup Root Arguments
				memcpy(pHitGroup + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, rootArguments.data(), rootArgumentsSize);

				pHitGroup += rtPipeline->SBT.hitGroupEntrySize;
			}
		}

		// ��ʼ��TLAS����
		rtPipeline->tlasIdx = GetNextTLASGroupIndex();
		auto tlasGroup = GetTLASGroupByIndex(rtPipeline->tlasIdx);
		tlasGroup->asGroup.resize(DX_MAX_FRAMES_IN_FLIGHT);
		tlasGroup->inUse = true;
		// ��ʼ������TLAS���м�Buffer
		mTLASScratchBuffers.resize(DX_MAX_FRAMES_IN_FLIGHT);
		mTLASInstanceBuffers.resize(DX_MAX_FRAMES_IN_FLIGHT);

		// ������׷���ߺͳ������������Դ������
		CreateRTPipelineData(rtPipelineID);
		CreateRTSceneData(rtPipelineID);

		return rtPipelineID;
	}

	void RenderAPID3D12::SwitchRayTracingPipeline(uint32_t rtPipelineID)
	{
		mCurRTPipelineID= rtPipelineID;

		mRTVPMatrix.clear();
		mRTVPMatrix.resize(DX_MAX_FRAMES_IN_FLIGHT);
		mRTFrameCount.clear();
		mRTFrameCount.resize(DX_MAX_FRAMES_IN_FLIGHT, 0);
	};

	uint32_t RenderAPID3D12::CreateRayTracingMaterialData()
	{ 
		uint32_t rtMaterialDataID = GetNextRTMaterialDataIndex();
		auto rtMaterialData = GetRTMaterialDataByIndex(rtMaterialDataID);

		rtMaterialData->inUse = true;

		return rtMaterialDataID;
	};

	void RenderAPID3D12::SetUpRayTracingMaterialData(Material* material)
	{
		auto rtMaterialData = GetRTMaterialDataByIndex(material->data->GetRTID());

		ShaderParser::SetUpRTMaterialData(material->data, GraphicsAPI::D3D12);

		rtMaterialData->buffers.resize(DX_MAX_FRAMES_IN_FLIGHT);

		UINT64 bufferSize = static_cast<UINT64>(material->data->rtMaterialDataSize);
		if (bufferSize > 0)
		{
			for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
			{
				// ���Buffer������һ���Դ��������޸ĵģ����Կ����Ż���D3D12_HEAP_TYPE_DEFAULT
				rtMaterialData->buffers[i] = CreateBuffer(bufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, true, false);
			}
		}

		for (auto& property : material->data->vec2Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->vec3Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->vec4Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->floatDatas)
			SetShaderScalar(material, property.first, property.second, true);
		for (auto& property : material->data->uintDatas)
			SetShaderScalar(material, property.first, property.second, true);
	};

	void RenderAPID3D12::DeleteRayTracingMaterialData(uint32_t id)
	{
		rtMaterialDatasToDelete.insert(pair(id, DX_MAX_FRAMES_IN_FLIGHT));
	};

	void RenderAPID3D12::SetRayTracingSkyBox(uint32_t textureID)
	{
		// ��պ�����Ĭ���ǵ�ǰ��һ֡�ĵ�һ��CubeMap
		mCurRTSceneCubeMapIndexMap[textureID] = 0;
		mCurRTSceneCubeMapIndexes.push_back(textureID);
	}

	void RenderAPID3D12::PushRayTracingMaterialData(Material* material)
	{
		// ���������ʹ�õ�������ӵ���ǰ��׷�����е��������б���
		for (auto& iter : material->data->textures)
		{
			auto textureID = iter.second->GetID();
			if (iter.second->type == TextureType::ZX_2D)
			{
				if (mCurRTSceneTextureIndexMap.find(textureID) == mCurRTSceneTextureIndexMap.end())
				{
					mCurRTSceneTextureIndexMap[textureID] = static_cast<uint32_t>(mCurRTSceneTextureIndexes.size());
					mCurRTSceneTextureIndexes.emplace_back(textureID);
				}
			}
			else if (iter.second->type == TextureType::ZX_Cube)
			{
				if (mCurRTSceneCubeMapIndexMap.find(textureID) == mCurRTSceneCubeMapIndexMap.end())
				{
					mCurRTSceneCubeMapIndexMap[textureID] = static_cast<uint32_t>(mCurRTSceneCubeMapIndexes.size());
					mCurRTSceneCubeMapIndexes.emplace_back(textureID);
				}
			}
		}

		// �������׷������ӵ���ǰ��׷�����е��ܹ�׷�����б���
		auto rtMaterialDataID = material->data->GetRTID();
		if (mCurRTSceneRTMaterialDataMap.find(rtMaterialDataID) == mCurRTSceneRTMaterialDataMap.end())
		{
			mCurRTSceneRTMaterialDataMap[rtMaterialDataID] = static_cast<uint32_t>(mCurRTSceneRTMaterialDatas.size());
			mCurRTSceneRTMaterialDatas.emplace_back(rtMaterialDataID);
		}

		// ��������������������д��Buffer
		for (auto& iter : material->data->textures)
		{
			auto textureID = iter.second->GetID();
			auto textureIdx = 0;

			if (iter.second->type == TextureType::ZX_2D)
				textureIdx = mCurRTSceneTextureIndexMap[textureID];
			else if (iter.second->type == TextureType::ZX_Cube)
				textureIdx = mCurRTSceneCubeMapIndexMap[textureID];

			SetShaderScalar(material, iter.first, textureIdx);
		}
	}

	void RenderAPID3D12::PushAccelerationStructure(uint32_t VAO, uint32_t hitGroupIdx, uint32_t rtMaterialDataID, const Matrix4& transform)
	{
		ZXD3D12ASInstanceData asIns = {};
		asIns.VAO = VAO;
		asIns.hitGroupIdx = hitGroupIdx;
		asIns.rtMaterialDataID = rtMaterialDataID;
		asIns.transform = transform;
		mASInstanceData.push_back(std::move(asIns));
	}

	void RenderAPID3D12::RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants)
	{
		auto rtPipeline = mRTPipelines[mCurRTPipelineID];

		// ���㻭�澲ֹ��֡�����ۻ�ʽ��׷��Ⱦ��Ҫ�������
		if (rtConstants.VP != mRTVPMatrix[mCurrentFrame])
		{
			mRTFrameCount[mCurrentFrame] = 0;
			mRTVPMatrix[mCurrentFrame] = rtConstants.VP;
		}
		uint32_t frameCount = mRTFrameCount[mCurrentFrame]++;

		// �ȸ��µ�ǰ֡�͹�׷���߰󶨵ĳ�������
		UpdateRTSceneData(mCurRTPipelineID);
		// ���µ�ǰ֡�͹�׷���߰󶨵Ĺ�������
		UpdateRTPipelineData(mCurRTPipelineID);

		// ��ȡ��ǰ֡��Command
		auto drawCommand = GetDrawCommandByIndex(commandID);
		auto& allocator = drawCommand->allocators[mCurrentFrame];
		auto& drawCommandList = drawCommand->commandLists[mCurrentFrame];

		// ����Command List
		ThrowIfFailed(allocator->Reset());
		ThrowIfFailed(drawCommandList->Reset(allocator.Get(), nullptr));

		// ��ȡ��׷���������Ŀ��ͼ��
		auto curFBO = GetFBOByIndex(mCurFBOIdx);
		uint32_t textureID = GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()];
		auto texture = GetTextureByIndex(textureID);

		// תΪ��׷�����ʽ
		CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
			texture->texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		drawCommandList->ResourceBarrier(1, &transition);

		// ���ù�׷����
		drawCommandList->SetPipelineState1(rtPipeline->pipeline.Get());
		// ���ù�׷��������
		vector<ID3D12DescriptorHeap*> descriptorHeaps = { rtPipeline->descriptorHeaps[mCurrentFrame].Get() };
		drawCommandList->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()), descriptorHeaps.data());

		// д�볣������
		{
			char* cBufferPtr = static_cast<char*>(rtPipeline->constantBuffers[mCurrentFrame].cpuAddress);

			float matVP[16];
			rtConstants.VP.ToColumnMajorArray(matVP);
			memcpy(cBufferPtr, matVP, 16 * sizeof(float));
			cBufferPtr += 16 * sizeof(float);

			float matIV[16];
			rtConstants.V_Inv.ToColumnMajorArray(matIV);
			memcpy(cBufferPtr, matIV, 16 * sizeof(float));
			cBufferPtr += 16 * sizeof(float);

			float matIP[16];
			rtConstants.P_Inv.ToColumnMajorArray(matIP);
			memcpy(cBufferPtr, matIP, 16 * sizeof(float));
			cBufferPtr += 16 * sizeof(float);

			memcpy(cBufferPtr, &rtConstants.lightPos, 3 * sizeof(float));
			cBufferPtr += 3 * sizeof(float);

			memcpy(cBufferPtr, &frameCount, sizeof(uint32_t));
			cBufferPtr += sizeof(uint32_t);

			memcpy(cBufferPtr, &rtConstants.time, sizeof(float));
		}
		
		D3D12_DISPATCH_RAYS_DESC dispatchRaysDesc = {};
		// Ray Generation Shader
		dispatchRaysDesc.RayGenerationShaderRecord.StartAddress = rtPipeline->SBT.buffers[mCurrentFrame].gpuAddress;
		dispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes = rtPipeline->SBT.rayGenEntrySize;
		// Miss Shader Table
		dispatchRaysDesc.MissShaderTable.StartAddress = rtPipeline->SBT.buffers[mCurrentFrame].gpuAddress + rtPipeline->SBT.rayGenSectionSize;
		dispatchRaysDesc.MissShaderTable.SizeInBytes = rtPipeline->SBT.missSectionSize;
		dispatchRaysDesc.MissShaderTable.StrideInBytes = rtPipeline->SBT.missEntrySize;
		// Hit Group Table
		dispatchRaysDesc.HitGroupTable.StartAddress = rtPipeline->SBT.buffers[mCurrentFrame].gpuAddress + rtPipeline->SBT.rayGenSectionSize + rtPipeline->SBT.missSectionSize;
		dispatchRaysDesc.HitGroupTable.SizeInBytes = rtPipeline->SBT.hitGroupSectionSize;
		dispatchRaysDesc.HitGroupTable.StrideInBytes = rtPipeline->SBT.hitGroupEntrySize;
		// Ray Generation Shader���߳�������(ͼ��ֱ���)
		dispatchRaysDesc.Width = mViewPortInfo.width;
		dispatchRaysDesc.Height = mViewPortInfo.height;
		dispatchRaysDesc.Depth = 1;
		// Ray Trace
		drawCommandList->DispatchRays(&dispatchRaysDesc);

		// תΪShader��ȡ��ʽ
		transition = CD3DX12_RESOURCE_BARRIER::Transition(
			texture->texture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);
		drawCommandList->ResourceBarrier(1, &transition);

		// �������ύCommand List
		ThrowIfFailed(drawCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { drawCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// ��յ�ǰ֡�ĳ�������
		mASInstanceData.clear();
		mCurRTSceneTextureIndexes.clear();
		mCurRTSceneTextureIndexMap.clear();
		mCurRTSceneCubeMapIndexes.clear();
		mCurRTSceneCubeMapIndexMap.clear();
		mCurRTSceneRTMaterialDatas.clear();
		mCurRTSceneRTMaterialDataMap.clear();
	}

	void RenderAPID3D12::BuildTopLevelAccelerationStructure(uint32_t commandID)
	{
		auto command = GetDrawCommandByIndex(commandID);
		auto& allocator = command->allocators[mCurrentFrame];
		auto& commandList = command->commandLists[mCurrentFrame];

		// ����Command List
		ThrowIfFailed(allocator->Reset());
		ThrowIfFailed(commandList->Reset(allocator.Get(), nullptr));

		// ��ȡ��ǰ֡��TLAS
		auto& curTLAS = GetTLASGroupByIndex(mRTPipelines[mCurRTPipelineID]->tlasIdx)->asGroup[mCurrentFrame];
		const bool isUpdate = curTLAS.isBuilt;

		// ����TLAS���������
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = static_cast<UINT>(mASInstanceData.size());
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

		// ����TLAS����Ҫ���ڴ��С
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		mD3D12Device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

		// Scratch Buffer
		UINT64 scratchSizeInBytes = Math::AlignUpPOT(prebuildInfo.ScratchDataSizeInBytes, static_cast<UINT64>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		mTLASScratchBuffers[mCurrentFrame] = CreateBuffer(scratchSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT, false, true);
		// ����TLASҪ�õ�BLAS����Buffer
		UINT64 instanceDescsSizeInBytes = Math::AlignUpPOT(static_cast<UINT64>(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * mASInstanceData.size()), static_cast<UINT64>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		mTLASInstanceBuffers[mCurrentFrame] = CreateBuffer(instanceDescsSizeInBytes, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, false, true);
		// ����TLAS Buffer
		if (!isUpdate)
		{
			UINT64 resultSizeInBytes = Math::AlignUpPOT(prebuildInfo.ResultDataMaxSizeInBytes, static_cast<UINT64>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
			curTLAS.as = CreateBuffer(resultSizeInBytes, 
				D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, 
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
				D3D12_HEAP_TYPE_DEFAULT, false, true);
		}

		// BLAS����Bufferָ��
		D3D12_RAYTRACING_INSTANCE_DESC* instanceDescs = nullptr;
		// ��Bufferӳ�䵽ָ��
		mTLASInstanceBuffers[mCurrentFrame].buffer->Map(0, nullptr, reinterpret_cast<void**>(&instanceDescs));
		// ӳ��ʧ��ֱ���׳��쳣
		if (!instanceDescs)
			throw std::logic_error("Failed to map instanceDescsBuffer.");

		// BLASʵ������
		UINT instanceCount = static_cast<UINT>(mASInstanceData.size());

		// �½�TLAS�Ļ��Ȱ�BLAS����Buffer����
		// ��֪����һ���Ƿ��Ҫ��Nvidia�Ĺ�׷�̳�������д�ģ�Ҳû����Ϊʲô
		if (!isUpdate)
			memset(instanceDescs, 0, instanceDescsSizeInBytes);

		// ��䳡����Ҫ��Ⱦ�Ķ���ʵ������
		for (UINT i = 0; i < instanceCount; i++)
		{
			auto& data = mASInstanceData[i];
			auto meshData = GetVAOByIndex(data.VAO);

			instanceDescs[i].InstanceID = static_cast<UINT>(i);
			instanceDescs[i].InstanceContributionToHitGroupIndex = data.hitGroupIdx;
			instanceDescs[i].InstanceMask = 0xFF;
			instanceDescs[i].AccelerationStructure = meshData->blas.as.gpuAddress;
			instanceDescs[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;

			float* array = new float[16];
			data.transform.ToRowMajorArray(array);
			for (int j = 0; j < 12; j++) instanceDescs[i].Transform[j / 4][j % 4] = array[j];
			delete[] array;
		}
		mTLASInstanceBuffers[mCurrentFrame].buffer->Unmap(0, nullptr);

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
		asDesc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		asDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		asDesc.Inputs.NumDescs = instanceCount;
		asDesc.Inputs.InstanceDescs = mTLASInstanceBuffers[mCurrentFrame].gpuAddress;
		asDesc.Inputs.Flags = isUpdate ? D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE : D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
		asDesc.DestAccelerationStructureData = curTLAS.as.gpuAddress;
		asDesc.SourceAccelerationStructureData = isUpdate ? curTLAS.as.gpuAddress : NULL;
		asDesc.ScratchAccelerationStructureData = mTLASScratchBuffers[mCurrentFrame].gpuAddress;

		// ����TLAS
		commandList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);

		// ȷ��TLAS�ڱ�ʹ��֮ǰ�ѹ������
		auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(curTLAS.as.buffer.Get());
		commandList->ResourceBarrier(1, &uavBarrier);

		// �������ύCommand List
		ThrowIfFailed(commandList->Close());
		ID3D12CommandList* cmdsLists[] = { commandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	};

	void RenderAPID3D12::BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
		geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		geometryDesc.Triangles.VertexBuffer.StartAddress = meshBuffer->vertexBuffer.gpuAddress;
		geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
		// �����ʽ��BLASֻ���Ķ���λ����һ������
		geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		geometryDesc.Triangles.VertexCount = meshBuffer->vertexCount;
		geometryDesc.Triangles.IndexBuffer = meshBuffer->indexBuffer.gpuAddress;
		geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
		geometryDesc.Triangles.IndexCount = meshBuffer->indexCount;
		// ����BLAS��ʱ����Զ�ģ��������һ���任����ʱ����Ҫ
		geometryDesc.Triangles.Transform3x4 = NULL;
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		// ������Դ�����һ���Թ������BLAS����ʱֻ��һ��
		inputs.NumDescs = 1;
		inputs.pGeometryDescs = &geometryDesc;
		// ָ��PREFER_FAST_TRACE������ʱ�����������ʵʱ��׷�ٶȸ���
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		// ����BLAS����Ҫ���ڴ��С
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		mD3D12Device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

		UINT64 scratchSizeInBytes = Math::AlignUpPOT(prebuildInfo.ScratchDataSizeInBytes, static_cast<UINT64>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		UINT64 resultSizeInBytes = Math::AlignUpPOT(prebuildInfo.ResultDataMaxSizeInBytes, static_cast<UINT64>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

		// ����BLAS��scratch buffer
		auto scratchBuffer = CreateBuffer(scratchSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT, false, true);
		// ����BLAS�Ľ��buffer
		meshBuffer->blas.as = CreateBuffer(resultSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_HEAP_TYPE_DEFAULT, false, true);

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
		asDesc.Inputs = inputs;
		asDesc.DestAccelerationStructureData = meshBuffer->blas.as.gpuAddress;
		asDesc.ScratchAccelerationStructureData = scratchBuffer.gpuAddress;
		// ����Ǹ���BLAS��Ҫָ��֮ǰ��BLAS
		asDesc.SourceAccelerationStructureData = NULL;

		ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
		{
			// ����BLAS
			cmdList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);
			// ȷ��BLAS�ڱ�ʹ��֮ǰ�ѹ������
			auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(meshBuffer->blas.as.buffer.Get());
			cmdList->ResourceBarrier(1, &uavBarrier);
		});

		meshBuffer->blas.isBuilt = true;
	};


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

		DestroyBuffer(VAO->indexBuffer);
		DestroyBuffer(VAO->vertexBuffer);

		if (VAO->blas.isBuilt)
		{
			DestroyAccelerationStructure(VAO->blas);
		}

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
		if (FBO->positionBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(FBO->positionBufferIdx);
			FBO->positionBufferIdx = UINT32_MAX;
		}
		if (FBO->normalBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(FBO->normalBufferIdx);
			FBO->normalBufferIdx = UINT32_MAX;
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

		texture->texture.Reset();

		texture->inUse = false;

#ifdef ZX_EDITOR
		ImGuiTextureManager::GetInstance()->DeleteByEngineID(idx);
#endif
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

		pipeline->textureNum = 0;
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

		for (auto& buffer : materialData->constantBuffers)
		{
			DestroyBuffer(buffer);
		}
		materialData->constantBuffers.clear();

		for (auto& iter : materialData->textureSets)
			iter.textureHandles.clear();
		materialData->textureSets.clear();

		materialData->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextInstanceBufferIndex()
	{
		uint32_t length = static_cast<uint32_t>(mInstanceBufferArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mInstanceBufferArray[i]->inUse)
				return i;
		}

		mInstanceBufferArray.push_back(new ZXD3D12InstanceBuffer());

		return length;
	}

	ZXD3D12InstanceBuffer* RenderAPID3D12::GetInstanceBufferByIndex(uint32_t idx)
	{
		return mInstanceBufferArray[idx];
	}

	void RenderAPID3D12::DestroyInstanceBufferByIndex(uint32_t idx)
	{
		auto instanceBuffer = mInstanceBufferArray[idx];

		DestroyBuffer(instanceBuffer->buffer);

		instanceBuffer->inUse = false;
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

	void RenderAPID3D12::CheckDeleteData()
	{
		vector<uint32_t> deleteList = {};

		// Material
		for (auto& iter : mMaterialDatasToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyMaterialDataByIndex(id);
			mMaterialDatasToDelete.erase(id);
		}

		// ��׷����
		deleteList.clear();
		for (auto& iter : rtMaterialDatasToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyRTMaterialDataByIndex(id);
			rtMaterialDatasToDelete.erase(id);
		}

		// Texture
		deleteList.clear();
		for (auto& iter : mTexturesToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyTextureByIndex(id);
			mTexturesToDelete.erase(id);
		}

		// Mesh
		deleteList.clear();
		for (auto& iter : mMeshsToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyVAOByIndex(id);
			mMeshsToDelete.erase(id);
		}

		// Shader
		deleteList.clear();
		for (auto& iter : mShadersToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyPipelineByIndex(id);
			mShadersToDelete.erase(id);
		}

		// Instance Buffer
		deleteList.clear();
		for (auto& iter : mInstanceBuffersToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyInstanceBufferByIndex(id);
			mInstanceBuffersToDelete.erase(id);
		}
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

	ZXD3D12Buffer RenderAPID3D12::CreateBuffer(UINT64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, D3D12_HEAP_TYPE heapType, bool cpuAddress, bool gpuAddress, const void* data)
	{
		ZXD3D12Buffer buffer;
		buffer.size = static_cast<uint32_t>(size);

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = size;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = flags;

		CD3DX12_HEAP_PROPERTIES bufferProps(heapType);
		ThrowIfFailed(mD3D12Device->CreateCommittedResource(
			&bufferProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			data == nullptr ? initState : D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(buffer.buffer.GetAddressOf()))
		);

		if (cpuAddress)
			ThrowIfFailed(buffer.buffer->Map(0, nullptr, reinterpret_cast<void**>(&buffer.cpuAddress)));

		if (gpuAddress)
			buffer.gpuAddress = buffer.buffer->GetGPUVirtualAddress();

		if (data)
		{
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

			ImmediatelyExecute([=](ComPtr<ID3D12GraphicsCommandList4> cmdList)
			{
				// ��ʵ����ֱ���ڴ���defaultBuffer��ʱ��Ͱѳ�ʼ״̬����ΪD3D12_RESOURCE_STATE_COPY_DEST
				// û��Ҫ��һ�����ת�������Ǵ�����ʱ����������� D3D12_RESOURCE_STATE_COMMON ��ʼ����Debug Layer��Ȼ�����Warning
				// ����Ϊ��û��Warning�����ų����⣬���������д��
				CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
					buffer.buffer.Get(),
					D3D12_RESOURCE_STATE_COMMON,
					D3D12_RESOURCE_STATE_COPY_DEST);
				cmdList->ResourceBarrier(1, &barrier1);

				UpdateSubresources<1>(cmdList.Get(), buffer.buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

				CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
					buffer.buffer.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST,
					initState);
				cmdList->ResourceBarrier(1, &barrier2);
			});
		}

		return buffer;
	}

	void RenderAPID3D12::DestroyBuffer(ZXD3D12Buffer& buffer)
	{
		buffer.buffer.Reset();
		buffer.cpuAddress = nullptr;
		buffer.gpuAddress = 0;
	}


	void RenderAPID3D12::InitDXR()
	{
		ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), (void**)&mDxcCompiler));
		ThrowIfFailed(DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary), (void**)&mDxcLibrary));
		ThrowIfFailed(mDxcLibrary->CreateIncludeHandler(&mDxcIncludeHandler));

		InitEmptyRootSignature();
	}

	void RenderAPID3D12::InitEmptyRootSignature()
	{
		ComPtr<ID3DBlob> error;
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
		ComPtr<ID3DBlob> localSignature;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &localSignature, &error));
		ThrowIfFailed(mD3D12Device->CreateRootSignature(0, localSignature->GetBufferPointer(), localSignature->GetBufferSize(), IID_PPV_ARGS(&mEmptyLocalRootSignature)));

		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);
		ComPtr<ID3DBlob> globalSignature;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &globalSignature, &error));
		ThrowIfFailed(mD3D12Device->CreateRootSignature(0, globalSignature->GetBufferPointer(), globalSignature->GetBufferSize(), IID_PPV_ARGS(&mEmptyGlobalRootSignature)));
	}

	uint32_t RenderAPID3D12::GetNextTLASGroupIndex()
	{
		uint32_t length = static_cast<uint32_t>(mTLASGroupArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mTLASGroupArray[i]->inUse)
				return i;
		}

		mTLASGroupArray.push_back(new ZXD3D12ASGroup());

		return length;
	}

	ZXD3D12ASGroup* RenderAPID3D12::GetTLASGroupByIndex(uint32_t idx)
	{
		return mTLASGroupArray[idx];
	}

	void RenderAPID3D12::DestroyTLASGroupByIndex(uint32_t idx)
	{
		auto tlasGroup = GetTLASGroupByIndex(idx);

		for (auto& tlas : tlasGroup->asGroup)
			DestroyAccelerationStructure(tlas);

		tlasGroup->inUse = false;
	}

	uint32_t RenderAPID3D12::GetNextRTMaterialDataIndex()
	{
		uint32_t length = static_cast<uint32_t>(mRTMaterialDataArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mRTMaterialDataArray[i]->inUse)
				return i;
		}

		mRTMaterialDataArray.push_back(new ZXD3D12RTMaterialData());

		return length;
	}

	ZXD3D12RTMaterialData* RenderAPID3D12::GetRTMaterialDataByIndex(uint32_t idx)
	{
		return mRTMaterialDataArray[idx];
	}

	void RenderAPID3D12::DestroyRTMaterialDataByIndex(uint32_t idx)
	{
		auto rtMaterialData = GetRTMaterialDataByIndex(idx);

		for (auto& buffer : rtMaterialData->buffers)
		{
			DestroyBuffer(buffer);
		}

		rtMaterialData->buffers.clear();

		rtMaterialData->inUse = false;
	}

	ComPtr<IDxcBlob> RenderAPID3D12::CompileRTShader(const string& path)
	{
		// ��ȡHLSL����
		auto code = Resources::LoadTextFile(Resources::GetAssetFullPath(path) + ".dxr");

		// ����HLSL�����Blob
		ComPtr<IDxcBlobEncoding> shaderBlobEncoding;
		ThrowIfFailed(mDxcLibrary->CreateBlobWithEncodingFromPinned((LPBYTE)code.c_str(), (UINT32)code.size(), 0, &shaderBlobEncoding));

		// ��ȡһ�´������֣����Ժ�Include��
		string name = Resources::GetAssetName(path);
		std::wstringstream wss;
		wss << std::wstring(name.begin(), name.end());
		std::wstring wName = wss.str();

		// ����HLSL����
		ComPtr<IDxcOperationResult> operationResult;
		ThrowIfFailed(mDxcCompiler->Compile(
			shaderBlobEncoding.Get(),
			wName.c_str(),
			L"",
			L"lib_6_3",
			nullptr,
			0,
			nullptr,
			0,
			mDxcIncludeHandler,
			&operationResult
		));

		// ��������������д�������������Ϣ
		HRESULT resultCode;
		ThrowIfFailed(operationResult->GetStatus(&resultCode));
		if (FAILED(resultCode))
		{
			if (operationResult)
			{
				ComPtr<IDxcBlobEncoding> errorBlob;
				ThrowIfFailed(operationResult->GetErrorBuffer(&errorBlob));
				if (errorBlob)
				{
					string errorString = string(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
					Debug::LogError(errorString);
				}
			}
			ThrowIfFailed(resultCode);
		}

		// ��ȡ�����ر�����
		ComPtr<IDxcBlob> shaderBlob;
		ThrowIfFailed(operationResult->GetResult(&shaderBlob));
		return shaderBlob;
	}

	ZXD3D12DXILLibraryDesc RenderAPID3D12::CreateDXILLibrary(const ComPtr<IDxcBlob>& dxilBlob, const vector<wstring>& exportedSymbols)
	{
		ZXD3D12DXILLibraryDesc dxilLibDesc = {};

		dxilLibDesc.exportDescs.resize(exportedSymbols.size());
		for (size_t i = 0; i < exportedSymbols.size(); i++)
		{
			dxilLibDesc.exportDescs[i].Name = exportedSymbols[i].c_str();
			dxilLibDesc.exportDescs[i].ExportToRename = L"main";
			dxilLibDesc.exportDescs[i].Flags = D3D12_EXPORT_FLAG_NONE;
		}

		dxilLibDesc.desc.DXILLibrary.pShaderBytecode = dxilBlob->GetBufferPointer();
		dxilLibDesc.desc.DXILLibrary.BytecodeLength = dxilBlob->GetBufferSize();
		dxilLibDesc.desc.NumExports = static_cast<UINT>(dxilLibDesc.exportDescs.size());
		dxilLibDesc.desc.pExports = dxilLibDesc.exportDescs.data();

		return dxilLibDesc;
	}

	void RenderAPID3D12::DestroyAccelerationStructure(ZXD3D12AccelerationStructure& accelerationStructure)
	{
		DestroyBuffer(accelerationStructure.as);
		accelerationStructure.isBuilt = false;
	}

	void RenderAPID3D12::CreateRTPipelineData(uint32_t id)
	{
		auto rtPipeline = mRTPipelines[id];
		rtPipeline->constantBuffers.resize(DX_MAX_FRAMES_IN_FLIGHT);

		for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto heapAddress = rtPipeline->descriptorHeaps[i]->GetCPUDescriptorHandleForHeapStart();

			// ����Constant Buffer (���������ͬ��Vulkan�е�PushConstant)
			rtPipeline->constantBuffers[i] = CreateBuffer(mRTPipelineConstantBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, true, true);

			// ��Constant Buffer
			CD3DX12_CPU_DESCRIPTOR_HANDLE cBufferHandle(heapAddress);
			cBufferHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapConstantBuffer), mCbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = rtPipeline->constantBuffers[i].gpuAddress;
			cbvDesc.SizeInBytes = mRTPipelineConstantBufferSize;
			mD3D12Device->CreateConstantBufferView(&cbvDesc, cBufferHandle);
		}
	}

	void RenderAPID3D12::UpdateRTPipelineData(uint32_t id)
	{
		auto rtPipeline = mRTPipelines[id];

		auto heapAddress = rtPipeline->descriptorHeaps[mCurrentFrame]->GetCPUDescriptorHandleForHeapStart();

		// ��TLAS
		CD3DX12_CPU_DESCRIPTOR_HANDLE tlasHandle(heapAddress);
		tlasHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapTLAS), mCbvSrvUavDescriptorSize);

		auto& curTLAS = GetTLASGroupByIndex(rtPipeline->tlasIdx)->asGroup[mCurrentFrame];

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.RaytracingAccelerationStructure.Location = curTLAS.as.gpuAddress;
		mD3D12Device->CreateShaderResourceView(nullptr, &srvDesc, tlasHandle);

		// ��ȡ��׷���������Ŀ��ͼ��
		auto curFBO = GetFBOByIndex(mCurFBOIdx);
		auto colorBuffer = GetTextureByIndex(GetRenderBufferByIndex(curFBO->colorBufferIdx)->renderBuffers[GetCurFrameBufferIndex()]);

		// �������ͼ��
		CD3DX12_CPU_DESCRIPTOR_HANDLE outputHandle(heapAddress);
		outputHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapOutputImage), mCbvSrvUavDescriptorSize);
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = mPresentBufferFormat;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		mD3D12Device->CreateUnorderedAccessView(colorBuffer->texture.Get(), nullptr, &uavDesc, outputHandle);
	}

	void RenderAPID3D12::CreateRTSceneData(uint32_t id)
	{
		
	}

	void RenderAPID3D12::UpdateRTSceneData(uint32_t id)
	{
		auto rtPipeline = mRTPipelines[id];

		auto heapAddress = rtPipeline->descriptorHeaps[mCurrentFrame]->GetCPUDescriptorHandleForHeapStart();

		// 2D��������
		CD3DX12_CPU_DESCRIPTOR_HANDLE textureHandle(heapAddress);
		textureHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapTexture2DArray), mCbvSrvUavDescriptorSize);
		for (size_t i = 0; i < mCurRTSceneTextureIndexes.size(); i++)
		{
			auto texture = GetTextureByIndex(mCurRTSceneTextureIndexes[i]);

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = mDefaultImageFormat;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;
			mD3D12Device->CreateShaderResourceView(texture->texture.Get(), &srvDesc, textureHandle);

			textureHandle.Offset(1, mCbvSrvUavDescriptorSize);
		}

		// CubeMap����
		CD3DX12_CPU_DESCRIPTOR_HANDLE cubeMapHandle(heapAddress);
		cubeMapHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapTextureCubeArray), mCbvSrvUavDescriptorSize);
		for (size_t i = 0; i < mCurRTSceneCubeMapIndexes.size(); i++)
		{
			auto texture = GetTextureByIndex(mCurRTSceneCubeMapIndexes[i]);

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = mDefaultImageFormat;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.TextureCube.MipLevels = 1;
			mD3D12Device->CreateShaderResourceView(texture->texture.Get(), &srvDesc, cubeMapHandle);

			cubeMapHandle.Offset(1, mCbvSrvUavDescriptorSize);
		}

		// ģ�ͺͲ�������
		CD3DX12_CPU_DESCRIPTOR_HANDLE indexHandle(heapAddress);
		indexHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapIndexBuffer), mCbvSrvUavDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE vertexHandle(heapAddress);
		vertexHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapVertexBuffer), mCbvSrvUavDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE materialHandle(heapAddress);
		materialHandle.Offset(static_cast<INT>(mRTRootParamOffsetInDescriptorHeapMaterialData), mCbvSrvUavDescriptorSize);

		UINT instanceCount = static_cast<UINT>(mASInstanceData.size());
		for (UINT i = 0; i < instanceCount; i++)
		{
			auto& data = mASInstanceData[i];
			auto meshData = GetVAOByIndex(data.VAO);
			auto rtMaterialData = GetRTMaterialDataByIndex(data.rtMaterialDataID);

			// ����������Ͳ�������ȫ����ByteAddressBuffer����ʽ���룬���ݸ�ʽ��Shader�Լ�����
			// ByteAddressBuffer��ҪFormat�̶�ΪDXGI_FORMAT_R32_TYPELESS��FlagsΪD3D12_BUFFER_SRV_FLAG_RAW
			D3D12_SHADER_RESOURCE_VIEW_DESC indexDesc = {};
			indexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			indexDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			indexDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			indexDesc.Buffer.FirstElement = 0;
			indexDesc.Buffer.NumElements = meshData->indexCount;
			indexDesc.Buffer.StructureByteStride = 0;
			indexDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
			mD3D12Device->CreateShaderResourceView(meshData->indexBuffer.buffer.Get(), &indexDesc, indexHandle);

			D3D12_SHADER_RESOURCE_VIEW_DESC vertexDesc = {};
			vertexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			vertexDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			vertexDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			vertexDesc.Buffer.FirstElement = 0;
			vertexDesc.Buffer.NumElements = meshData->vertexBuffer.size / 4;
			vertexDesc.Buffer.StructureByteStride = 0;
			vertexDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
			mD3D12Device->CreateShaderResourceView(meshData->vertexBuffer.buffer.Get(), &vertexDesc, vertexHandle);

			if (rtMaterialData->buffers[mCurrentFrame].size > 0)
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC materialDesc = {};
				materialDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				materialDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				materialDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				materialDesc.Buffer.FirstElement = 0;
				materialDesc.Buffer.NumElements = rtMaterialData->buffers[mCurrentFrame].size / 4;
				materialDesc.Buffer.StructureByteStride = 0;
				materialDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
				mD3D12Device->CreateShaderResourceView(rtMaterialData->buffers[mCurrentFrame].buffer.Get(), &materialDesc, materialHandle);
			}

			indexHandle.Offset(1, mCbvSrvUavDescriptorSize);
			vertexHandle.Offset(1, mCbvSrvUavDescriptorSize);
			materialHandle.Offset(1, mCbvSrvUavDescriptorSize);
		}
	}


	uint32_t RenderAPID3D12::GetCurFrameBufferIndex() const
	{
		if (mCurFBOIdx == mPresentFBOIdx)
			return mCurPresentIdx;
		else
			return mCurrentFrame;
	}

	void RenderAPID3D12::DoWindowSizeChange()
	{
		WaitForRenderFinish();

#ifdef ZX_EDITOR
		ProjectSetting::SetWindowSize(mNewWindowWidth, mNewWindowHeight);
#else
		GlobalData::srcWidth = mNewWindowWidth;
		GlobalData::srcHeight = mNewWindowHeight;
#endif
		
		// �ͷ�ԭPresent Buffer
		DestroyFBOByIndex(mPresentFBOIdx);
		// ���ﻹ�и����ã�Ҫ���ͷ�һ�£�ȷ���������ö��ͷ���
		for (UINT i = 0; i < mPresentBufferCount; i++)
			mPresentBuffers[i].Reset();

		// ��������Present Buffer��С
		ThrowIfFailed(mSwapChain->ResizeBuffers(mPresentBufferCount,
#ifdef ZX_EDITOR
			ProjectSetting::srcWidth, ProjectSetting::srcHeight,
#else
			GlobalData::srcWidth, GlobalData::srcHeight,
#endif
			mPresentBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		// ���´���Present Buffer
		CreateSwapChainBuffers();

		// ���´������д�С�ʹ��ڱ���һ�µ�FBO
		FBOManager::GetInstance()->RecreateAllFollowWindowFBO();

#ifdef ZX_EDITOR
		EditorGUIManager::GetInstance()->OnWindowSizeChange();
#endif

		EventManager::GetInstance()->FireEvent(EventType::WINDOW_RESIZE, "");

		mWindowResized = false;
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
		// ���Fence�Ľ��Ȼ�û����һ�����õĽ���ֵ�Ϳ�ʼ�ȴ�
		// �жϴ���0�Ƿ�ֹ�ȴ�û��Signal����Fence
		if (fence->currentFence > 0 && fence->fence->GetCompletedValue() < fence->currentFence)
		{
			// ����һ��"���ȵִ�ո����õ��ź���ֵ"���¼�
			auto event = CreateEventEx(nullptr, NULL, NULL, EVENT_ALL_ACCESS);
			if (event)
			{
				ThrowIfFailed(fence->fence->SetEventOnCompletion(fence->currentFence, event));
				// �ȴ��¼�
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
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame].cpuAddress) + property.offset + property.arrayOffset * idx);

		for (auto& property : reference->shaderInfo.geomProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame].cpuAddress) + property.offset + property.arrayOffset * idx);

		for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame].cpuAddress) + property.offset + property.arrayOffset * idx);

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
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i].cpuAddress) + addressOffset));
				return addresses;
			}
		}

		for (auto& property : reference->shaderInfo.geomProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i].cpuAddress) + addressOffset));
				return addresses;
			}
		}

		for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i].cpuAddress) + addressOffset));
				return addresses;
			}
		}

		Debug::LogError("Could not find shader property named " + name);

		return addresses;
	}

	void* RenderAPID3D12::GetRTMaterialPropertyAddress(MaterialData* materialData, const string& name, uint32_t idx)
	{
		auto rtMaterialData = GetRTMaterialDataByIndex(materialData->GetRTID());

		for (auto& property : materialData->rtMaterialProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(rtMaterialData->buffers[mCurrentFrame].cpuAddress) + property.offset + property.arrayOffset * idx);

		Debug::LogError("Could not find ray tracing material property named " + name);

		return nullptr;
	}

	vector<void*> RenderAPID3D12::GetRTMaterialPropertyAddressAllBuffer(MaterialData* materialData, const string& name, uint32_t idx)
	{
		vector<void*> addresses;
		auto rtMaterialData = GetRTMaterialDataByIndex(materialData->GetRTID());

		for (auto& property : materialData->rtMaterialProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < DX_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(rtMaterialData->buffers[i].cpuAddress) + addressOffset));
				return addresses;
			}
		}

		Debug::LogError("Could not find ray tracing material property named " + name);

		return addresses;
	}

	array<const CD3DX12_STATIC_SAMPLER_DESC, 4> RenderAPID3D12::GetStaticSamplersDesc()
	{
		// ���Բ�ֵ��������Ե�ظ�
		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(0,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // W

		// ���Բ�ֵ��������Ե�ض�
		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(1,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // W

		// �������Բ�������Ե�ظ�
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(2,
			D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // U
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // V
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // W

		// �������Բ�������Ե�ض�
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

	void RenderAPID3D12::ImmediatelyExecute(std::function<void(ComPtr<ID3D12GraphicsCommandList4> cmdList)>&& function)
	{
		// ��������
		ThrowIfFailed(mImmediateExeAllocator->Reset());
		ThrowIfFailed(mImmediateExeCommandList->Reset(mImmediateExeAllocator.Get(), nullptr));

		// ��¼����
		function(mImmediateExeCommandList);

		// ����ִ��
		ThrowIfFailed(mImmediateExeCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { mImmediateExeCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// ����Fence
		SignalFence(mImmediateExeFence);

		// �ȴ�Fence
		WaitForFence(mImmediateExeFence);
	}
}