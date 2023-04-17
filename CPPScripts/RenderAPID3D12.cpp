#include "RenderAPID3D12.h"
#include "ProjectSetting.h"

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

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ZXEngine
{
	RenderAPID3D12::RenderAPID3D12()
	{
		InitWindow();
		InitD3D12();
		GetDeviceProperties();
		CreateCommandResources();

		ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	}

	void RenderAPID3D12::InitWindow()
	{
		WNDCLASSEXW wndClass = {};
		wndClass.cbSize = sizeof(wndClass);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = GetModuleHandle(NULL);
		wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(0, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wndClass.lpszMenuName = 0;
		wndClass.lpszClassName = L"MainWnd";
		wndClass.hIconSm = LoadIcon(0, IDI_APPLICATION);

		RegisterClassExW(&wndClass);

		mMainWnd = CreateWindowW(wndClass.lpszClassName, L"ZXEngine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			ProjectSetting::srcWidth, ProjectSetting::srcHeight, NULL, NULL, wndClass.hInstance, NULL);

		ShowWindow(mMainWnd, SW_SHOW);
		UpdateWindow(mMainWnd);
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

		ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
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
		swapChainDesc.OutputWindow = mMainWnd;
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
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}