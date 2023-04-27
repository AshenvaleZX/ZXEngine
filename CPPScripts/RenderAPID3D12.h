#pragma once
#include "RenderAPI.h"
#include "DirectX12/D3D12EnumStruct.h"

namespace ZXEngine
{
	class RenderAPID3D12 : public RenderAPI
	{
		friend class ZXD3D12DescriptorAllocator;
		/// <summary>
		/// 标准RenderAPI接口
		/// </summary>
	public:
		RenderAPID3D12();
		~RenderAPID3D12() {};

		virtual void BeginFrame();
		virtual void EndFrame();

		// FrameBuffer
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0);
		virtual void DeleteFrameBufferObject(FrameBufferObject* FBO);

		// 纹理
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual unsigned int LoadCubeMap(const vector<string>& faces);
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
		virtual void DeleteTexture(unsigned int id);

		// Shader
		virtual ShaderReference* LoadAndSetUpShader(const char* path, FrameBufferType type);
		virtual void DeleteShader(uint32_t id);

		// 材质
		virtual uint32_t CreateMaterialData();
		virtual void SetUpMaterial(ShaderReference* shaderReference, MaterialData* materialData);
		virtual void UseMaterialData(uint32_t ID);
		virtual void DeleteMaterialData(uint32_t id);

		// Draw
		virtual uint32_t AllocateDrawCommand(CommandType commandType);
		virtual void Draw(uint32_t VAO);
		virtual void GenerateDrawCommand(uint32_t id);

		// Mesh
		virtual void DeleteMesh(unsigned int VAO);
		virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
		virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void GenerateParticleMesh(unsigned int& VAO);

		// Shader参数
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderScalar(Material* material, const string& name, int value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, float value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);
		virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);


		/// <summary>
		/// 仅启动时一次性初始化的核心D3D12组件及相关变量
		/// </summary>
	private:
		const UINT mSwapChainBufferCount = 2;

		// 各类型描述符大小
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		// 屏幕后台缓冲区图像格式
		DXGI_FORMAT mPresentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 默认的纹理和FrameBuffer色彩空间
		const DXGI_FORMAT mDefaultImageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 4X MSAA质量等级
		UINT m4xMSAAQuality = 0;
		UINT msaaSamplesCount = 4;

		// 当前是DX_MAX_FRAMES_IN_FLIGHT中的第几帧
		uint32_t mCurrentFrame = 0;

		UINT64 mCurrentFence = 0;
		ComPtr<ID3D12Fence> mFence;

		ComPtr<IDXGIFactory4> mDXGIFactory;
		ComPtr<ID3D12Device> mD3D12Device;
		ComPtr<IDXGISwapChain> mSwapChain;

		ComPtr<ID3D12CommandQueue> mCommandQueue;

		void InitD3D12();
		void GetDeviceProperties();
		void CreateSwapChain();


		/// <summary>
		/// D3D12资源，以及相关创建销毁接口
		/// </summary>
	private:
		vector<ZXD3D12Fence*> mFenceArray;
		vector<ZXD3D12VAO*> mVAOArray;
		vector<ZXD3D12FBO*> mFBOArray;
		vector<ZXD3D12RenderBuffer*> mRenderBufferArray;
		vector<ZXD3D12Texture*> mTextureArray;
		vector<ZXD3D12Pipeline*> mPipelineArray;
		vector<ZXD3D12MaterialData*> mMaterialDataArray;
		vector<ZXD3D12DrawCommand*> mDrawCommandArray;

		uint32_t GetNextFenceIndex();
		ZXD3D12Fence* GetFenceByIndex(uint32_t idx);
		void DestroyFenceByIndex(uint32_t idx);
		uint32_t GetNextVAOIndex();
		ZXD3D12VAO* GetVAOByIndex(uint32_t idx);
		void DestroyVAOByIndex(uint32_t idx);
		uint32_t GetNextFBOIndex();
		ZXD3D12FBO* GetFBOByIndex(uint32_t idx);
		void DestroyFBOByIndex(uint32_t idx);
		uint32_t GetNextRenderBufferIndex();
		ZXD3D12RenderBuffer* GetRenderBufferByIndex(uint32_t idx);
		void DestroyRenderBufferByIndex(uint32_t idx);
		uint32_t GetNextTextureIndex();
		ZXD3D12Texture* GetTextureByIndex(uint32_t idx);
		void DestroyTextureByIndex(uint32_t idx);
		uint32_t GetNextPipelineIndex();
		ZXD3D12Pipeline* GetPipelineByIndex(uint32_t idx);
		void DestroyPipelineByIndex(uint32_t idx);
		uint32_t GetNextMaterialDataIndex();
		ZXD3D12MaterialData* GetMaterialDataByIndex(uint32_t idx);
		void DestroyMaterialDataByIndex(uint32_t idx);
		uint32_t GetNextDrawCommandIndex();
		ZXD3D12DrawCommand* GetDrawCommandByIndex(uint32_t idx);

		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc);
		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, const D3D12_DEPTH_STENCIL_VIEW_DESC& dsvDesc);
		ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize);

		ZXD3D12ConstantBuffer CreateConstantBuffer(UINT64 byteSize);


		/// <summary>
		/// 其它辅助接口
		/// </summary>
	private:
		UINT64 mImmediateExeFenceValue = 0;
		ComPtr<ID3D12Fence> mImmediateExeFence;
		ComPtr<ID3D12CommandAllocator> mImmediateExeAllocator;
		ComPtr<ID3D12GraphicsCommandList> mImmediateExeCommandList;

		uint32_t mCurPipeLineIdx = 0;
		uint32_t mCurMaterialDataIdx = 0;

		vector<ZXD3D12DrawIndex> mDrawIndexes;

		void* GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);
		vector<void*> GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);

		array<const CD3DX12_STATIC_SAMPLER_DESC, 4> GetStaticSamplersDesc();
		void InitImmediateExecution();
		void ImmediatelyExecute(std::function<void(ComPtr<ID3D12GraphicsCommandList> cmdList)>&& function);
	};
}