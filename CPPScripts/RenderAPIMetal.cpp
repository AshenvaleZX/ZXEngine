#include "RenderAPIMetal.h"
#include "ProjectSetting.h"
#include "Window/WindowManager.h"
#include "Resources.h"

namespace ZXEngine
{
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