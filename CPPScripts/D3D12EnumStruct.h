#pragma once
#include "pubh.h"
#include <DirectX/d3dx12.h>
#include <wrl.h>
#include <comdef.h>
#include <dxgi1_4.h>
#include "PublicStruct.h"

// ����D3D12����ؿ�
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;

namespace ZXEngine
{
    const uint32_t DX_MAX_FRAMES_IN_FLIGHT = 2;

    typedef uint32_t ZX_D3D12_TEXTURE_USAGE_FLAGS;
    typedef enum ZX_D3D12_TEXTURE_USAGE {
        ZX_D3D12_TEXTURE_USAGE_NONE_BIT = 0x00000000,
        ZX_D3D12_TEXTURE_USAGE_SRV_BIT = 0x00000001,
        ZX_D3D12_TEXTURE_USAGE_RTV_BIT = 0x00000002,
        ZX_D3D12_TEXTURE_USAGE_DSV_BIT = 0x00000004,
    } ZX_D3D12_TEXTURE_USAGE;

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

    struct ZXD3D12DescriptorHandle
    {
        uint32_t descriptorIdx = UINT32_MAX;
        uint32_t descriptorHeapIdx = UINT32_MAX;
        D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    };

    struct ZXD3D12Texture
    {
        ComPtr<ID3D12Resource> texture = nullptr;
        ZXD3D12DescriptorHandle handleSRV = {};
        ZXD3D12DescriptorHandle handleRTV = {};
        ZXD3D12DescriptorHandle handleDSV = {};
        ZX_D3D12_TEXTURE_USAGE_FLAGS usageFlags = ZX_D3D12_TEXTURE_USAGE_NONE_BIT;
        bool inUse = false;
    };

    struct ZXD3D12VAO
    {
        UINT size = 0;

        ComPtr<ID3D12Resource> indexBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
        void* indexBufferAddress = nullptr; // Only for dynamic mesh

        ComPtr<ID3D12Resource> vertexBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
        void* vertexBufferAddress = nullptr; // Only for dynamic mesh

        bool inUse = false;
    };

    struct ZXD3D12RenderBuffer
    {
        vector<uint32_t> renderBuffers;
        bool inUse = false;
    };

    struct ZXD3D12FBO
    {
        uint32_t colorBufferIdx = UINT32_MAX;
        uint32_t depthBufferIdx = UINT32_MAX;
        FrameBufferType bufferType = FrameBufferType::Normal;
        ClearInfo clearInfo = {};
        bool inUse = false;
    };
}