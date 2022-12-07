#include "Resources.h"

namespace ZXEngine
{
	string Resources::assetsPath;
	void Resources::SetAssetsPath(string path)
	{
		assetsPath = path;
	}
	
	string Resources::GetAssetsPath()
	{
		return assetsPath;
	}

	string Resources::GetAssetFullPath(string path)
	{
		return assetsPath + (string)path;
	}

	string Resources::JsonStrToString(json data)
	{
		string p = to_string(data);
		// ���json�ַ���ȡ����ǰ�����˫���ţ���Ҫȥ������
		p = p.substr(1, p.length() - 2);
		return p;
	}

	json Resources::LoadJson(string path)
	{
		std::ifstream f(path);
		if (!f.is_open())
		{
			Debug::LogError("Load asset failed: " + path);
		}
		json data;
		try
		{
			data = json::parse(f);
		}
		catch (json::exception& e)
		{
			Debug::LogError("Asset format error: " + path);
			string msg = e.what();
			Debug::LogError("Error detail: " + msg);
		}
		return data;
	}

	json Resources::GetAssetData(string path)
	{
		string p = Resources::GetAssetFullPath(path);
		Debug::Log("Load asset: " + p);
		return Resources::LoadJson(p);
	}

	SceneStruct* Resources::LoadScene(string path)
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

	PrefabStruct* Resources::LoadPrefab(string path)
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

	MaterialStruct* Resources::LoadMaterial(string path)
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
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"])) + Resources::JsonStrToString(data["Right"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"])) + Resources::JsonStrToString(data["Left"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"])) + Resources::JsonStrToString(data["Up"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"])) + Resources::JsonStrToString(data["Down"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"])) + Resources::JsonStrToString(data["Front"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"])) + Resources::JsonStrToString(data["Back"]));
		return cube;
	}
}