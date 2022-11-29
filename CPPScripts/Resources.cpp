#include "Resources.h"

namespace ZXEngine
{
	const char* Resources::assetsPath;
	void Resources::SetAssetsPath(const char* path)
	{
		assetsPath = path;
	}
	
	const char* Resources::GetAssetsPath()
	{
		return assetsPath;
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
		json data;
		try
		{
			data = json::parse(f);
		}
		catch (json::exception& e)
		{
			Debug::LogError("Asset format error: " + p);
			string msg = e.what();
			Debug::LogError("Error detail: " + msg);
		}
		return data;
	}

	SceneStruct* Resources::LoadScene(const char* path)
	{
		json data = Resources::GetAssetData(path);
		SceneStruct* scene = new SceneStruct;

		scene->skyBox = Resources::LoadCubeMap(data["SkyBox"]);

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

		if (data["Layer"].is_null())
			prefab->layer = GameObjectLayer::Default;
		else
			prefab->layer = data["Layer"];

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

	vector<string> Resources::LoadCubeMap(json data)
	{
		vector<string> cube;
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]).c_str()) + Resources::JsonStrToString(data["Right"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]).c_str()) + Resources::JsonStrToString(data["Left"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]).c_str()) + Resources::JsonStrToString(data["Up"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]).c_str()) + Resources::JsonStrToString(data["Down"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]).c_str()) + Resources::JsonStrToString(data["Front"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]).c_str()) + Resources::JsonStrToString(data["Back"]));
		return cube;
	}
}