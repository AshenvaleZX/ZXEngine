#include "RenderAPIMetal.h"
#include "ProjectSetting.h"
#include "Window/WindowManager.h"

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

	void RenderAPIMetal::CheckDeleteData()
	{
		vector<uint32_t> deleteList = {};

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