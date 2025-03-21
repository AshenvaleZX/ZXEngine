#include "SPIRVCompiler.h"
#include "../Utils.h"
#include "../Resources.h"
#include "../ShaderParser.h"
#include "../ProjectSetting.h"

namespace ZXEngine
{
	void SPIRVCompiler::CompileAllShader(string path)
	{
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string extension = entry.path().filename().extension().string();
			if (extension == ".zxshader")
				CompileShader(entry.path());
			else if (extension == ".vkr")
				GenerateSPIRVFile(entry.path());
			else if (extension == ".glc")
				CompileCompute(entry.path());
			else if (entry.is_directory())
				CompileAllShader(entry.path().string());
		}
	}

	void SPIRVCompiler::CompileShader(const string& path)
	{
		filesystem::path p(path);
		if (filesystem::exists(p))
		{
			CompileShader(p);
		}
	}

	void SPIRVCompiler::CompileCompute(const string& path)
	{
		filesystem::path p(path);
		if (filesystem::exists(p))
		{
			CompileCompute(p);
		}
	}

	void SPIRVCompiler::GenerateSPIRVFile(const string& path)
	{
		filesystem::path p(path);
		if (filesystem::exists(p))
		{
			GenerateSPIRVFile(p);
		}
	}

	void SPIRVCompiler::CompileShader(const filesystem::path& path)
	{
		string shaderCode = Resources::LoadTextFile(path.string());
		string vertCode, geomCode, fragCode;
		ShaderParser::ParseShaderCode(shaderCode, vertCode, geomCode, fragCode);
		ShaderInfo info = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::Vulkan);

		vertCode = ShaderParser::TranslateToVulkan(vertCode, info.vertProperties);
		geomCode = ShaderParser::TranslateToVulkan(geomCode, info.geomProperties);
		fragCode = ShaderParser::TranslateToVulkan(fragCode, info.fragProperties);

		if (vertCode.empty())
			Debug::LogError("Empty vertex shader: " + path.string());
		else
			GenerateSPIRVFile(path, vertCode, ZX_SHADER_STAGE_VERTEX_BIT);

		if (fragCode.empty())
			Debug::LogError("Empty fragment shader: " + path.string());
		else
			GenerateSPIRVFile(path, fragCode, ZX_SHADER_STAGE_FRAGMENT_BIT);

		if (!geomCode.empty())
			GenerateSPIRVFile(path, geomCode, ZX_SHADER_STAGE_GEOMETRY_BIT);
	}

	void SPIRVCompiler::CompileCompute(const filesystem::path& path)
	{
		string code = Resources::LoadTextFile(path.string());
		GenerateSPIRVFile(path, code, ZX_SHADER_STAGE_COMPUTE_BIT);
	}

	void SPIRVCompiler::GenerateSPIRVFile(const filesystem::path& path)
	{
		string filePath = path.string();
		string outputFinalPath = path.parent_path().string() + "/" + path.stem().string() + ".spv";
		string stage = Utils::GetFileExtension(path.stem().string());

#if defined(ZX_PLATFORM_WINDOWS)
		string command = "..\\..\\..\\Tools\\glslangValidator.exe -V " + Utils::ConvertPathToWindowsFormat(filePath) +
			" --target-env spirv1.6 -S " + stage + " -o " + Utils::ConvertPathToWindowsFormat(outputFinalPath);
#elif defined(ZX_PLATFORM_MACOS)
		string command = "../../../Tools/glslang_macOS -V " + filePath +
			" --target-env spirv1.6 -S " + stage + " -o " + outputFinalPath;
#elif defined(ZX_PLATFORM_LINUX)
		string command = "../../../Tools/glslang_Linux_x86-64 -V " + filePath +
			" --target-env spirv1.6 -S " + stage + " -o " + outputFinalPath;
#endif
		int ret = std::system(command.c_str());
		if (ret != 0)
			Debug::LogError("Generate SPIR-V file failed: " + path.string());
	}

	void SPIRVCompiler::GenerateSPIRVFile(const filesystem::path& path, const string& code, ShaderStageFlagBit stage)
	{
		string fileName = path.stem().string();
		string folderPath = path.parent_path().string();
		string extension = "";
		if (stage & ZX_SHADER_STAGE_VERTEX_BIT)
			extension = "vert";
		else if (stage & ZX_SHADER_STAGE_GEOMETRY_BIT)
			extension = "geom";
		else if (stage & ZX_SHADER_STAGE_FRAGMENT_BIT)
			extension = "frag";
		else if (stage & ZX_SHADER_STAGE_RAYGEN_BIT)
			extension = "rgen";
		else if (stage & ZX_SHADER_STAGE_ANY_HIT_BIT)
			extension = "rahit";
		else if (stage & ZX_SHADER_STAGE_CLOSEST_HIT_BIT)
			extension = "rchit";
		else if (stage & ZX_SHADER_STAGE_MISS_BIT)
			extension = "rmiss";
		else if (stage & ZX_SHADER_STAGE_INTERSECTION_BIT)
			extension = "rint";
		else if (stage & ZX_SHADER_STAGE_COMPUTE_BIT)
			extension = "comp";
		else
			Debug::LogError("Unknown shader stage: " + path.string());

		string writeTempPath = folderPath + "/" + fileName + "." + extension;
		string outputFinalPath = writeTempPath + ".spv";

		ofstream writer;
		try
		{
			writer.open(writeTempPath);
			writer << code;
			writer.close();
		}
		catch (ofstream::failure e)
		{
			Debug::LogError("Generate SPIR-V file failed: " + path.string());
		}

		// 用Vulkan工具生成SPIR-V文件
#if defined(ZX_PLATFORM_WINDOWS)
		string command = "..\\..\\..\\Tools\\glslangValidator.exe -V " + Utils::ConvertPathToWindowsFormat(writeTempPath) + " -o " + Utils::ConvertPathToWindowsFormat(outputFinalPath);
#elif defined(ZX_PLATFORM_MACOS)
		string command = "../../../Tools/glslang_macOS -V " + writeTempPath + " -o " + outputFinalPath;
#elif defined(ZX_PLATFORM_LINUX)
		string command = "../../../Tools/glslang_Linux_x86-64 -V " + writeTempPath + " -o " + outputFinalPath;
#endif
		int ret = std::system(command.c_str());
		if (ret != 0)
			Debug::LogError("Generate SPIR-V file failed: " + path.string());

		// 删除临时文件
		if (!ProjectSetting::preserveIntermediateShader)
			if (!(std::remove(writeTempPath.c_str()) == 0))
				Debug::LogError("Remove temporary SPIR-V file failed: " + path.string());
	}

	/*
	void SPIRVCompiler::RTInitialize()
	{
		glslang::InitializeProcess();
	}

	void SPIRVCompiler::RTFinalize()
	{
		glslang::FinalizeProcess();
	}

	void SPIRVCompiler::RTInitResources(TBuiltInResource& Resources)
	{
		Resources.maxLights = 32;
		Resources.maxClipPlanes = 6;
		Resources.maxTextureUnits = 32;
		Resources.maxTextureCoords = 32;
		Resources.maxVertexAttribs = 64;
		Resources.maxVertexUniformComponents = 4096;
		Resources.maxVaryingFloats = 64;
		Resources.maxVertexTextureImageUnits = 32;
		Resources.maxCombinedTextureImageUnits = 80;
		Resources.maxTextureImageUnits = 32;
		Resources.maxFragmentUniformComponents = 4096;
		Resources.maxDrawBuffers = 32;
		Resources.maxVertexUniformVectors = 128;
		Resources.maxVaryingVectors = 8;
		Resources.maxFragmentUniformVectors = 16;
		Resources.maxVertexOutputVectors = 16;
		Resources.maxFragmentInputVectors = 15;
		Resources.minProgramTexelOffset = -8;
		Resources.maxProgramTexelOffset = 7;
		Resources.maxClipDistances = 8;
		Resources.maxComputeWorkGroupCountX = 65535;
		Resources.maxComputeWorkGroupCountY = 65535;
		Resources.maxComputeWorkGroupCountZ = 65535;
		Resources.maxComputeWorkGroupSizeX = 1024;
		Resources.maxComputeWorkGroupSizeY = 1024;
		Resources.maxComputeWorkGroupSizeZ = 64;
		Resources.maxComputeUniformComponents = 1024;
		Resources.maxComputeTextureImageUnits = 16;
		Resources.maxComputeImageUniforms = 8;
		Resources.maxComputeAtomicCounters = 8;
		Resources.maxComputeAtomicCounterBuffers = 1;
		Resources.maxVaryingComponents = 60;
		Resources.maxVertexOutputComponents = 64;
		Resources.maxGeometryInputComponents = 64;
		Resources.maxGeometryOutputComponents = 128;
		Resources.maxFragmentInputComponents = 128;
		Resources.maxImageUnits = 8;
		Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		Resources.maxCombinedShaderOutputResources = 8;
		Resources.maxImageSamples = 0;
		Resources.maxVertexImageUniforms = 0;
		Resources.maxTessControlImageUniforms = 0;
		Resources.maxTessEvaluationImageUniforms = 0;
		Resources.maxGeometryImageUniforms = 0;
		Resources.maxFragmentImageUniforms = 8;
		Resources.maxCombinedImageUniforms = 8;
		Resources.maxGeometryTextureImageUnits = 16;
		Resources.maxGeometryOutputVertices = 256;
		Resources.maxGeometryTotalOutputComponents = 1024;
		Resources.maxGeometryUniformComponents = 1024;
		Resources.maxGeometryVaryingComponents = 64;
		Resources.maxTessControlInputComponents = 128;
		Resources.maxTessControlOutputComponents = 128;
		Resources.maxTessControlTextureImageUnits = 16;
		Resources.maxTessControlUniformComponents = 1024;
		Resources.maxTessControlTotalOutputComponents = 4096;
		Resources.maxTessEvaluationInputComponents = 128;
		Resources.maxTessEvaluationOutputComponents = 128;
		Resources.maxTessEvaluationTextureImageUnits = 16;
		Resources.maxTessEvaluationUniformComponents = 1024;
		Resources.maxTessPatchComponents = 120;
		Resources.maxPatchVertices = 32;
		Resources.maxTessGenLevel = 64;
		Resources.maxViewports = 16;
		Resources.maxVertexAtomicCounters = 0;
		Resources.maxTessControlAtomicCounters = 0;
		Resources.maxTessEvaluationAtomicCounters = 0;
		Resources.maxGeometryAtomicCounters = 0;
		Resources.maxFragmentAtomicCounters = 8;
		Resources.maxCombinedAtomicCounters = 8;
		Resources.maxAtomicCounterBindings = 1;
		Resources.maxVertexAtomicCounterBuffers = 0;
		Resources.maxTessControlAtomicCounterBuffers = 0;
		Resources.maxTessEvaluationAtomicCounterBuffers = 0;
		Resources.maxGeometryAtomicCounterBuffers = 0;
		Resources.maxFragmentAtomicCounterBuffers = 1;
		Resources.maxCombinedAtomicCounterBuffers = 1;
		Resources.maxAtomicCounterBufferSize = 16384;
		Resources.maxTransformFeedbackBuffers = 4;
		Resources.maxTransformFeedbackInterleavedComponents = 64;
		Resources.maxCullDistances = 8;
		Resources.maxCombinedClipAndCullDistances = 8;
		Resources.maxSamples = 4;
		Resources.maxMeshOutputVerticesNV = 256;
		Resources.maxMeshOutputPrimitivesNV = 512;
		Resources.maxMeshWorkGroupSizeX_NV = 32;
		Resources.maxMeshWorkGroupSizeY_NV = 1;
		Resources.maxMeshWorkGroupSizeZ_NV = 1;
		Resources.maxTaskWorkGroupSizeX_NV = 32;
		Resources.maxTaskWorkGroupSizeY_NV = 1;
		Resources.maxTaskWorkGroupSizeZ_NV = 1;
		Resources.maxMeshViewCountNV = 4;
		Resources.limits.nonInductiveForLoops = 1;
		Resources.limits.whileLoops = 1;
		Resources.limits.doWhileLoops = 1;
		Resources.limits.generalUniformIndexing = 1;
		Resources.limits.generalAttributeMatrixVectorIndexing = 1;
		Resources.limits.generalVaryingIndexing = 1;
		Resources.limits.generalSamplerIndexing = 1;
		Resources.limits.generalVariableIndexing = 1;
		Resources.limits.generalConstantMatrixVectorIndexing = 1;
	}

	void SPIRVCompiler::RTCompileGLSL(EShLanguage stage, const char* glslCode, vector<uint32_t>& spirv)
	{
		glslang::TShader shader(stage);
		glslang::TProgram program;
		const char* shaderStrings[1]{};
		TBuiltInResource Resources = {};
		RTInitResources(Resources);

		// Enable SPIR-V and Vulkan rules when parsing GLSL
		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		shaderStrings[0] = glslCode;
		shader.setStrings(shaderStrings, 1);

		if (!shader.parse(&Resources, 100, false, messages))
		{
			puts(shader.getInfoLog());
			puts(shader.getInfoDebugLog());
			return;
		}

		program.addShader(&shader);

		if (!program.link(messages))
		{
			puts(shader.getInfoLog());
			puts(shader.getInfoDebugLog());
			fflush(stdout);
			return;
		}

		glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
	}

	void SPIRVCompiler::RTGetVkShaderModule(VkDevice device, EShLanguage stage, string path, VkShaderModule& shaderModule)
	{
		string shaderCode;
		ifstream shaderFile;

		shaderFile.open(path);
		stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();

		vector<uint32_t> spirv;
		RTCompileGLSL(stage, shaderCode.c_str(), spirv);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		// 这里的codeSize是以bytes计算的，spirv.size()是uint32_t的数量，一个32位int占4字节
		createInfo.codeSize = spirv.size() * 4;
		createInfo.pCode = spirv.data();

		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module!");
	}
	//*/
}