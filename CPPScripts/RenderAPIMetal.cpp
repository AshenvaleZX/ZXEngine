#include "RenderAPIMetal.h"
#include "ProjectSetting.h"
#include "Window/WindowManager.h"
#include "Resources.h"
#include "ShaderParser.h"

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

		uint32_t textureID = CreateMetalCubeMap(static_cast<uint32_t>(texWidth), textureData);

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

		return CreateMetalCubeMap(data->width, textureData);
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

	uint32_t RenderAPIMetal::CreateMetalTexture(uint32_t width, uint32_t height, void* data)
	{
		MTL::TextureDescriptor* textureDesc = MTL::TextureDescriptor::texture2DDescriptor(
			MTL::PixelFormatBGRA8Unorm,
			static_cast<NS::Integer>(width),
			static_cast<NS::Integer>(height),
			true
		);
		textureDesc->setStorageMode(MTL::StorageModePrivate);
		textureDesc->setUsage(MTL::TextureUsageShaderRead);
		textureDesc->setResourceOptions(MTL::ResourceStorageModePrivate);

		MTL::Texture* texture = mDevice->newTexture(textureDesc);

		FillGPUTexture(texture, data, width, height);

		uint32_t textureID = GetNextTextureIndex();
		auto mtTexture = GetTextureByIndex(textureID);
		mtTexture->texture = texture;
		mtTexture->inUse = true;

		return textureID;
	}

	uint32_t RenderAPIMetal::CreateMetalCubeMap(uint32_t width, const array<void*, 6>& datas)
	{
		MTL::TextureDescriptor* textureDesc = MTL::TextureDescriptor::textureCubeDescriptor(
			MTL::PixelFormatBGRA8Unorm,
			static_cast<NS::Integer>(width),
			true
		);
		textureDesc->setStorageMode(MTL::StorageModePrivate);
		textureDesc->setUsage(MTL::TextureUsageShaderRead);
		textureDesc->setResourceOptions(MTL::ResourceStorageModePrivate);
		
		MTL::Texture* texture = mDevice->newTexture(textureDesc);

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

	void RenderAPIMetal::FillGPUCubeMap(MTL::Texture* texture, const array<void*, 6>& datas, uint32_t width)
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
			tempTexture->replaceRegion(region, 0, static_cast<NS::Integer>(i), datas[i],
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

	void RenderAPIMetal::ImmediatelyExecute(std::function<void(MTL::CommandBuffer* cmd)>&& function)
	{
		MTL::CommandBuffer* commandBuffer = mCommandQueue->commandBuffer();

		function(commandBuffer);

		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
	}
}