#pragma once
#include "pubh.h"
#include <wrl.h>
#include <d3d12.h>
#include <comdef.h>
#include <dxgi1_4.h>

// 链接D3D12的相关库
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

namespace ZXEngine
{
    const uint32_t DX_MAX_FRAMES_IN_FLIGHT = 2;

    struct D3D12Command
    {
        // Allocator和CommandList其实不用1对1的，一个Allocator可以对应多个CommandList
        // 但是在一个Allocator所对应的所有CommandList中，只能有一个CommandList处于Recording状态
        // 这里稍微有点偷懒了，先直接搞成1对1，反正现在数量小
        ComPtr<ID3D12CommandAllocator> allocator;
        ComPtr<ID3D12GraphicsCommandList> commandList;
    };
}