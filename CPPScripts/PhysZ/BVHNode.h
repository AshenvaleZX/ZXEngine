#pragma once
#include "../pubh.h"
#include "PhysZEnumStruct.h"
#include "BoundingVolume/BoundingSphere.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		// Bounding Volume Hierarchy Node
		class BVHNode
		{
		public:
			// ���ڵ�
			BVHNode* mParent;
			// �ӽڵ�
			BVHNode* mChildren[2];
			// �ýڵ��µĸ���(ͨ��Ҷ�ӽڵ����)
			RigidBody* mRigidBody;
			// �����ýڵ�������ӽڵ��µĶ����BoundingVolume
			BoundingSphere mBoundingVolume;

			// parentΪ�ձ�ʾ�Ǹ��ڵ�
			BVHNode(BVHNode* parent, const BoundingSphere& boundingVolume, RigidBody* rigidBody);
			// ����������ݹ�ɾ�������ӽڵ�
			~BVHNode();

			// �Ƿ�ΪҶ�ӽڵ�
			bool IsLeaf() const;
			// ���µ�ǰ�ڵ��BV��Ĭ�ϻ�����ڵ�ݹ����
			void UpdateBoundingVolume(bool recurse = true);
			// �ڵ�ǰ�ڵ��²���һ�����壬�����ǰ�ڵ���Ҷ�ӽڵ������Ϊ���ڵ�Ȼ���ԭ��ǰ������¸�����2���ӽڵ�
			// �����ǰ�ڵ㲻��Ҷ�ӽڵ㣬�ͻ����²���Ҷ�ӽڵ�Ȼ�����
			void Insert(const BoundingSphere& volume, RigidBody* body);
			// ��⵱ǰ�ڵ��BV����Χ�����и���֮���Ǳ����ײ��д��һ��PotentialContact������
			// ��һ��������PotentialContact�����ַ���ڶ���������PotentialContact���鳤��
			// ����ֵ��ʵ��д���PotentialContact������Ҳ������ײ����(���ᳬ��limit)
			uint32_t GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const;

		private:
			// ����һ���ڵ��BV�Ƿ����ص�
			bool IsOverlapWith(const BVHNode* other) const;
			// ��ȡ��ǰ�ڵ��Χ�����и������һ���ڵ��Χ�����и���֮��Ǳ�ڵ���ײ
			// �ڶ���������PotentialContact�����ַ��������������PotentialContact���鳤��
			// (ʵ���ϵ������������ʱ�������������Ǵ���һ��������м俪ʼ��)
			// ����ֵ��ʵ��д���PotentialContact������Ҳ������ײ����(���ᳬ��limit)
			uint32_t GetPotentialContactsWith(const BVHNode* other, PotentialContact* contacts, uint32_t limit) const;
		};
	}
}