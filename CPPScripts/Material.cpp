#include "Material.h"

namespace ZXEngine
{
	int Material::GetRenderQueue()
	{
		return shader->GetRenderQueue();
	}
}