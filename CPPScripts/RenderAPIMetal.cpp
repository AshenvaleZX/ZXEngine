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
		mMetalView->setDepthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);

		NS::Window* nsWindow = static_cast<NS::Window*>(WindowManager::GetInstance()->GetWindow());
		nsWindow->setContentView(mMetalView);

		mSemaphore = dispatch_semaphore_create(MT_MAX_FRAMES_IN_FLIGHT);
	}
}