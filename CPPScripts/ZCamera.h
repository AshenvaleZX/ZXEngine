#pragma once
#include "GameObject.h"

namespace ZXEngine
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void Render(list<GameObject*> gameObjects);

	private:

	};
}