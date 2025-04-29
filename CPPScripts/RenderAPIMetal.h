#pragma once
#include "RenderAPI.h"
#include "Metal/MetalEnumStruct.h"

namespace ZXEngine
{
	class RenderAPIMetal : public RenderAPI
	{
		/// <summary>
		/// 标准渲染管线接口
		/// Standard Rendering Pipeline Interface
		/// </summary>
	public:
		RenderAPIMetal();
		~RenderAPIMetal() {};

		virtual void BeginFrame();
		virtual void EndFrame();

		// 渲染状态
		virtual void OnWindowSizeChange(uint32_t width, uint32_t height);
		virtual void OnGameViewSizeChange();
		virtual void SetRenderState(RenderStateSetting* state);
		virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);
		virtual void WaitForRenderFinish();

		// Frame Buffer
		virtual void SwitchFrameBuffer(uint32_t id);
		virtual void ClearFrameBuffer(FrameBufferClearFlags clearFlags);
		virtual void BlitFrameBuffer(uint32_t cmd, const string& src, const string& dst, FrameBufferPieceFlags flags);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0);
		virtual void DeleteFrameBufferObject(FrameBufferObject* FBO);
		virtual uint32_t GetRenderBufferTexture(uint32_t id);

		// Instance Buffer
		virtual uint32_t CreateStaticInstanceBuffer(uint32_t size, uint32_t num, const void* data);
		virtual uint32_t CreateDynamicInstanceBuffer(uint32_t size, uint32_t num);
		virtual void UpdateDynamicInstanceBuffer(uint32_t id, uint32_t size, uint32_t num, const void* data);
		virtual void SetUpInstanceBufferAttribute(uint32_t VAO, uint32_t instanceBuffer, uint32_t size, uint32_t offset = 6);
		virtual void DeleteInstanceBuffer(uint32_t id);

		// 纹理
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual unsigned int LoadCubeMap(const vector<string>& faces);
		virtual unsigned int CreateTexture(TextureFullData* data);
		virtual unsigned int CreateCubeMap(CubeMapFullData* data);
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
		virtual void DeleteTexture(unsigned int id);

		// Shader
		virtual ShaderReference* LoadAndSetUpShader(const string& path, FrameBufferType type);
		virtual ShaderReference* SetUpShader(const string& path, const string& shaderCode, FrameBufferType type);
		virtual void DeleteShader(uint32_t id);

		// 材质
		virtual uint32_t CreateMaterialData();
		virtual void SetUpMaterial(Material* material);
		virtual void UseMaterialData(uint32_t ID);
		virtual void DeleteMaterialData(uint32_t id);

		// Draw
		virtual uint32_t AllocateDrawCommand(CommandType commandType, FrameBufferClearFlags clearFlags);
		virtual void FreeDrawCommand(uint32_t commandID);
		virtual void Draw(uint32_t VAO);
		virtual void DrawInstanced(uint32_t VAO, uint32_t instanceNum, uint32_t instanceBuffer);
		virtual void GenerateDrawCommand(uint32_t id);

		// Mesh
		virtual void DeleteMesh(unsigned int VAO);
		virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool skinned = false);
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
		virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
		virtual void GenerateParticleMesh(unsigned int& VAO);

		// Shader参数
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, float value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, int32_t value, bool allBuffer = false);
		virtual void SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderVector(Material* material, const string& name, const Vector4* value, uint32_t count, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false);
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4* value, uint32_t count, bool allBuffer = false);
		virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);
		virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);


		/// <summary>
		/// 通用计算管线接口
		/// Compute Pipeline Interface 
		/// </summary>
	public:
		// Shader Storage Buffer
		virtual uint32_t CreateShaderStorageBuffer(const void* data, size_t size, GPUBufferType type) { return 0; };
		virtual void BindShaderStorageBuffer(uint32_t id, uint32_t binding) {};
		virtual void UpdateShaderStorageBuffer(uint32_t id, const void* data, size_t size) {};
		virtual void DeleteShaderStorageBuffer(uint32_t id) {};

		// Vertex Buffer Binding
		virtual void BindVertexBuffer(uint32_t VAO, uint32_t binding) {};

		// Compute Shader
		virtual ComputeShaderReference* LoadAndSetUpComputeShader(const string& path) { return nullptr; };
		virtual void DeleteComputeShader(uint32_t id) {};

		// Compute Command
		virtual void Dispatch(uint32_t commandID, uint32_t shaderID, uint32_t groupX, uint32_t groupY, uint32_t groupZ) {};
		virtual void SubmitAllComputeCommands() {};


		/// <summary>
		/// 光线追踪管线接口 (暂未实现基于Metal的光线追踪)
		/// Ray Tracing Pipeline Interface (Not Implemented for Metal yet)
		/// </summary>
	public:
		// Pipeline
		virtual uint32_t CreateRayTracingPipeline(const RayTracingShaderPathGroup& rtShaderPathGroup) { return 0; };
		virtual void SwitchRayTracingPipeline(uint32_t rtPipelineID) {};

		// Material
		virtual uint32_t CreateRayTracingMaterialData() { return 0; };
		virtual void SetUpRayTracingMaterialData(Material* material) {};
		virtual void DeleteRayTracingMaterialData(uint32_t id) {};

		// 数据更新
		virtual void SetRayTracingSkyBox(uint32_t textureID) {};
		virtual void PushRayTracingMaterialData(Material* material) {};
		virtual void PushAccelerationStructure(uint32_t VAO, uint32_t hitGroupIdx, uint32_t rtMaterialDataID, const Matrix4& transform) {};

		// Ray Trace
		virtual void RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants) {};

		// Acceleration Structure
		virtual void BuildTopLevelAccelerationStructure(uint32_t commandID) {};
		virtual void BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact) {};


		/// <summary>
		/// 仅启动时一次性初始化的核心Metal组件及相关变量
		/// </summary>
	private:
		MTL::Device* mDevice;
		MTL::CommandQueue* mCommandQueue;
		CA::MetalLayer* mMetalLayer;
		// 控制帧同步的信号量
		dispatch_semaphore_t mSemaphore;
		// 当前是MT_MAX_FRAMES_IN_FLIGHT中的第几帧
		uint32_t mCurrentFrame = 0;


		/// <summary>
		/// Metal资源创建相关接口
		/// </summary>
	private:
		vector<MetalVAO*> mVAOArray;
		vector<MetalFBO*> mFBOArray;
		vector<MetalRenderBuffer*> mRenderBufferArray;
		vector<MetalTexture*> mMetalTextureArray;
		vector<MetalPipeline*> mMetalPipelineArray;
		vector<MetalMaterialData*> mMaterialDataArray;
		vector<MetalBuffer*> mMetalInstanceBufferArray;
		vector<MetalDrawCommand*> mDrawCommandArray;

		unordered_map<uint32_t, uint32_t> mMeshsToDelete;
		unordered_map<uint32_t, uint32_t> mTexturesToDelete;
		unordered_map<uint32_t, uint32_t> mPipelinesToDelete;
		unordered_map<uint32_t, uint32_t> mMaterialDatasToDelete;
		unordered_map<uint32_t, uint32_t> mInstanceBuffersToDelete;
		unordered_map<uint32_t, uint32_t> mDrawCommandsToDelete;

		uint32_t GetNextVAOIndex();
		MetalVAO* GetVAOByIndex(uint32_t idx);
		void DestroyVAOByIndex(uint32_t idx);

		uint32_t GetNextFBOIndex();
		MetalFBO* GetFBOByIndex(uint32_t idx);
		void DestroyFBOByIndex(uint32_t idx);

		uint32_t GetNextRenderBufferIndex();
		MetalRenderBuffer* GetRenderBufferByIndex(uint32_t idx);
		void DestroyRenderBufferByIndex(uint32_t idx);

		uint32_t GetNextTextureIndex();
		MetalTexture* GetTextureByIndex(uint32_t idx);
		void DestroyTextureByIndex(uint32_t idx);

		uint32_t GetNextPipelineIndex();
		MetalPipeline* GetPipelineByIndex(uint32_t idx);
		void DestroyPipelineByIndex(uint32_t idx);

		uint32_t GetNextMaterialDataIndex();
		MetalMaterialData* GetMaterialDataByIndex(uint32_t idx);
		void DestroyMaterialDataByIndex(uint32_t idx);

		uint32_t GetNextInstanceBufferIndex();
		MetalBuffer* GetInstanceBufferByIndex(uint32_t idx);
		void DestroyInstanceBufferByIndex(uint32_t idx);

		uint32_t GetNextDrawCommandIndex();
		MetalDrawCommand* GetDrawCommandByIndex(uint32_t idx);
		void DestroyDrawCommandByIndex(uint32_t idx);

		MTL::Buffer* CreateMetalBuffer(size_t size, MTL::ResourceOptions flags, const void* data = nullptr);

		uint32_t CreateMetalTexture(uint32_t width, uint32_t height, void* data = nullptr);
		uint32_t CreateMetalTexture(MTL::TextureDescriptor* desc, uint32_t width, uint32_t height, void* data = nullptr);
		uint32_t CreateMetalCubeMap(uint32_t width, const array<void*, 6>* datas = nullptr);
		uint32_t CreateMetalCubeMap(MTL::TextureDescriptor* desc, uint32_t width, const array<void*, 6>* datas = nullptr);

		void FillGPUTexture(MTL::Texture* texture, const void* data, uint32_t width, uint32_t height);
		void FillGPUCubeMap(MTL::Texture* texture, const array<void*, 6>* datas, uint32_t width);

		void CheckDeleteData();


		/// <summary>
		/// 其它辅助接口
		/// </summary>
	private:
		bool mWindowResized = false;
		bool mGameViewResized = false;
		uint32_t mNewWindowWidth = 0;
		uint32_t mNewWindowHeight = 0;

		uint32_t mCurFBOIdx = 0;
		uint32_t mCurPipeLineIdx = 0;
		uint32_t mCurMaterialDataIdx = 0;
		vector<MetalDrawRecord> mDrawRecords;

		ViewPortInfo mViewPortInfo;

		uint32_t GetCurFrameBufferIndex() const;

		void DoWindowSizeChange();
		void DoGameViewSizeChange();

		void* GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);
		vector<void*> GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);

		void ImmediatelyExecute(std::function<void(MTL::CommandBuffer* cmd)>&& function);
	};
}