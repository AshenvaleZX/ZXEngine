#pragma once
#include "../pubh.h"
#include <DirectX/d3dx12.h>
#include <wrl.h>
#include <comdef.h>
#include <dxgi1_4.h>
#include "../PublicStruct.h"

// 链接D3D12的相关库
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

    struct ZXD3D12DrawIndex
    {
        uint32_t VAO = 0;
        uint32_t pipelineID = 0;
        uint32_t materialDataID = 0;
    };

    struct ZXD3D12DrawCommand
    {
        // Allocator和CommandList其实不用1对1的，一个Allocator可以对应多个CommandList
        // 但是在一个Allocator所对应的所有CommandList中，只能有一个CommandList处于Recording状态
        // 这里稍微有点偷懒了，先直接搞成1对1，反正现在数量小
        vector<ComPtr<ID3D12CommandAllocator>> allocators;
        vector<ComPtr<ID3D12GraphicsCommandList>> commandLists;
        bool inUse = false;
    };

    struct ZXD3D12Fence
    {
        UINT64 currentFence = 0;
        ComPtr<ID3D12Fence> fence = nullptr;
        bool inUse = false;
    };

    struct ZXD3D12DescriptorHandle
    {
        // 在描述符堆里的索引
        uint32_t descriptorIdx = UINT32_MAX;
        // 在描述符堆数组里的索引
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

    struct ZXD3D12Pipeline
    {
        ComPtr<ID3D12RootSignature> rootSignature = nullptr;
        ComPtr<ID3D12PipelineState> pipelineState = nullptr;
        bool inUse = false;
	};

    struct ZXD3D12ConstantBuffer
    {
        ComPtr<ID3D12Resource> constantBuffer = nullptr;
        void* constantBufferAddress = nullptr;
    };

    struct ZXD3D12MaterialTextureSet
    {
        vector<ZXD3D12DescriptorHandle> textureHandles;
    };

    struct ZXD3D12MaterialData
    {
        vector<ZXD3D12ConstantBuffer> constantBuffers;
        vector<ZXD3D12MaterialTextureSet> textureSets;
        bool inUse = false;
    };
}