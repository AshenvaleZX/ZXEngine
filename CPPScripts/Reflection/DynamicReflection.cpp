#include "DynamicReflection.h"

using namespace ZXEngine;

ZXENGINE_DYNAMIC_REFLECTION_REGISTRATION
{
	Reflection::Dynamic::Register<string>()
		.Register("string");

	Reflection::Dynamic::Register<Vector2>()
		.Register("Vector2")
		.AddVariable("x", &Vector2::x)
		.AddVariable("y", &Vector2::y)
		.AddFunction("Normalize", &Vector2::Normalize)
		.AddFunction("GetNormalized", &Vector2::GetNormalized);

	Reflection::Dynamic::Register<Vector3>()
		.Register("Vector3")
		.AddVariable("x", &Vector3::x)
		.AddVariable("y", &Vector3::y)
		.AddVariable("z", &Vector3::z)
		.AddFunction("Normalize", &Vector3::Normalize)
		.AddFunction("GetNormalized", &Vector3::GetNormalized);

	Reflection::Dynamic::Register<Vector4>()
		.Register("Vector4")
		.AddVariable("x", &Vector4::x)
		.AddVariable("y", &Vector4::y)
		.AddVariable("z", &Vector4::z)
		.AddVariable("w", &Vector4::w)
		.AddFunction("Normalize", &Vector4::Normalize)
		.AddFunction("GetNormalized", &Vector4::GetNormalized);

	Reflection::Dynamic::Register<Quaternion>()
		.Register("Quaternion")
		.AddVariable("x", &Quaternion::x)
		.AddVariable("y", &Quaternion::y)
		.AddVariable("z", &Quaternion::z)
		.AddVariable("w", &Quaternion::w)
		.AddFunction("Rotate", &Quaternion::Rotate)
		.AddFunction("Normalize", &Quaternion::Normalize)
		.AddFunction("GetInverse", &Quaternion::GetInverse)
		.AddFunction("GetEulerAngles", &Quaternion::GetEulerAngles);
}