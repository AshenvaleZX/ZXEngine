#pragma once
#include "../pubh.h"
#include "../Util/Serialization.h"
#include "../Reflection/DynamicReflection.h"
#include "../Reflection/Variable.h"

namespace ZXEngine
{
	enum class MyEnum
	{
		One,
		Two,
		Three
	};

	class Human
	{
	public:
		uint32_t mAge = 0;
		float mHeight = 0.0f;
	};

	ZXRef_StaticReflection
	(
		Human,
		ZXRef_Fields
		(
			ZXRef_Field(&Human::mAge),
			ZXRef_Field(&Human::mHeight)
		)
	)

	class Student : public Human
	{
	public:
		string mName;
		Vector3 mPos;

		int GetAge() const
		{
			return mAge;
		}

		void Speak(const string& words)
		{
			Debug::Log("Student speak: %s", words);
		}
	};

	ZXRef_StaticReflection
	(
		Student,
		ZXRef_BaseType(Human)
		ZXRef_Fields
		(
			ZXRef_Field(&Student::mName),
			ZXRef_Field(&Student::mPos)
		)
	)

	void RunReflectionAndSerializationCase()
	{
		// Static reflection and serialization test
		Student student;
		student.mAge = 18;
		student.mName = "Tom";
		student.mHeight = 185.3f;
		student.mPos = { 1, 2, 3 };

		string res = Serialization::Serialize(student);
		Debug::Log("Serialize student:\n%s", res);

		Student student2;
		Serialization::Deserialize(student2, res);

		Student student3 = Serialization::Deserialize<Student>(res);

		// Dynamic reflection test for enum
		Reflection::Dynamic::Register<MyEnum>()
			.Register("MyEnum")
			.AddEnum("One", MyEnum::One)
			.AddEnum("Two", MyEnum::Two)
			.AddEnum("Three", MyEnum::Three);

		auto typeInfo = Reflection::Dynamic::GetTypeInfo("MyEnum");
		auto enumInfo = typeInfo->AsEnum();

		Debug::Log("Type name: %s", typeInfo->GetName());
		for (auto& item : enumInfo->GetItems())
		{
			Debug::Log("Enum item: %s value %s", item.name, item.value);
		}

		// Dynamic reflection test for class
		Reflection::Dynamic::Register<Student>()
			.Register("Student")
			.AddVariable("mName", &Student::mName)
			.AddVariable("mAge", &Student::mAge)
			.AddVariable("mHeight", &Student::mHeight)
			.AddVariable("mPos", &Student::mPos)
			.AddFunction("GetAge", &Student::GetAge)
			.AddFunction("Speak", &Student::Speak);

		auto sTypeInfo = Reflection::Dynamic::GetTypeInfo("Student")->AsClass();

		Debug::Log("Type name: %s", sTypeInfo->GetName());
		for (auto& item : sTypeInfo->GetVariables())
		{
			Debug::Log("Variable: %s %s", item->type->GetName(), item->name);
		}
		for (auto& item : sTypeInfo->GetFunctions())
		{
			Debug::Log("Function: %s return %s", item->name, item->returnType->GetName());
			for (auto& param : item->paramTypes)
			{
				Debug::Log("Param: %s", param->GetName());
			}
		}

		Debug::Log("Origin mAge %s", student.mAge);
		sTypeInfo->SetVariable(&student, "mAge", 12);
		Debug::Log("New mAge %s", student.mAge);

		Debug::Log("Origin mPos %s", student.mPos);
		Vector3 nPos = { 6, 0, 3 };
		sTypeInfo->SetVariable(&student, "mPos", nPos);
		Debug::Log("New mPos %s", student.mPos);

		auto age = sTypeInfo->GetVariable(&student, "mAge");
		Debug::Log("Get value1 %s", std::any_cast<uint32_t>(age));
		auto pos = sTypeInfo->GetVariable(&student, "mPos");
		Debug::Log("Get value2 %s", std::any_cast<Vector3>(pos));

		auto qTypeInfo = Reflection::Dynamic::GetTypeInfo<Quaternion>()->AsClass();

		Debug::Log("Type name: %s", qTypeInfo->GetName());
		for (auto& item : qTypeInfo->GetVariables())
		{
			Debug::Log("Variable: %s %s", item->type->GetName(), item->name);
		}
		for (auto& item : qTypeInfo->GetFunctions())
		{
			Debug::Log("Function: %s return %s", item->name, item->returnType->GetName());
			for (auto& param : item->paramTypes)
			{
				Debug::Log("Param: %s", param->GetName());
			}
		}

		// Variable copy test
		Reflection::Dynamic::Variable var = Reflection::Dynamic::Variable::Copy(student);
	}
}