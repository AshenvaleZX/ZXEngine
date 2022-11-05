#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion() {};

		static Quaternion Euler(float x, float y, float z);

		// ���������
		Quaternion operator *(const Quaternion& q) const;
		void operator *= (const Quaternion& q);

		// ���������ĺ����const����˼���Ǳ�ʾ������������޸����е��κ����ݳ�Ա������Ҳ�С�
		// ���Ǽ��˵Ļ�����������ȥ�ľͻᱨ��ǿ�����㲻�ܸģ���ֹд�����ʱ��С��ȥ���ˣ���ǿ��׳�ԡ�
		float Normal() const;
		float NormalSquare() const;
		void Normalize();
		vec3 GetEulerAngles() const;
		void SetEulerAngles(float x, float y, float z);
	};
}
