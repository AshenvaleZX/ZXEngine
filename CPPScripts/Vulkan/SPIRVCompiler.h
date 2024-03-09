#pragma once
#include "../pubh.h"
#include <Vulkan/vulkan/vulkan.h>
#include <Vulkan/glslang/SPIRV/GlslangToSpv.h>

namespace ZXEngine
{
	class SPIRVCompiler
	{
		// 预编译
	public:
		static void CompileAllShader(string path);

	private:
		static void CompileShader(const filesystem::path& path);
		static void GenerateSPIRVFile(const filesystem::path& path);
		static void GenerateSPIRVFile(const filesystem::path& path, const string& code, ShaderStageFlagBit stage);


		// 运行时编译
		// 需要用到一堆GLSLang库的dll，但是这些dll的Debug版本太大了，工程里就没有添加，所以这些接口无法直接使用，需要手动把这些库链接进来才能用
		// 可以在这里找官方编译好的Windows库:https://github.com/KhronosGroup/glslang/releases
		/*
	public:
		static void RTInitialize();
		static void RTFinalize();
		static void RTInitResources(TBuiltInResource& Resources);
		static void RTCompileGLSL(EShLanguage stage, const char* glslCode, std::vector<uint32_t>& spirv);
		static void RTGetVkShaderModule(VkDevice device, EShLanguage stage, string path, VkShaderModule& shaderModule);
		//*/
	};
}