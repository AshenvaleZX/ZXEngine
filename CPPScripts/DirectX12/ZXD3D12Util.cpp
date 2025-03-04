#include "ZXD3D12Util.h"
#include "../Utils.h"
#include "../Resources.h"
#include "../ShaderParser.h"
#include "../ProjectSetting.h"

namespace ZXEngine
{
	void ZXD3D12Util::CompileAllShader(string path)
	{
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string extension = entry.path().filename().extension().string();
			if (extension == ".zxshader")
				CompileShader(entry.path().string());
			else if (entry.is_directory())
				CompileAllShader(entry.path().string());
		}
	}

	void ZXD3D12Util::CompileShader(const filesystem::path& path)
	{
		ShaderStageFlags stageFlags = 0;
		string hlslPath = TranslateShaderToHLSL(path, stageFlags);
		string outputPath = hlslPath.substr(0, hlslPath.length() - 5);

		// 用微软的工具预编译hlsl代码
		// VS
		string command = "..\\..\\..\\Tools\\fxc.exe /T vs_5_1 /E VS /Fo " + Utils::ConvertPathToWindowsFormat(outputPath + ".vert.fxc") + " " + Utils::ConvertPathToWindowsFormat(hlslPath);
		int ret = std::system(command.c_str());
		if (ret != 0)
			Debug::LogError("Compile vertex shader failed: " + path.string());

		// PS
		command = "..\\..\\..\\Tools\\fxc.exe /T ps_5_1 /E PS /Fo " + Utils::ConvertPathToWindowsFormat(outputPath + ".frag.fxc") + " " + Utils::ConvertPathToWindowsFormat(hlslPath);
		ret = std::system(command.c_str());
		if (ret != 0)
			Debug::LogError("Compile fragment shader failed: " + path.string());

		// GS
		if (stageFlags & ZX_SHADER_STAGE_GEOMETRY_BIT)
		{
			command = "..\\..\\..\\Tools\\fxc.exe /T gs_5_1 /E GS /Fo " + Utils::ConvertPathToWindowsFormat(outputPath + ".geom.fxc") + " " + Utils::ConvertPathToWindowsFormat(hlslPath);
			ret = std::system(command.c_str());
			if (ret != 0)
				Debug::LogError("Compile geometry shader failed: " + path.string());
		}

		// 删除临时文件
		if (!ProjectSetting::preserveIntermediateShader)
			if (!(std::remove(hlslPath.c_str()) == 0))
				Debug::LogError("Remove temporary HLSL file failed: " + path.string());
	}

	void ZXD3D12Util::TranslateAllShaderToHLSL(const string& path)
	{
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string extension = entry.path().filename().extension().string();
			if (extension == ".zxshader")
				TranslateShaderToHLSL(entry.path().string());
			else if (entry.is_directory())
				TranslateAllShaderToHLSL(entry.path().string());
		}
	}

	void ZXD3D12Util::TranslateShaderToHLSL(const filesystem::path& path)
	{
		string shaderCode = Resources::LoadTextFile(path.string());
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::D3D12);
		string hlslCode = ShaderParser::TranslateToD3D12(shaderCode, shaderInfo);

		string fileName = path.stem().string();
		string folderPath = path.parent_path().string();
		string writePath = folderPath + "/" + fileName + ".hlsl";

		ofstream writer;
		try
		{
			writer.open(writePath);
			writer << hlslCode;
			writer.close();
		}
		catch (ofstream::failure e)
		{
			Debug::LogError("Generate HLSL file failed: " + path.string());
		}
	}

	string ZXD3D12Util::TranslateShaderToHLSL(const filesystem::path& path, ShaderStageFlags& stageFlags)
	{
		string shaderCode = Resources::LoadTextFile(path.string());
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::D3D12);
		string hlslCode = ShaderParser::TranslateToD3D12(shaderCode, shaderInfo);

		string fileName = path.stem().string();
		string folderPath = path.parent_path().string();
		string writePath = folderPath + "/" + fileName + ".hlsl";

		ofstream writer;
		try
		{
			writer.open(writePath);
			writer << hlslCode;
			writer.close();
		}
		catch (ofstream::failure e)
		{
			Debug::LogError("Generate HLSL file failed: " + path.string());
		}

		stageFlags = shaderInfo.stages;
		return writePath;
	}
}