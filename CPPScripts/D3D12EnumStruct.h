#pragma once
#include "pubh.h"
#include <DirectX/d3dx12.h>
#include <wrl.h>
#include <comdef.h>
#include <dxgi1_4.h>

// ����D3D12����ؿ�
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;

namespace ZXEngine
{
    const uint32_t DX_MAX_FRAMES_IN_FLIGHT = 2;

    struct ZXD3D12Command
    {
        // Allocator��CommandList��ʵ����1��1�ģ�һ��Allocator���Զ�Ӧ���CommandList
        // ������һ��Allocator����Ӧ������CommandList�У�ֻ����һ��CommandList����Recording״̬
        // ������΢�е�͵���ˣ���ֱ�Ӹ��1��1��������������С
        ComPtr<ID3D12CommandAllocator> allocator = nullptr;
        ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
    };

    struct ZXD3D12Fence
    {
        UINT64 currentFence = 0;
        ComPtr<ID3D12Fence> fence = nullptr;
        bool inUse = false;
    };

    struct ZXD3D12Texture
    {
        ComPtr<ID3D12Resource> texture = nullptr;
        bool inUse = false;
    };
}