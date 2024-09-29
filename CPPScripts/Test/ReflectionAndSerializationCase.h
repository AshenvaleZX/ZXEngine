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

	class Student
	{
	public:
		string mName;
		uint32_t mAge = 0;
		float mHeight = 0.0f;
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

	StaticReflection
	(
		Student,
		Fields
		(
			Field(&Student::mAge),
			Field(&Student::mHeight),
			Field(&Student::mPos)
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

		json student_data = json::parse(res);

		Student student2;
		Serialization::Deserialize(student2, student_data);

		Student student3 = Serialization::Deserialize<Student>(student_data);

		// Dynamic reflection test for enum
		auto& eFac = Reflection::Dynamic::Register<MyEnum>();
		eFac.Register("MyEnum");
		eFac.AddEnum("One", MyEnum::One);
		eFac.AddEnum("Two", MyEnum::Two);
		eFac.AddEnum("Three", MyEnum::Three);

		auto typeInfo = Reflection::Dynamic::GetTypeInfo("MyEnum");
		auto enumInfo = typeInfo->AsEnum();

		Debug::Log("Type name: %s", typeInfo->GetName());
		for (auto& item : enumInfo->GetItems())
		{
			Debug::Log("Enum item: %s value %s", item.name, item.value);
		}

		// Dynamic reflection test for class
		auto& strFac = Reflection::Dynamic::Register<string>();
		strFac.Register("string");

		auto& vFac = Reflection::Dynamic::Register<Vector3>();
		vFac.Register("Vector3");
		vFac.AddVariable("x", &Vector3::x);
		vFac.AddVariable("y", &Vector3::y);
		vFac.AddVariable("z", &Vector3::z);

		auto& sFac = Reflection::Dynamic::Register<Student>();
		sFac.Register("Student");
		sFac.AddVariable("mName", &Student::mName);
		sFac.AddVariable("mAge", &Student::mAge);
		sFac.AddVariable("mHeight", &Student::mHeight);
		sFac.AddVariable("mPos", &Student::mPos);
		sFac.AddFunction("GetAge", &Student::GetAge);
		sFac.AddFunction("Speak", &Student::Speak);

		auto sTypeInfo = Reflection::Dynamic::GetTypeInfo("Student");
		auto sInfo = sTypeInfo->AsClass();

		Debug::Log("Type name: %s", sTypeInfo->GetName());
		for (auto& item : sInfo->GetVariables())
		{
			Debug::Log("Variable: %s %s", item->type->GetName(), item->name);
		}
		for (auto& item : sInfo->GetFunctions())
		{
			Debug::Log("Function: %s return %s", item->name, item->returnType->GetName());
			for (auto& param : item->paramTypes)
			{
				Debug::Log("Param: %s", param->GetName());
			}
		}

		Debug::Log("Origin mAge %s", student.mAge);
		sInfo->SetVariable(&student, "mAge", 12);
		Debug::Log("New mAge %s", student.mAge);

		Debug::Log("Origin mPos %s", student.mPos);
		Vector3 nPos = { 6, 0, 3 };
		sInfo->SetVariable(&student, "mPos", nPos);
		Debug::Log("New mPos %s", student.mPos);

		auto age = sInfo->GetVariable(&student, "mAge");
		Debug::Log("Get value1 %s", std::any_cast<uint32_t>(age));
		auto pos = sInfo->GetVariable(&student, "mPos");
		Debug::Log("Get value2 %s", std::any_cast<Vector3>(pos));

		// Variable copy test
		Reflection::Dynamic::Variable var = Reflection::Dynamic::Variable::Copy(student);
	}
}