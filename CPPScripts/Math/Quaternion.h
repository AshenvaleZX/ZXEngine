#pragma once

namespace ZXEngine
{
	class Vector3;
	class Matrix4;
	class Quaternion
	{
	public:
		static Quaternion Euler(float x, float y, float z);

	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion() {};

		// ���������ĺ����const����˼���Ǳ�ʾ������������޸����е��κ����ݳ�Ա������Ҳ�С�
		// ���Ǽ��˵Ļ�����������ȥ�ľͻᱨ��ǿ�����㲻�ܸģ���ֹд�����ʱ��С��ȥ���ˣ���ǿ��׳�ԡ�
		float Normal() const;
		float NormalSquare() const;
		void Normalize();
		Vector3 GetEulerAngles() const;
		void SetEulerAngles(float x, float y, float z);
		Matrix4 ToMatrix();

		Quaternion operator* (const Quaternion& q) const;
		Quaternion& operator*= (const Quaternion& q);
	};
}
