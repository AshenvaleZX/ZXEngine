#include "Resources.h"

namespace ZXEngine
{
	const char* Resources::assetsPath;
	void Resources::SetAssetsPath(const char* path)
	{
		assetsPath = path;
	}

	string Resources::GetAssetFullPath(const char* path)
	{
		std::stringstream ss;
		ss << assetsPath << path;
		return ss.str();
	}

	string Resources::JsonStrToString(json data)
	{
		string p = to_string(data);
		// 这个json字符串取出来前后会有双引号，需要去掉再用
		p = p.substr(1, p.length() - 2);
		return p;
	}

	json Resources::GetAssetData(const char* path)
	{
		string p = Resources::GetAssetFullPath(path);
		Debug::Log("Load asset: " + p);
		std::ifstream f(p);
		if (!f.is_open())
		{
			Debug::LogError("Load asset failed: " + p);
		}
		return json::parse(f);
	}

	SceneStruct* Resources::LoadScene(const char* path)
	{
		json data = Resources::GetAssetData(path);
		SceneStruct* scene = new SceneStruct;

		for (unsigned int i = 0; i < data["Cameras"].size(); i++)
		{
			CameraStruct* cam = new CameraStruct;
			cam->position = vec3(data["Cameras"][i]["Position"][0], data["Cameras"][i]["Position"][1], data["Cameras"][i]["Position"][2]);
			scene->cameras.push_back(cam);
		}

		for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
		{
			string p = Resources::JsonStrToString(data["GameObjects"][i]);
			PrefabStruct* prefab = Resources::LoadPrefab(p.c_str());
			scene->prefabs.push_back(prefab);
		}

		return scene;
	}

	PrefabStruct* Resources::LoadPrefab(const char* path)
	{
		json data = Resources::GetAssetData(path);
		PrefabStruct* prefab = new PrefabStruct;

		for (unsigned int i = 0; i < data["Components"].size(); i++)
		{
			json component = data["Components"][i];
			prefab->components.push_back(component);
		}

		return prefab;
	}

	MaterialStruct* Resources::LoadMaterial(const char* path)
	{
		json data = Resources::GetAssetData(path);
		MaterialStruct* matStruct = new MaterialStruct;

		string p = Resources::JsonStrToString(data["Shader"]);
		matStruct->shaderPath = Resources::GetAssetFullPath(p.c_str());

		for (unsigned int i = 0; i < data["Textures"].size(); i++)
		{
			json texture = data["Textures"][i];
			TextureStruct* textureStruct = new TextureStruct();
			textureStruct->path = Resources::GetAssetFullPath(Resources::JsonStrToString(texture["Path"]).c_str());
			textureStruct->uniformName = Resources::JsonStrToString(texture["UniformName"]);

			matStruct->textures.push_back(textureStruct);
		}

		return matStruct;
	}
}