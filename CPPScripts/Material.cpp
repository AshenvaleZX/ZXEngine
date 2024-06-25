#include "Material.h"
#include "ZShader.h"
#include "Texture.h"
#include "RenderAPI.h"
#include "GlobalData.h"
#include "MaterialData.h"
#include "RenderEngineProperties.h"
#include "Time.h"

namespace ZXEngine
{
	Material::Material(MaterialStruct* matStruct)
	{
		name = matStruct->name;
		path = matStruct->path;
		isShareShader = false;
		type = matStruct->type;
		data = new MaterialData(type);

		CopyMaterialStructToMaterialData(matStruct, data);

		if (type == MaterialType::Forward)
		{
			shader = new Shader(matStruct->shaderPath, matStruct->shaderCode, FrameBufferType::Normal);
			renderQueue = (int)shader->reference->shaderInfo.stateSet.renderQueue;
			renderQueueType = shader->reference->shaderInfo.stateSet.renderQueue;
			RenderAPI::GetInstance()->SetUpMaterial(this);
		}
		else if (type == MaterialType::Deferred)
		{
			renderQueue = (int)RenderQueueType::Deferred;
			renderQueueType = RenderQueueType::Deferred;
		}
		else if (type == MaterialType::RayTracing)
		{
			hitGroupIdx = matStruct->hitGroupIdx;
			// 光追管线暂时不需要区分队列，用一个默认的Opaque队列
			renderQueue = (int)RenderQueueType::Opaque;
			renderQueueType = RenderQueueType::Opaque;
			RenderAPI::GetInstance()->SetUpRayTracingMaterialData(this);
		}
	}

	Material::Material(Shader* shader)
	{
		// 这里要注意一个问题，如果调用这个构造函数时，是直接把new Shader写在参数里的，如:
		// new Material(new Shader(...))
		// 那么会有一个问题，new的时候没有保存引用，最终会少一个delete，导致referenceCount至少大于1，这个shader永远不会被正真销毁
		isShareShader = true;
		shader->reference->referenceCount++;
		this->shader = shader;
		renderQueue = (int)shader->reference->shaderInfo.stateSet.renderQueue;
		// 通过这个构造函数初始化的材质，默认为光栅化渲染管线的材质
		type = MaterialType::Forward;
		data = new MaterialData(type);
		RenderAPI::GetInstance()->SetUpMaterial(this);
	}

	Material::~Material()
	{
		delete data;

		if (shader)
		{
			if (isShareShader)
				shader->reference->referenceCount--;
			else
				delete shader;
		}
	}

	void Material::Use()
	{
		data->Use();
		shader->Use();
	}

	int Material::GetRenderQueue() const
	{
		return renderQueue;
	}

	RenderQueueType Material::GetRenderQueueType() const
	{
		return renderQueueType;
	}

	void Material::SetEngineProperties()
	{
		for (auto& property : shader->reference->shaderInfo.vertProperties.baseProperties)
			SetEngineProperty(property.name, property.type);
		for (auto& property : shader->reference->shaderInfo.vertProperties.textureProperties)
			SetEngineProperty(property.name, property.type);
		for (auto& property : shader->reference->shaderInfo.fragProperties.baseProperties)
			SetEngineProperty(property.name, property.type);
		for (auto& property : shader->reference->shaderInfo.fragProperties.textureProperties)
			SetEngineProperty(property.name, property.type);
	}

	void Material::SetEngineProperty(const string& name, ShaderPropertyType type)
	{
		auto engineProperties = RenderEngineProperties::GetInstance();

		if (type == ShaderPropertyType::ENGINE_MODEL)
			SetMatrix(name, engineProperties->matM);
		else if (type == ShaderPropertyType::ENGINE_VIEW)
			SetMatrix(name, engineProperties->matV);
		else if (type == ShaderPropertyType::ENGINE_PROJECTION)
			SetMatrix(name, engineProperties->matP);
		else if (type == ShaderPropertyType::ENGINE_CAMERA_POS)
			SetVector(name, engineProperties->camPos);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_MAT)
			SetMatrix(name, engineProperties->lightMat);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_POS)
			SetVector(name, engineProperties->lightPos);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_DIR)
			SetVector(name, engineProperties->lightDir);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_COLOR)
			SetVector(name, engineProperties->lightColor);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_INTENSITY)
			SetScalar(name, engineProperties->lightIntensity);
		else if (type == ShaderPropertyType::ENGINE_FAR_PLANE)
			SetScalar(name, GlobalData::shadowCubeMapFarPlane);
		else if (type == ShaderPropertyType::ENGINE_MODEL_INV)
			SetMatrix(name, engineProperties->matM_Inv);
		else if (type == ShaderPropertyType::ENGINE_TIME)
			SetVector(name, Vector2(Time::curTime, Time::deltaTime));
		else if (type == ShaderPropertyType::ENGINE_SHADOW_MAP)
		{
			SetTexture(name, engineProperties->shadowMap, textureIdx, false, engineProperties->isShadowMapBuffer);
			textureIdx++;
		}
		else if (type == ShaderPropertyType::ENGINE_SHADOW_CUBE_MAP)
		{
			// 先设置SetMaterialProperties获得引擎纹理的初始textureIdx，然后++
			SetCubeMap(name, engineProperties->shadowCubeMap, textureIdx, false, engineProperties->isShadowCubeMapBuffer);
			textureIdx++;
		}
	}

	void Material::SetMaterialProperties()
	{
		if (data->isDirty)
		{
			for (auto& iter : data->floatDatas)
				SetScalar(iter.first, iter.second);
			for (auto& iter : data->uintDatas)
				SetScalar(iter.first, iter.second);
			for (auto& iter : data->vec2Datas)
				SetVector(iter.first, iter.second);
			for (auto& iter : data->vec3Datas)
				SetVector(iter.first, iter.second);
			for (auto& iter : data->vec4Datas)
				SetVector(iter.first, iter.second);
			for (auto& iter : data->colorDatas)
				SetVector(iter.first, iter.second);

			data->isDirty = false;
		}

		uint32_t textureNum = static_cast<uint32_t>(data->textures.size());

		for (uint32_t i = 0; i < textureNum; i++)
		{
			if (data->textures[i].second->type == TextureType::ZX_2D)
				SetTexture(data->textures[i].first, data->textures[i].second->GetID(), i);
			else if (data->textures[i].second->type == TextureType::ZX_Cube)
				SetCubeMap(data->textures[i].first, data->textures[i].second->GetID(), i);
		}

		textureIdx = textureNum;
	}

	void Material::SetMaterialProperty(const string& name, ShaderPropertyType type)
	{

	}

	void Material::SetScalar(const string& name, bool value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetScalar(const string& name, float value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetScalar(const string& name, int32_t value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetScalar(const string& name, uint32_t value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector2& value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector2& value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, idx, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector3& value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector3& value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, idx, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector4& value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector4& value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, idx, allBuffer);
	}
	void Material::SetVector(const string& name, const Vector4* value, uint32_t count, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, count, allBuffer);
	}
	void Material::SetMatrix(const string& name, const Matrix3& value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, allBuffer);
	}
	void Material::SetMatrix(const string& name, const Matrix3& value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, idx, allBuffer);
	}
	void Material::SetMatrix(const string& name, const Matrix4& value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, allBuffer);
	}
	void Material::SetMatrix(const string& name, const Matrix4& value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, idx, allBuffer);
	}
	void Material::SetMatrix(const string& name, const Matrix4* value, uint32_t count, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, count, allBuffer);
	}
	void Material::SetTexture(const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		RenderAPI::GetInstance()->SetShaderTexture(this, name, ID, idx, allBuffer, isBuffer);
	}
	void Material::SetCubeMap(const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		RenderAPI::GetInstance()->SetShaderCubeMap(this, name, ID, idx, allBuffer, isBuffer);
	}

	void Material::CopyMaterialStructToMaterialData(MaterialStruct* matStruct, MaterialData* data)
	{
		data->floatDatas = matStruct->floatDatas;
		data->uintDatas  = matStruct->uintDatas;
		data->vec2Datas  = matStruct->vec2Datas;
		data->vec3Datas  = matStruct->vec3Datas;
		data->vec4Datas  = matStruct->vec4Datas;
		data->colorDatas = matStruct->colorDatas;

		for (auto textureStruct : matStruct->textures)
		{
			Texture* texture = new Texture(textureStruct->data);
			data->textures.push_back(make_pair(textureStruct->uniformName, texture));
		}

		for (auto cubeMapStruct : matStruct->cubeMaps)
		{
			Texture* texture = new Texture(cubeMapStruct->data);
			data->textures.push_back(make_pair(cubeMapStruct->uniformName, texture));
		}
	}
}