#pragma once
#include "../pubh.h"
#include <Vulkan/vulkan/vulkan.h>
#include <Vulkan/glslang/SPIRV/GlslangToSpv.h>

namespace ZXEngine
{
	class SPIRVCompiler
	{
		// Ԥ����
	public:
		static void CompileAllShader(string path);

	private:
		static void CompileShader(const filesystem::path& path);
		static void GenerateSPIRVFile(const filesystem::path& path);
		static void GenerateSPIRVFile(const filesystem::path& path, const string& code, ShaderStageFlagBit stage);


		// ����ʱ����
		// ��Ҫ�õ�һ��GLSLang���dll��������Щdll��Debug�汾̫���ˣ��������û����ӣ�������Щ�ӿ��޷�ֱ��ʹ�ã���Ҫ�ֶ�����Щ�����ӽ���������
		// �����������ҹٷ�����õ�Windows��:https://github.com/KhronosGroup/glslang/releases
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