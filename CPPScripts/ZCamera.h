#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "RenderEngine.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	class Camera
	{
	public:
        // Camera Attributes
        vec3 Position;
        vec3 Front;
        vec3 Up;
        vec3 Right;
        vec3 WorldUp;
        // Euler Angles
        float Yaw;
        float Pitch;
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        Camera(vec3 position, vec3 up, float yaw, float pitch);
		~Camera() {};

        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();
		void Render(list<GameObject*> gameObjects);

	private:
        void UpdateCameraVectors();

	};
}