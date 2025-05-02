#pragma once
#include "pubh.h"
#include "PublicStruct.h"
#include "FrameBufferObject.h"

namespace ZXEngine
{
	class Material;
	class MaterialData;
	class RenderStateSetting;
	class RenderAPI
	{
	public:
		static void Creat();
		static RenderAPI* GetInstance();
	private:
		static RenderAPI* mInstance;

		/// <summary>
		/// 标准渲染管线接口
		/// Standard Rendering Pipeline Interface
		/// </summary>
	public:
		RenderAPI() {};
		~RenderAPI() {};

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		// 渲染状态
		virtual void OnWindowSizeChange(uint32_t width, uint32_t height) = 0;
		virtual void OnGameViewSizeChange() = 0;
		virtual void SetRenderState(RenderStateSetting* state) = 0;
		virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0) = 0;
		virtual void WaitForRenderFinish() = 0;

		// Frame Buffer
		virtual void SwitchFrameBuffer(uint32_t id) = 0;
		virtual void ClearFrameBuffer(FrameBufferClearFlags clearFlags) = 0;
		virtual void BlitFrameBuffer(uint32_t cmd, const string& src, const string& dst, FrameBufferPieceFlags flags) = 0;
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0) = 0;
		virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0) = 0;
		virtual void DeleteFrameBufferObject(FrameBufferObject* FBO) = 0;
		virtual uint32_t GetRenderBufferTexture(uint32_t id) = 0;

		// Instance Buffer
		virtual uint32_t CreateStaticInstanceBuffer(uint32_t size, uint32_t num, const void* data) = 0;
		virtual uint32_t CreateDynamicInstanceBuffer(uint32_t size, uint32_t num) = 0;
		virtual void UpdateDynamicInstanceBuffer(uint32_t id, uint32_t size, uint32_t num, const void* data) = 0;
		virtual void SetUpInstanceBufferAttribute(uint32_t VAO, uint32_t instanceBuffer, uint32_t size, uint32_t offset = 6) = 0;
		virtual void DeleteInstanceBuffer(uint32_t id) = 0;

		// 纹理
		virtual unsigned int LoadTexture(const char* path, int& width, int& height) = 0;
		virtual unsigned int LoadCubeMap(const vector<string>& faces) = 0;
		virtual unsigned int CreateTexture(TextureFullData* data) = 0;
		virtual unsigned int CreateCubeMap(CubeMapFullData* data) = 0;
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data) = 0;
		virtual void DeleteTexture(unsigned int id) = 0;

		// Shader
		virtual ShaderReference* LoadAndSetUpShader(const string& path, FrameBufferType type) = 0;
		virtual ShaderReference* SetUpShader(const string& path, const string& shaderCode, FrameBufferType type) = 0;
		virtual void DeleteShader(uint32_t id) = 0;

		// 材质
		virtual uint32_t CreateMaterialData() = 0;
		virtual void SetUpMaterial(Material* material) = 0;
		virtual void UseMaterialData(uint32_t ID) = 0;
		virtual void DeleteMaterialData(uint32_t id) = 0;

		// Draw
		virtual uint32_t AllocateDrawCommand(CommandType commandType, FrameBufferClearFlags clearFlags) = 0;
		virtual void FreeDrawCommand(uint32_t commandID) = 0;
		virtual void Draw(uint32_t VAO) = 0;
		virtual void DrawInstanced(uint32_t VAO, uint32_t instanceNum, uint32_t instanceBuffer) = 0;
		virtual void GenerateDrawCommand(uint32_t id) = 0;

		// Mesh
		virtual void DeleteMesh(unsigned int VAO) = 0;
		virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool skinned = false) = 0;
		virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize) = 0;
		virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices) = 0;
		virtual void GenerateParticleMesh(unsigned int& VAO) = 0;

		// Shader参数
		virtual void UseShader(unsigned int ID) = 0;
		virtual void SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer = false) = 0;
		virtual void SetShaderScalar(Material* material, const string& name, float value, bool allBuffer = false) = 0;
		virtual void SetShaderScalar(Material* material, const string& name, int32_t value, bool allBuffer = false) = 0;
		virtual void SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false) = 0;
		virtual void SetShaderVector(Material* material, const string& name, const Vector4* value, uint32_t count, bool allBuffer = false) = 0;
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer = false) = 0;
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false) = 0;
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer = false) = 0;
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false) = 0;
		virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4* value, uint32_t count, bool allBuffer = false) = 0;
		virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false) = 0;
		virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false) = 0;


		/// <summary>
		/// 通用计算管线接口
		/// Compute Pipeline Interface
		/// </summary>
	public:
		// Shader Storage Buffer
		virtual uint32_t CreateShaderStorageBuffer(const void* data, size_t size, GPUBufferType type) = 0;
		virtual void BindShaderStorageBuffer(uint32_t id, uint32_t binding) = 0;
		virtual void UpdateShaderStorageBuffer(uint32_t id, const void* data, size_t size) = 0;
		virtual void DeleteShaderStorageBuffer(uint32_t id) = 0;

		// Vertex Buffer Binding
		virtual void BindVertexBuffer(uint32_t VAO, uint32_t binding) = 0;

		// Compute Shader
		virtual ComputeShaderReference* LoadAndSetUpComputeShader(const string& path) = 0;
		virtual void DeleteComputeShader(uint32_t id) = 0;

		// Compute Command
		virtual void Dispatch(uint32_t commandID, uint32_t shaderID, uint32_t groupX, uint32_t groupY, uint32_t groupZ, uint32_t xSizeInGroup, uint32_t ySizeInGroup, uint32_t zSizeInGroup) = 0;
		virtual void SubmitAllComputeCommands() = 0;

		/// <summary>
		/// 光线追踪管线接口
		/// Ray Tracing Pipeline Interface
		/// </summary>
	public:
		// Pipeline
		virtual uint32_t CreateRayTracingPipeline(const RayTracingShaderPathGroup& rtShaderPathGroup) = 0;
		virtual void SwitchRayTracingPipeline(uint32_t rtPipelineID) = 0;

		// Material
		virtual uint32_t CreateRayTracingMaterialData() = 0;
		virtual void SetUpRayTracingMaterialData(Material* material) = 0;
		virtual void DeleteRayTracingMaterialData(uint32_t id) = 0;

		// 数据更新
		virtual void SetRayTracingSkyBox(uint32_t textureID) = 0;
		virtual void PushRayTracingMaterialData(Material* material) = 0;
		virtual void PushAccelerationStructure(uint32_t VAO, uint32_t hitGroupIdx, uint32_t rtMaterialDataID, const Matrix4& transform) = 0;

		// Ray Trace
		virtual void RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants) = 0;

		// Acceleration Structure
		virtual void BuildTopLevelAccelerationStructure(uint32_t commandID) = 0;
		virtual void BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact) = 0;
	};
}