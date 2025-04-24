#include "RenderAPIMetal.h"
#include "ProjectSetting.h"
#include "Window/WindowManager.h"
#include "Resources.h"
#include "ShaderParser.h"
#include "GlobalData.h"
#include "FBOManager.h"
#include "Material.h"
#include "MaterialData.h"
#include "ZShader.h"
#include "Texture.h"

namespace ZXEngine
{
	unordered_map<BlendFactor, MTL::BlendFactor> mtBlendFactorMap =
	{
		{ BlendFactor::ZERO,      MTL::BlendFactor::BlendFactorZero             }, { BlendFactor::ONE,                 MTL::BlendFactor::BlendFactorOne                      },
		{ BlendFactor::SRC_COLOR, MTL::BlendFactor::BlendFactorSourceColor      }, { BlendFactor::ONE_MINUS_SRC_COLOR, MTL::BlendFactor::BlendFactorOneMinusSourceColor      },
		{ BlendFactor::DST_COLOR, MTL::BlendFactor::BlendFactorDestinationColor }, { BlendFactor::ONE_MINUS_DST_COLOR, MTL::BlendFactor::BlendFactorOneMinusDestinationColor },
		{ BlendFactor::SRC_ALPHA, MTL::BlendFactor::BlendFactorSourceAlpha      }, { BlendFactor::ONE_MINUS_SRC_ALPHA, MTL::BlendFactor::BlendFactorOneMinusSourceAlpha      },
		{ BlendFactor::DST_ALPHA, MTL::BlendFactor::BlendFactorDestinationAlpha }, { BlendFactor::ONE_MINUS_DST_ALPHA, MTL::BlendFactor::BlendFactorOneMinusDestinationAlpha },
	};

	unordered_map<BlendOption, MTL::BlendOperation> mtBlendOptionMap =
	{
		{ BlendOption::ADD, MTL::BlendOperation::BlendOperationAdd }, { BlendOption::SUBTRACT, MTL::BlendOperation::BlendOperationSubtract }, { BlendOption::REVERSE_SUBTRACT, MTL::BlendOperation::BlendOperationReverseSubtract },
		{ BlendOption::MIN, MTL::BlendOperation::BlendOperationMin }, { BlendOption::MAX,      MTL::BlendOperation::BlendOperationMax      },
	};

	unordered_map<CompareOption, MTL::CompareFunction> mtCompareOptionMap =
	{
		{ CompareOption::NEVER,         MTL::CompareFunction::CompareFunctionNever     }, { CompareOption::LESS,             MTL::CompareFunction::CompareFunctionLess         },
		{ CompareOption::ALWAYS,        MTL::CompareFunction::CompareFunctionAlways    }, { CompareOption::GREATER,          MTL::CompareFunction::CompareFunctionGreater      },
		{ CompareOption::EQUAL,         MTL::CompareFunction::CompareFunctionEqual     }, { CompareOption::NOT_EQUAL,        MTL::CompareFunction::CompareFunctionNotEqual     },
		{ CompareOption::LESS_OR_EQUAL, MTL::CompareFunction::CompareFunctionLessEqual }, { CompareOption::GREATER_OR_EQUAL, MTL::CompareFunction::CompareFunctionGreaterEqual },
	};

	unordered_map<FaceCullOption, MTL::CullMode> mtFaceCullOptionMap =
	{
		{ FaceCullOption::Back, MTL::CullMode::CullModeBack }, { FaceCullOption::Front, MTL::CullMode::CullModeFront },
		{ FaceCullOption::None, MTL::CullMode::CullModeNone },
	};

	RenderAPIMetal::RenderAPIMetal()
	{
		mDevice = MTL::CreateSystemDefaultDevice();
		assert(mDevice != nullptr && "Metal device is null");

		mCommandQueue = mDevice->newCommandQueue();
		assert(mCommandQueue != nullptr && "Metal command queue is null");

		CGRect frame;
		frame.origin.x = 0.0;
		frame.origin.y = 0.0;
		frame.size.width = static_cast<CGFloat>(ProjectSetting::srcWidth);
		frame.size.height = static_cast<CGFloat>(ProjectSetting::srcHeight);
		mMetalView = MTK::View::alloc()->init(frame, mDevice);
		mMetalView->setColorPixelFormat(MTL::PixelFormatBGRA8Unorm);

		NS::Window* nsWindow = static_cast<NS::Window*>(WindowManager::GetInstance()->GetWindow());
		nsWindow->setContentView(mMetalView);

		mSemaphore = dispatch_semaphore_create(MT_MAX_FRAMES_IN_FLIGHT);
	}

	void RenderAPIMetal::BeginFrame()
	{
		dispatch_semaphore_wait(mSemaphore, DISPATCH_TIME_FOREVER);

		CheckDeleteData();
	}

	void RenderAPIMetal::EndFrame()
	{
		mCurrentFrame = (mCurrentFrame + 1) % MT_MAX_FRAMES_IN_FLIGHT;
	}

	void RenderAPIMetal::SwitchFrameBuffer(uint32_t id)
	{
		if (id == UINT32_MAX)
			// TODO: 这里需要以某种形式切换到Present Buffer
			mCurFBOIdx = 0;
		else
			mCurFBOIdx = id;
	}

	void RenderAPIMetal::ClearFrameBuffer(FrameBufferClearFlags clearFlags)
	{
		// Metal不需要实现这个接口
	}

	void RenderAPIMetal::BlitFrameBuffer(uint32_t cmd, const string& src, const string& dst, FrameBufferPieceFlags flags)
	{
		MTL::CommandBuffer* commandBuffer = mCommandQueue->commandBuffer();
		MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();

		auto sFBO = FBOManager::GetInstance()->GetFBO(src);
		auto dFBO = FBOManager::GetInstance()->GetFBO(dst);

		if (flags & ZX_FRAME_BUFFER_PIECE_COLOR)
		{
			auto sColorBuffer = GetRenderBufferByIndex(sFBO->ColorBuffer);
			auto dColorBuffer = GetRenderBufferByIndex(dFBO->ColorBuffer);

			auto sColorTexture = GetTextureByIndex(sColorBuffer->renderBuffers[mCurrentFrame]);
			auto dColorTexture = GetTextureByIndex(dColorBuffer->renderBuffers[mCurrentFrame]);

			blitEncoder->copyFromTexture(sColorTexture->texture, dColorTexture->texture);
		}

		if (flags & ZX_FRAME_BUFFER_PIECE_DEPTH)
		{
			auto sDepthBuffer = GetRenderBufferByIndex(sFBO->DepthBuffer);
			auto dDepthBuffer = GetRenderBufferByIndex(dFBO->DepthBuffer);

			auto sDepthTexture = GetTextureByIndex(sDepthBuffer->renderBuffers[mCurrentFrame]);
			auto dDepthTexture = GetTextureByIndex(dDepthBuffer->renderBuffers[mCurrentFrame]);

			blitEncoder->copyFromTexture(sDepthTexture->texture, dDepthTexture->texture);
		}

		blitEncoder->endEncoding();

		commandBuffer->commit();
	}

	FrameBufferObject* RenderAPIMetal::CreateFrameBufferObject(FrameBufferType type, unsigned int width, unsigned int height)
	{
		ClearInfo clearInfo = {};
		return CreateFrameBufferObject(type, clearInfo, width, height);
	}

	FrameBufferObject* RenderAPIMetal::CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width, unsigned int height)
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

			auto mtFBO = GetFBOByIndex(FBO->ID);
			mtFBO->colorBufferIdx = FBO->ColorBuffer;
			mtFBO->depthBufferIdx = FBO->DepthBuffer;
			mtFBO->bufferType = type;
			mtFBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
			{
				MTL::TextureDescriptor* colorBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatBGRA8Unorm,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				colorBufferDesc->setStorageMode(MTL::StorageModePrivate);
				colorBufferDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
				colorBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);
		
				colorBuffer->renderBuffers[i] = CreateMetalTexture(colorBufferDesc, width, height);

				MTL::TextureDescriptor* depthBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatDepth32Float,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				depthBufferDesc->setStorageMode(MTL::StorageModePrivate);
				depthBufferDesc->setUsage(MTL::TextureUsageRenderTarget);
				depthBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				depthBuffer->renderBuffers[i] = CreateMetalTexture(depthBufferDesc, width, height);
			}

			mtFBO->renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a));
			mtFBO->renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->depthAttachment()->setClearDepth(clearInfo.depth);

			mtFBO->inUse = true;
		}
		else if (type == FrameBufferType::Color)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = GetNextRenderBufferIndex();
			auto colorBuffer = GetRenderBufferByIndex(FBO->ColorBuffer);
			colorBuffer->inUse = true;
			FBO->DepthBuffer = UINT32_MAX;

			auto mtFBO = GetFBOByIndex(FBO->ID);
			mtFBO->colorBufferIdx = FBO->ColorBuffer;
			mtFBO->bufferType = FrameBufferType::Color;
			mtFBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
			{
				MTL::TextureDescriptor* colorBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatBGRA8Unorm,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				colorBufferDesc->setStorageMode(MTL::StorageModePrivate);
				colorBufferDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
				colorBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				colorBuffer->renderBuffers[i] = CreateMetalTexture(colorBufferDesc, width, height);
			}

			mtFBO->renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a));

			mtFBO->inUse = true;
		}
		else if (type == FrameBufferType::ShadowMap)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = UINT32_MAX;
			FBO->DepthBuffer = GetNextRenderBufferIndex();
			auto depthBuffer = GetRenderBufferByIndex(FBO->DepthBuffer);
			depthBuffer->inUse = true;

			auto mtFBO = GetFBOByIndex(FBO->ID);
			mtFBO->depthBufferIdx = FBO->DepthBuffer;
			mtFBO->bufferType = FrameBufferType::ShadowMap;
			mtFBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
			{
				MTL::TextureDescriptor* depthBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatDepth32Float,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				depthBufferDesc->setStorageMode(MTL::StorageModePrivate);
				depthBufferDesc->setUsage(MTL::TextureUsageRenderTarget);
				depthBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				depthBuffer->renderBuffers[i] = CreateMetalTexture(depthBufferDesc, width, height);
			}

			mtFBO->renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			mtFBO->renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->depthAttachment()->setClearDepth(clearInfo.depth);

			mtFBO->inUse = true;
		}
		else if (type == FrameBufferType::ShadowCubeMap)
		{
			FBO->ID = GetNextFBOIndex();
			FBO->ColorBuffer = UINT32_MAX;
			FBO->DepthBuffer = GetNextRenderBufferIndex();
			auto depthBuffer = GetRenderBufferByIndex(FBO->DepthBuffer);
			depthBuffer->inUse = true;

			auto mtFBO = GetFBOByIndex(FBO->ID);
			mtFBO->depthBufferIdx = FBO->DepthBuffer;
			mtFBO->bufferType = FrameBufferType::ShadowCubeMap;
			mtFBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
			{
				MTL::TextureDescriptor* depthBufferDesc = MTL::TextureDescriptor::textureCubeDescriptor(
					MTL::PixelFormatDepth32Float,
					static_cast<NS::Integer>(width),
					false
				);
				depthBufferDesc->setStorageMode(MTL::StorageModePrivate);
				depthBufferDesc->setUsage(MTL::TextureUsageRenderTarget);
				depthBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				depthBuffer->renderBuffers[i] = CreateMetalTexture(depthBufferDesc, width, width);
			}

			mtFBO->renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			mtFBO->renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->depthAttachment()->setClearDepth(clearInfo.depth);

			mtFBO->inUse = true;
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

			auto mtFBO = GetFBOByIndex(FBO->ID);
			mtFBO->colorBufferIdx = FBO->ColorBuffer;
			mtFBO->depthBufferIdx = FBO->DepthBuffer;
			mtFBO->positionBufferIdx = FBO->PositionBuffer;
			mtFBO->normalBufferIdx = FBO->NormalBuffer;
			mtFBO->bufferType = FrameBufferType::GBuffer;
			mtFBO->clearInfo = clearInfo;

			for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
			{
				MTL::TextureDescriptor* positionBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatRGBA32Float,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				positionBufferDesc->setStorageMode(MTL::StorageModePrivate);
				positionBufferDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
				positionBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				positionBuffer->renderBuffers[i] = CreateMetalTexture(positionBufferDesc, width, height);

				MTL::TextureDescriptor* normalBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatRGBA32Float,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				normalBufferDesc->setStorageMode(MTL::StorageModePrivate);
				normalBufferDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
				normalBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				normalBuffer->renderBuffers[i] = CreateMetalTexture(normalBufferDesc, width, height);

				MTL::TextureDescriptor* colorBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatBGRA8Unorm,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				colorBufferDesc->setStorageMode(MTL::StorageModePrivate);
				colorBufferDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
				colorBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				colorBuffer->renderBuffers[i] = CreateMetalTexture(colorBufferDesc, width, height);

				MTL::TextureDescriptor* depthBufferDesc = MTL::TextureDescriptor::texture2DDescriptor(
					MTL::PixelFormatDepth32Float,
					static_cast<NS::Integer>(width),
					static_cast<NS::Integer>(height),
					false
				);
				depthBufferDesc->setStorageMode(MTL::StorageModePrivate);
				depthBufferDesc->setUsage(MTL::TextureUsageRenderTarget);
				depthBufferDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

				depthBuffer->renderBuffers[i] = CreateMetalTexture(depthBufferDesc, width, height);
			}

			mtFBO->renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(0.0, 0.0, 0.0, 1.0));
			mtFBO->renderPassDescriptor->colorAttachments()->object(1)->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->colorAttachments()->object(1)->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->colorAttachments()->object(1)->setClearColor(MTL::ClearColor(0.0, 0.0, 0.0, 0.0));
			mtFBO->renderPassDescriptor->colorAttachments()->object(2)->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->colorAttachments()->object(2)->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->colorAttachments()->object(2)->setClearColor(MTL::ClearColor(clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a));
			mtFBO->renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
			mtFBO->renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
			mtFBO->renderPassDescriptor->depthAttachment()->setClearDepth(clearInfo.depth);

			mtFBO->inUse = true;
		}
		else if (type == FrameBufferType::RayTracing)
		{
			// 暂未实现
		}
		else
		{
			Debug::LogError("Invalide frame buffer type.");
		}

		return FBO;
	}

	void RenderAPIMetal::DeleteFrameBufferObject(FrameBufferObject* FBO)
	{
		DestroyFBOByIndex(FBO->ID);
	}

	uint32_t RenderAPIMetal::GetRenderBufferTexture(uint32_t id)
	{
		auto buffer = GetRenderBufferByIndex(id);
		return buffer->renderBuffers[mCurrentFrame];
	}

	uint32_t RenderAPIMetal::CreateStaticInstanceBuffer(uint32_t size, uint32_t num, const void* data)
	{
		uint32_t idx = GetNextInstanceBufferIndex();
		auto instanceBuffer = mMetalInstanceBufferArray[idx];
		instanceBuffer->inUse = true;

		size_t bufferSize = static_cast<size_t>(size * num) * sizeof(Vector4);
		instanceBuffer->buffer = mDevice->newBuffer(static_cast<NS::Integer>(bufferSize), MTL::ResourceStorageModePrivate);
		assert(instanceBuffer->buffer != nullptr && "Metal buffer is null");

		ImmediatelyExecute([=](MTL::CommandBuffer* cmd)
		{
			MTL::Buffer* sharedBuffer = mDevice->newBuffer(static_cast<NS::Integer>(bufferSize), MTL::ResourceStorageModeShared);
			memcpy(sharedBuffer->contents(), data, bufferSize);

			MTL::BlitCommandEncoder* blitEncoder = cmd->blitCommandEncoder();
			blitEncoder->copyFromBuffer(sharedBuffer, 0, instanceBuffer->buffer, 0, bufferSize);
			blitEncoder->endEncoding();
		});
		
		return idx;
	}

	uint32_t RenderAPIMetal::CreateDynamicInstanceBuffer(uint32_t size, uint32_t num)
	{
		uint32_t idx = GetNextInstanceBufferIndex();
		auto instanceBuffer = mMetalInstanceBufferArray[idx];
		instanceBuffer->inUse = true;

		size_t bufferSize = static_cast<size_t>(size * num) * sizeof(Vector4);
		instanceBuffer->buffer = mDevice->newBuffer(static_cast<NS::Integer>(bufferSize), MTL::ResourceStorageModeShared);
		assert(instanceBuffer->buffer != nullptr && "Metal buffer is null");

		return idx;
	}

	void RenderAPIMetal::UpdateDynamicInstanceBuffer(uint32_t id, uint32_t size, uint32_t num, const void* data)
	{
		auto instanceBuffer = mMetalInstanceBufferArray[id];

		size_t bufferSize = static_cast<size_t>(size * num) * sizeof(Vector4);

		memcpy(instanceBuffer->buffer->contents(), data, bufferSize);
	}

	void RenderAPIMetal::SetUpInstanceBufferAttribute(uint32_t VAO, uint32_t instanceBuffer, uint32_t size, uint32_t offset)
	{
		// Metal不需要实现这个接口
		return;
	};

	void RenderAPIMetal::DeleteInstanceBuffer(uint32_t id)
	{
		mInstanceBuffersToDelete.insert(pair(id, MT_MAX_FRAMES_IN_FLIGHT));
	}

	unsigned int RenderAPIMetal::LoadTexture(const char* path, int& width, int& height)
	{
		int nrComponents;
		unsigned char* pixels = Resources::LoadTexture(path, &width, &height, &nrComponents, STBI_rgb_alpha);

		uint32_t textureID = CreateMetalTexture(static_cast<uint32_t>(width), static_cast<uint32_t>(height), pixels);

		Resources::DeleteTexture(pixels);

		return textureID;
	}

	unsigned int RenderAPIMetal::LoadCubeMap(const vector<string>& faces)
	{
		array<void*, 6> textureData = {};
		int texWidth = 0, texHeight = 0, texChannels = 0;
		for (size_t i = 0; i < faces.size(); i++)
			textureData[i] = Resources::LoadTexture(faces[i], &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		uint32_t textureID = CreateMetalCubeMap(static_cast<uint32_t>(texWidth), &textureData);

		for (size_t i = 0; i < faces.size(); i++)
			stbi_image_free(textureData[i]);

		return textureID;
	}

	unsigned int RenderAPIMetal::CreateTexture(TextureFullData* data)
	{
		return CreateMetalTexture(data->width, data->height, data->data);
	}

	unsigned int RenderAPIMetal::CreateCubeMap(CubeMapFullData* data)
	{
		array<void*, 6> textureData = {};
		for (size_t i = 0; i < 6; i++)
		{
			textureData[i] = data->data[i];
		}

		return CreateMetalCubeMap(data->width, &textureData);
	}

	unsigned int RenderAPIMetal::GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data)
	{
		return CreateMetalTexture(width, height, data);
	}

	void RenderAPIMetal::DeleteTexture(unsigned int id)
	{
		mTexturesToDelete.insert(pair(id, MT_MAX_FRAMES_IN_FLIGHT));
	}

	ShaderReference* RenderAPIMetal::LoadAndSetUpShader(const string& path, FrameBufferType type)
	{
		string shaderCode = Resources::LoadTextFile(path);
		return SetUpShader(path, shaderCode, type);
	}

	ShaderReference* RenderAPIMetal::SetUpShader(const string& path, const string& shaderCode, FrameBufferType type)
	{
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::Metal);

		NS::Error* pError = nullptr;
		MTL::Library* pLibrary = mDevice->newLibrary(NS::String::string(shaderCode.c_str(), NS::StringEncoding::UTF8StringEncoding), nullptr, &pError);
		if (!pLibrary)
		{
			Debug::LogError(pError->localizedDescription()->utf8String());
			assert(false);
		}
	
		uint32_t pipelineID = GetNextPipelineIndex();
		auto pipeline = GetPipelineByIndex(pipelineID);
		pipeline->inUse = true;
		pipeline->faceCullOption = shaderInfo.stateSet.cull;
		
		MTL::Function* pVertFn = pLibrary->newFunction(NS::String::string("VertMain", NS::StringEncoding::UTF8StringEncoding));
		MTL::Function* pFragFn = pLibrary->newFunction(NS::String::string("FragMain", NS::StringEncoding::UTF8StringEncoding));
	
		// 绑定Shader入口函数
		MTL::RenderPipelineDescriptor* pipelineDesc = MTL::RenderPipelineDescriptor::alloc()->init();
		pipelineDesc->setVertexFunction(pVertFn);
		pipelineDesc->setFragmentFunction(pFragFn);

		// 设置顶点数据格式
		MTL::VertexDescriptor* vertexDesc = MTL::VertexDescriptor::alloc()->init();

		// 常规顶点数据
		auto positionAttr = vertexDesc->attributes()->object(0);
		positionAttr->setFormat(MTL::VertexFormatFloat4);
		positionAttr->setOffset(static_cast<NS::UInteger>(offsetof(Vertex, Position)));
		positionAttr->setBufferIndex(0);
		auto texCoordAttr = vertexDesc->attributes()->object(1);
		texCoordAttr->setFormat(MTL::VertexFormatFloat4);
		texCoordAttr->setOffset(static_cast<NS::UInteger>(offsetof(Vertex, TexCoords)));
		texCoordAttr->setBufferIndex(0);
		auto normalAttr = vertexDesc->attributes()->object(2);
		normalAttr->setFormat(MTL::VertexFormatFloat4);
		normalAttr->setOffset(static_cast<NS::UInteger>(offsetof(Vertex, Normal)));
		normalAttr->setBufferIndex(0);
		auto tangentAttr = vertexDesc->attributes()->object(3);
		tangentAttr->setFormat(MTL::VertexFormatFloat4);
		tangentAttr->setOffset(static_cast<NS::UInteger>(offsetof(Vertex, Tangent)));
		tangentAttr->setBufferIndex(0);
		auto weightsAttr = vertexDesc->attributes()->object(4);
		weightsAttr->setFormat(MTL::VertexFormatFloat4);
		weightsAttr->setOffset(static_cast<NS::UInteger>(offsetof(Vertex, Weights)));
		weightsAttr->setBufferIndex(0);
		auto boneIDAttr = vertexDesc->attributes()->object(5);
		boneIDAttr->setFormat(MTL::VertexFormatUInt4);
		boneIDAttr->setOffset(static_cast<NS::UInteger>(offsetof(Vertex, BoneIDs)));
		boneIDAttr->setBufferIndex(0);

		auto layoutDesc = vertexDesc->layouts()->object(0);
		layoutDesc->setStride(static_cast<NS::UInteger>(sizeof(Vertex)));
		layoutDesc->setStepFunction(MTL::VertexStepFunctionPerVertex);
		layoutDesc->setStepRate(1); // 逐顶点

		// GPU Instance数据
		if (shaderInfo.instanceInfo.size > 0)
		{
			for (NS::UInteger i = 0; i < shaderInfo.instanceInfo.size; i++)
			{
				auto instanceAttr = vertexDesc->attributes()->object(6 + i);
				instanceAttr->setFormat(MTL::VertexFormatFloat4);
				instanceAttr->setOffset(i * static_cast<NS::UInteger>(sizeof(Vector4)));
				instanceAttr->setBufferIndex(1);
			}

			auto instanceLayoutDesc = vertexDesc->layouts()->object(1);
			instanceLayoutDesc->setStride(static_cast<NS::UInteger>(shaderInfo.instanceInfo.size * sizeof(Vector4)));
			instanceLayoutDesc->setStepFunction(MTL::VertexStepFunctionPerInstance);
			instanceLayoutDesc->setStepRate(1); // 逐实例
		}

		pipelineDesc->setVertexDescriptor(vertexDesc);
		pipelineDesc->setRasterSampleCount(1); // MSAA采样数
		pipelineDesc->setAlphaToCoverageEnabled(false); // 配合MSAA使用的参数

		if (type == FrameBufferType::GBuffer)
		{
			pipelineDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA32Float);
			pipelineDesc->colorAttachments()->object(1)->setPixelFormat(MTL::PixelFormatRGBA32Float);
			pipelineDesc->colorAttachments()->object(2)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

			for (NS::Integer i = 0; i < 3; i++)
			{
				pipelineDesc->colorAttachments()->object(i)->setBlendingEnabled(true);
				pipelineDesc->colorAttachments()->object(i)->setSourceRGBBlendFactor(mtBlendFactorMap[shaderInfo.stateSet.srcFactor]);
				pipelineDesc->colorAttachments()->object(i)->setDestinationRGBBlendFactor(mtBlendFactorMap[shaderInfo.stateSet.dstFactor]);
				pipelineDesc->colorAttachments()->object(i)->setRgbBlendOperation(mtBlendOptionMap[shaderInfo.stateSet.blendOp]);
				pipelineDesc->colorAttachments()->object(i)->setSourceAlphaBlendFactor(MTL::BlendFactor::BlendFactorOne);
				pipelineDesc->colorAttachments()->object(i)->setDestinationAlphaBlendFactor(MTL::BlendFactor::BlendFactorOne);
				pipelineDesc->colorAttachments()->object(i)->setAlphaBlendOperation(MTL::BlendOperation::BlendOperationAdd);
			}
		}
		else
		{
			pipelineDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
			pipelineDesc->colorAttachments()->object(0)->setBlendingEnabled(true);
			pipelineDesc->colorAttachments()->object(0)->setSourceRGBBlendFactor(mtBlendFactorMap[shaderInfo.stateSet.srcFactor]);
			pipelineDesc->colorAttachments()->object(0)->setDestinationRGBBlendFactor(mtBlendFactorMap[shaderInfo.stateSet.dstFactor]);
			pipelineDesc->colorAttachments()->object(0)->setRgbBlendOperation(mtBlendOptionMap[shaderInfo.stateSet.blendOp]);
			pipelineDesc->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactor::BlendFactorOne);
			pipelineDesc->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactor::BlendFactorOne);
			pipelineDesc->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperation::BlendOperationAdd);
		}

		if (type == FrameBufferType::Present || type == FrameBufferType::PresentOverspread || type == FrameBufferType::Color)
		{
			pipelineDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatInvalid);
		}
		else
		{
			pipelineDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
		}

		pipelineDesc->setInputPrimitiveTopology(MTL::PrimitiveTopologyClassTriangle);

		pipeline->pipeline = mDevice->newRenderPipelineState(pipelineDesc, &pError);
		if (!pipeline->pipeline)
		{
			Debug::LogError(pError->localizedDescription()->utf8String());
			assert(false);
		}
		
		MTL::DepthStencilDescriptor* depthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
		depthStencilDesc->setDepthWriteEnabled(shaderInfo.stateSet.depthWrite);
		depthStencilDesc->setDepthCompareFunction(mtCompareOptionMap[shaderInfo.stateSet.depthCompareOp]);

		pipeline->depthStencilState = mDevice->newDepthStencilState(depthStencilDesc);

		ShaderReference* reference = new ShaderReference();
		reference->ID = pipelineID;
		reference->shaderInfo = shaderInfo;
		reference->targetFrameBufferType = type;

		return reference;
	}

	void RenderAPIMetal::DeleteShader(uint32_t id)
	{
		mPipelinesToDelete.insert(pair(id, MT_MAX_FRAMES_IN_FLIGHT));
	}

	uint32_t RenderAPIMetal::CreateMaterialData()
	{
		uint32_t materialDataID = GetNextMaterialDataIndex();
		auto materialData = GetMaterialDataByIndex(materialDataID);

		materialData->inUse = true;

		return materialDataID;
	}

	void RenderAPIMetal::SetUpMaterial(Material* material)
	{
		auto shaderReference = material->shader->reference;
		auto mtMaterialData = GetMaterialDataByIndex(material->data->GetID());

		uint32_t bufferSize = 0;
		if (shaderReference->shaderInfo.fragProperties.baseProperties.size() > 0)
			bufferSize = shaderReference->shaderInfo.fragProperties.baseProperties.back().offset + shaderReference->shaderInfo.fragProperties.baseProperties.back().size;
		else if (shaderReference->shaderInfo.vertProperties.baseProperties.size() > 0)
			bufferSize = shaderReference->shaderInfo.vertProperties.baseProperties.back().offset + shaderReference->shaderInfo.vertProperties.baseProperties.back().size;

		mtMaterialData->textures.resize(MT_MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
		{
			mtMaterialData->constantBuffers.push_back(mDevice->newBuffer(static_cast<NS::Integer>(bufferSize), MTL::ResourceStorageModeShared));

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

				mtMaterialData->textures[i][binding] = matTexture.second->GetID();
			}
		}

		// 设置材质数据
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

	void RenderAPIMetal::UseMaterialData(uint32_t ID)
	{
		mCurMaterialDataIdx = ID;
	}

	void RenderAPIMetal::DeleteMaterialData(uint32_t id)
	{
		mMaterialDatasToDelete.insert(pair(id, MT_MAX_FRAMES_IN_FLIGHT));
	}

	void RenderAPIMetal::DeleteMesh(unsigned int VAO)
	{
		mMeshsToDelete.insert(pair(VAO, MT_MAX_FRAMES_IN_FLIGHT));
	}

	void RenderAPIMetal::SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool skinned)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->indexCount = static_cast<uint32_t>(indices.size());
		meshBuffer->vertexCount = static_cast<uint32_t>(vertices.size());

		size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
		meshBuffer->vertexBuffer = CreateMetalBuffer(vertexBufferSize, MTL::ResourceStorageModePrivate, vertices.data());

		size_t indexBufferSize = indices.size() * sizeof(uint32_t);
		meshBuffer->indexBuffer = CreateMetalBuffer(indexBufferSize, MTL::ResourceStorageModePrivate, indices.data());

		meshBuffer->inUse = true;
	}

	void RenderAPIMetal::SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->indexCount = static_cast<uint32_t>(indexSize);
		meshBuffer->vertexCount = static_cast<uint32_t>(vertexSize);

		size_t vertexBufferSize = vertexSize * sizeof(Vertex);
		meshBuffer->vertexBuffer = CreateMetalBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);

		size_t indexBufferSize = indexSize * sizeof(uint32_t);
		meshBuffer->indexBuffer = CreateMetalBuffer(indexBufferSize, MTL::ResourceStorageModeShared);

		meshBuffer->inUse = true;
	}

	void RenderAPIMetal::UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
		memcpy(meshBuffer->vertexBuffer->contents(), vertices.data(), vertexBufferSize);

		size_t indexBufferSize = indices.size() * sizeof(uint32_t);
		memcpy(meshBuffer->indexBuffer->contents(), indices.data(), indexBufferSize);
	}

	void RenderAPIMetal::GenerateParticleMesh(unsigned int& VAO)
	{
		vector<Vertex> vertices =
		{
			{ .Position = {  0.5f,  0.5f, 0.0f, 1.0f }, .TexCoords = { 1.0f, 0.0f, 0.0f, 0.0f } },
			{ .Position = {  0.5f, -0.5f, 0.0f, 1.0f }, .TexCoords = { 1.0f, 1.0f, 0.0f, 0.0f } },
			{ .Position = { -0.5f,  0.5f, 0.0f, 1.0f }, .TexCoords = { 0.0f, 0.0f, 0.0f, 0.0f } },
			{ .Position = { -0.5f, -0.5f, 0.0f, 1.0f }, .TexCoords = { 0.0f, 1.0f, 0.0f, 0.0f } },
		};

		vector<uint32_t> indices =
		{
			2, 1, 3,
			2, 0, 1,
		};

		SetUpStaticMesh(VAO, vertices, indices);
	}

	void RenderAPIMetal::UseShader(unsigned int ID)
	{
		mCurPipeLineIdx = ID;
	}

	// Boolean
	void RenderAPIMetal::SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Float
	void RenderAPIMetal::SetShaderScalar(Material* material, const string& name, float value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Integer
	void RenderAPIMetal::SetShaderScalar(Material* material, const string& name, int32_t value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Unsigned Integer
	void RenderAPIMetal::SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, &value, sizeof(value));
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

			if (valueAddress != nullptr)
				memcpy(valueAddress, &value, sizeof(value));
		}
	}

	// Vector2
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer)
	{
		SetShaderVector(material, name, value, 0, allBuffer);
	}
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value.v, sizeof(float) * 2);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, value.v, sizeof(float) * 2);
		}
	}

	// Vector3
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer)
	{
		SetShaderVector(material, name, value, 0, allBuffer);
	}
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value.v, sizeof(float) * 3);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, value.v, sizeof(float) * 3);
		}
	}

	// Vector4
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer)
	{
		SetShaderVector(material, name, value, 0, allBuffer);
	}
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value.v, sizeof(float) * 4);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, value.v, sizeof(float) * 4);
		}
	}
	void RenderAPIMetal::SetShaderVector(Material* material, const string& name, const Vector4* value, uint32_t count, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, 0);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value, sizeof(Vector4) * count);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, 0);

			if (valueAddress != nullptr)
				memcpy(valueAddress, value, sizeof(Vector4) * count);
		}
	}

	// Matrix3
	void RenderAPIMetal::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer)
	{
		SetShaderMatrix(material, name, value, 0, allBuffer);
	}
	void RenderAPIMetal::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[9];
		value.ToColumnMajorArray(array);
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 9);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, array, sizeof(float) * 9);
		}
		delete[] array;
	}

	// Matrix4
	void RenderAPIMetal::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer)
	{
		SetShaderMatrix(material, name, value, 0, allBuffer);
	}
	void RenderAPIMetal::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer)
	{
		float* array = new float[16];
		value.ToColumnMajorArray(array);
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, array, sizeof(float) * 16);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

			if (valueAddress != nullptr)
				memcpy(valueAddress, array, sizeof(float) * 16);
		}
		delete[] array;
	}
	void RenderAPIMetal::SetShaderMatrix(Material* material, const string& name, const Matrix4* value, uint32_t count, bool allBuffer)
	{
		if (allBuffer)
		{
			vector<void*> valueAddresses;

			valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, 0);

			for (auto valueAddress : valueAddresses)
				memcpy(valueAddress, value, sizeof(Matrix4) * count);
		}
		else
		{
			void* valueAddress = nullptr;

			valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, 0);

			if (valueAddress != nullptr)
				memcpy(valueAddress, value, sizeof(Matrix4) * count);
		}
	}

	void RenderAPIMetal::SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());

		if (allBuffer)
		{
			for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
			{
				uint32_t textureID = ID;
				if (isBuffer)
					textureID = GetRenderBufferByIndex(ID)->renderBuffers[i];

				uint32_t binding = UINT32_MAX;

				for (auto& textureProperty : material->shader->reference->shaderInfo.fragProperties.textureProperties)
					if (name == textureProperty.name)
						binding = textureProperty.binding;

				if (binding == UINT32_MAX)
					for (auto& textureProperty : material->shader->reference->shaderInfo.vertProperties.textureProperties)
						if (name == textureProperty.name)
							binding = textureProperty.binding;

				if (binding == UINT32_MAX)
				{
					Debug::LogError("No texture found named: " + name);
					return;
				}
				
				materialData->textures[i][binding] = textureID;
			}
		}
		else
		{
			uint32_t textureID = ID;
			if (isBuffer)
				textureID = GetRenderBufferByIndex(ID)->renderBuffers[mCurrentFrame];

			uint32_t binding = UINT32_MAX;

			for (auto& textureProperty : material->shader->reference->shaderInfo.fragProperties.textureProperties)
				if (name == textureProperty.name)
					binding = textureProperty.binding;

			if (binding == UINT32_MAX)
				for (auto& textureProperty : material->shader->reference->shaderInfo.vertProperties.textureProperties)
					if (name == textureProperty.name)
						binding = textureProperty.binding;

			if (binding == UINT32_MAX)
			{
				Debug::LogError("No texture found named: " + name);
				return;
			}

			materialData->textures[mCurrentFrame][binding] = textureID;
		}
	}

	void RenderAPIMetal::SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		SetShaderTexture(material, name, ID, idx, allBuffer, isBuffer);
	}

	uint32_t RenderAPIMetal::GetNextVAOIndex()
	{
		uint32_t length = static_cast<uint32_t>(mVAOArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mVAOArray[i]->inUse)
				return i;
		}

		MetalVAO* vao = new MetalVAO();
		mVAOArray.push_back(vao);

		return length;
	}

	MetalVAO* RenderAPIMetal::GetVAOByIndex(uint32_t idx)
	{
		return mVAOArray[idx];
	}

	void RenderAPIMetal::DestroyVAOByIndex(uint32_t idx)
	{
		auto vao = mVAOArray[idx];

		vao->indexCount = 0;
		if (vao->indexBuffer)
			vao->indexBuffer->release();

		vao->vertexCount = 0;
		if (vao->vertexBuffer)
			vao->vertexBuffer->release();

		vao->inUse = false;
	}

	uint32_t RenderAPIMetal::GetNextFBOIndex()
	{
		uint32_t length = static_cast<uint32_t>(mFBOArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mFBOArray[i]->inUse)
				return i;
		}

		MetalFBO* fbo = new MetalFBO();
		mFBOArray.push_back(fbo);

		return length;
	}

	MetalFBO* RenderAPIMetal::GetFBOByIndex(uint32_t idx)
	{
		return mFBOArray[idx];
	}

	void RenderAPIMetal::DestroyFBOByIndex(uint32_t idx)
	{
		auto fbo = mFBOArray[idx];

		if (fbo->colorBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(fbo->colorBufferIdx);
			fbo->colorBufferIdx = UINT32_MAX;
		}
		if (fbo->depthBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(fbo->depthBufferIdx);
			fbo->depthBufferIdx = UINT32_MAX;
		}
		if (fbo->positionBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(fbo->positionBufferIdx);
			fbo->positionBufferIdx = UINT32_MAX;
		}
		if (fbo->normalBufferIdx != UINT32_MAX)
		{
			DestroyRenderBufferByIndex(fbo->normalBufferIdx);
			fbo->normalBufferIdx = UINT32_MAX;
		}

		fbo->bufferType = FrameBufferType::Normal;
		fbo->clearInfo = {};

		if (fbo->renderPassDescriptor)
		{
			fbo->renderPassDescriptor->release();
			fbo->renderPassDescriptor = nullptr;
		}

		fbo->inUse = false;
	}

	uint32_t RenderAPIMetal::GetNextRenderBufferIndex()
	{
		uint32_t length = static_cast<uint32_t>(mRenderBufferArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mRenderBufferArray[i]->inUse)
				return i;
		}

		MetalRenderBuffer* renderBuffer = new MetalRenderBuffer();
		renderBuffer->renderBuffers.resize(MT_MAX_FRAMES_IN_FLIGHT, UINT32_MAX);
		mRenderBufferArray.push_back(renderBuffer);

		return length;
	}

	MetalRenderBuffer* RenderAPIMetal::GetRenderBufferByIndex(uint32_t idx)
	{
		return mRenderBufferArray[idx];
	}

	void RenderAPIMetal::DestroyRenderBufferByIndex(uint32_t idx)
	{
		auto renderBuffer = mRenderBufferArray[idx];

		for (auto& buffer : renderBuffer->renderBuffers)
		{
			auto texture = GetTextureByIndex(buffer);
			texture->texture->release();
			texture->texture = nullptr;
		}

		renderBuffer->renderBuffers.resize(MT_MAX_FRAMES_IN_FLIGHT, UINT32_MAX);

		renderBuffer->inUse = false;
	}

	uint32_t RenderAPIMetal::GetNextTextureIndex()
	{
		uint32_t length = static_cast<uint32_t>(mMetalTextureArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mMetalTextureArray[i]->inUse)
				return i;
		}

		MetalTexture* texture = new MetalTexture();
		mMetalTextureArray.push_back(texture);

		return length;
	}

	MetalTexture* RenderAPIMetal::GetTextureByIndex(uint32_t idx)
	{
		return mMetalTextureArray[idx];
	}

	void RenderAPIMetal::DestroyTextureByIndex(uint32_t idx)
	{
		auto texture = mMetalTextureArray[idx];

		if (texture->texture)
		{
			texture->texture->release();
			texture->texture = nullptr;
		}

		texture->inUse = false;
	}

	uint32_t RenderAPIMetal::GetNextPipelineIndex()
	{
		uint32_t length = static_cast<uint32_t>(mMetalPipelineArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mMetalPipelineArray[i]->inUse)
				return i;
		}

		MetalPipeline* pipeline = new MetalPipeline();
		mMetalPipelineArray.push_back(pipeline);

		return length;
	}

	MetalPipeline* RenderAPIMetal::GetPipelineByIndex(uint32_t idx)
	{
		return mMetalPipelineArray[idx];
	}

	void RenderAPIMetal::DestroyPipelineByIndex(uint32_t idx)
	{
		auto pipeline = mMetalPipelineArray[idx];

		if (pipeline->pipeline)
		{
			pipeline->pipeline->release();
			pipeline->pipeline = nullptr;
		}

		if (pipeline->depthStencilState)
		{
			pipeline->depthStencilState->release();
			pipeline->depthStencilState = nullptr;
		}

		pipeline->inUse = false;
	}

	uint32_t RenderAPIMetal::GetNextMaterialDataIndex()
	{
		uint32_t length = static_cast<uint32_t>(mMaterialDataArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mMaterialDataArray[i]->inUse)
				return i;
		}

		MetalMaterialData* materialData = new MetalMaterialData();
		mMaterialDataArray.push_back(materialData);

		return length;
	}

	MetalMaterialData* RenderAPIMetal::GetMaterialDataByIndex(uint32_t idx)
	{
		return mMaterialDataArray[idx];
	}
	
	void RenderAPIMetal::DestroyMaterialDataByIndex(uint32_t idx)
	{
		auto materialData = mMaterialDataArray[idx];

		for (auto buffer : materialData->constantBuffers)
		{
			if (buffer)
			{
				buffer->release();
				buffer = nullptr;
			}
		}
		materialData->constantBuffers.clear();

		materialData->textures.clear();

		materialData->inUse = false;
	}

	uint32_t RenderAPIMetal::GetNextInstanceBufferIndex()
	{
		uint32_t length = static_cast<uint32_t>(mMetalInstanceBufferArray.size());

		for (uint32_t i = 0; i < length; i++)
		{
			if (!mMetalInstanceBufferArray[i]->inUse)
				return i;
		}

		MetalBuffer* buffer = new MetalBuffer();
		mMetalInstanceBufferArray.push_back(buffer);

		return length;
	}

	MetalBuffer* RenderAPIMetal::GetInstanceBufferByIndex(uint32_t idx)
	{
		return mMetalInstanceBufferArray[idx];
	}

	void RenderAPIMetal::DestroyInstanceBufferByIndex(uint32_t idx)
	{
		auto instanceBuffer = mMetalInstanceBufferArray[idx];

		if (instanceBuffer->buffer)
		{
			instanceBuffer->buffer->release();
			instanceBuffer->buffer = nullptr;
		}

		instanceBuffer->inUse = false;
	}

	MTL::Buffer* RenderAPIMetal::CreateMetalBuffer(size_t size, MTL::ResourceOptions resOpt, const void* data)
	{
		MTL::Buffer* buffer = mDevice->newBuffer(static_cast<NS::Integer>(size), resOpt);
		assert(buffer != nullptr && "Metal buffer is null");

		if (data)
		{
			if (resOpt == MTL::ResourceStorageModePrivate)
			{
				ImmediatelyExecute([=](MTL::CommandBuffer* cmd)
				{
					MTL::Buffer* sharedBuffer = mDevice->newBuffer(static_cast<NS::Integer>(size), MTL::ResourceStorageModeShared);
					memcpy(sharedBuffer->contents(), data, size);
		
					MTL::BlitCommandEncoder* blitEncoder = cmd->blitCommandEncoder();
					blitEncoder->copyFromBuffer(sharedBuffer, 0, buffer, 0, static_cast<NS::Integer>(size));
					blitEncoder->endEncoding();
				});
			}
			else
			{
				memcpy(buffer->contents(), data, size);
			}
		}

		return buffer;
	}

	uint32_t RenderAPIMetal::CreateMetalTexture(uint32_t width, uint32_t height, void* data)
	{
		MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
			MTL::PixelFormatBGRA8Unorm,
			static_cast<NS::Integer>(width),
			static_cast<NS::Integer>(height),
			true
		);

		desc->setStorageMode(MTL::StorageModePrivate);
		desc->setUsage(MTL::TextureUsageShaderRead);
		desc->setResourceOptions(MTL::ResourceStorageModePrivate);

		return CreateMetalTexture(desc, width, height, data);
	}

	uint32_t RenderAPIMetal::CreateMetalTexture(MTL::TextureDescriptor* desc, uint32_t width, uint32_t height, void* data)
	{
		MTL::Texture* texture = mDevice->newTexture(desc);

		if (data)
			FillGPUTexture(texture, data, width, height);

		uint32_t textureID = GetNextTextureIndex();
		auto mtTexture = GetTextureByIndex(textureID);
		mtTexture->texture = texture;
		mtTexture->inUse = true;

		return textureID;
	}

	uint32_t RenderAPIMetal::CreateMetalCubeMap(uint32_t width, const array<void*, 6>* datas)
	{
		MTL::TextureDescriptor* desc = MTL::TextureDescriptor::textureCubeDescriptor(
			MTL::PixelFormatBGRA8Unorm,
			static_cast<NS::Integer>(width),
			true
		);
		desc->setStorageMode(MTL::StorageModePrivate);
		desc->setUsage(MTL::TextureUsageShaderRead);
		desc->setResourceOptions(MTL::ResourceStorageModePrivate);
		
		return CreateMetalCubeMap(desc, width, datas);
	}

	uint32_t RenderAPIMetal::CreateMetalCubeMap(MTL::TextureDescriptor* desc, uint32_t width, const array<void*, 6>* datas)
	{
		MTL::Texture* texture = mDevice->newTexture(desc);

		if (datas)
			FillGPUCubeMap(texture, datas, width);

		uint32_t textureID = GetNextTextureIndex();
		auto mtTexture = GetTextureByIndex(textureID);
		mtTexture->texture = texture;
		mtTexture->inUse = true;

		return textureID;
	}

	void RenderAPIMetal::FillGPUTexture(MTL::Texture* texture, const void* data, uint32_t width, uint32_t height)
	{
		MTL::TextureDescriptor* tempTextureDesc = MTL::TextureDescriptor::texture2DDescriptor(
			texture->pixelFormat(),
			width,
			height,
			texture->mipmapLevelCount() > 1
		);
		tempTextureDesc->setStorageMode(MTL::StorageModeShared);
	
		MTL::Texture* tempTexture = mDevice->newTexture(tempTextureDesc);

		MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
		// 这里默认每个像素4字节
		tempTexture->replaceRegion(region, 0, data, width * 4);
	
		ImmediatelyExecute([=](MTL::CommandBuffer* cmd)
		{
			MTL::BlitCommandEncoder* blitEncoder = cmd->blitCommandEncoder();
			blitEncoder->copyFromTexture(tempTexture, texture);
			blitEncoder->endEncoding();
		});

		tempTexture->release();
	}

	void RenderAPIMetal::FillGPUCubeMap(MTL::Texture* texture, const array<void*, 6>* datas, uint32_t width)
	{
		MTL::TextureDescriptor* tempTextureDesc = MTL::TextureDescriptor::textureCubeDescriptor(
			texture->pixelFormat(),
			width,
			texture->mipmapLevelCount() > 1
		);
		tempTextureDesc->setStorageMode(MTL::StorageModeShared);

		MTL::Texture* tempTexture = mDevice->newTexture(tempTextureDesc);

		for (size_t i = 0; i < 6; i++)
		{
			MTL::Region region = MTL::Region::Make2D(0, 0, width, width);
			tempTexture->replaceRegion(region, 0, static_cast<NS::Integer>(i), datas->at(i),
				static_cast<NS::Integer>(width * 4), static_cast<NS::Integer>(width * width * 4));
		}

		ImmediatelyExecute([=](MTL::CommandBuffer* cmd)
		{
			MTL::BlitCommandEncoder* blitEncoder = cmd->blitCommandEncoder();
			blitEncoder->copyFromTexture(tempTexture, texture);
			blitEncoder->endEncoding();
		});

		tempTexture->release();
	}

	void RenderAPIMetal::CheckDeleteData()
	{
		vector<uint32_t> deleteList = {};

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

		// Shader
		deleteList.clear();
		for (auto& iter : mPipelinesToDelete)
		{
			if (iter.second > 0)
				iter.second--;
			else
				deleteList.push_back(iter.first);
		}
		for (auto id : deleteList)
		{
			DestroyPipelineByIndex(id);
			mPipelinesToDelete.erase(id);
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

	void* RenderAPIMetal::GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx)
	{
		auto materialData = GetMaterialDataByIndex(materialDataID);

		for (auto& property : reference->shaderInfo.vertProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame]->contents()) + property.offset + property.arrayOffset * idx);

		for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
			if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[mCurrentFrame]->contents()) + property.offset + property.arrayOffset * idx);

		Debug::LogError("Could not find shader property named " + name);

		return nullptr;
	}

	vector<void*> RenderAPIMetal::GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx)
	{
		vector<void*> addresses;
		auto materialData = GetMaterialDataByIndex(materialDataID);

		for (auto& property : reference->shaderInfo.vertProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i]->contents()) + addressOffset));
				return addresses;
			}
		}

		for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
		{
			if (name == property.name)
			{
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < MT_MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(materialData->constantBuffers[i]->contents()) + addressOffset));
				return addresses;
			}
		}

		Debug::LogError("Could not find shader property named " + name);

		return addresses;
	}

	void RenderAPIMetal::ImmediatelyExecute(std::function<void(MTL::CommandBuffer* cmd)>&& function)
	{
		MTL::CommandBuffer* commandBuffer = mCommandQueue->commandBuffer();

		function(commandBuffer);

		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
	}
}