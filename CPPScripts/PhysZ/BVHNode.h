#pragma once
#include "../pubh.h"
#include "PhysZEnumStruct.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;

		// Bounding Volume Hierarchy Node
		template<class BoundingVolume>
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
			BoundingVolume* mBoundingVolume;

			BVHNode(BVHNode* parent, BoundingVolume* boundingVolume, RigidBody* rigidBody):
			~BVHNode() {};

			// �Ƿ�ΪҶ�ӽڵ�
			bool IsLeaf() const;
			void Insert(const RigidBody* body, const BoundingVolume* volume);
			// ��⵱ǰ�ڵ��BV����Χ�����и���֮���Ǳ����ײ��д��һ��PotentialContact������
			// ��һ��������PotentialContact�����ַ���ڶ���������PotentialContact���鳤��
			// ����ֵ��ʵ��д���PotentialContact������Ҳ������ײ����(���ᳬ��limit)
			uint32_t GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const;

		private:
			bool IsOverlapWith(const BVHNode* other) const;
			void RecalculateBoundingVolume(bool recurse = true);
			// ��ȡ��ǰ�ڵ��Χ�����и������һ���ڵ��Χ�����и���֮��Ǳ�ڵ���ײ
			// �ڶ���������PotentialContact�����ַ��������������PotentialContact���鳤��
			// (ʵ���ϵ������������ʱ�������������Ǵ���һ��������м俪ʼ��)
			// ����ֵ��ʵ��д���PotentialContact������Ҳ������ײ����(���ᳬ��limit)
			uint32_t GetPotentialContactsWith(const BVHNode* other, PotentialContact* contacts, uint32_t limit) const;
		};
	
		template<class BoundingVolume>
		BVHNode<BoundingVolume>::BVHNode(BVHNode* parent, BoundingVolume* boundingVolume, RigidBody* rigidBody) :
			mParent(parent),
			mBoundingVolume(boundingVolume),
			mRigidBody(rigidBody)
		{
			mChildren[0] = mChildren[1] = nullptr;
		}

		template<class BoundingVolume>
		bool BVHNode<BoundingVolume>::IsLeaf() const
		{
			return mRigidBody != nullptr;
		}

		template<class BoundingVolume>
		uint32_t BVHNode<BoundingVolume>::GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const
		{
			if (IsLeaf() || limit == 0) 
				return 0;

			return mChildren[0]->GetPotentialContactsWith(mChildren[1], contacts, limit);
		}

		template<class BoundingVolume>
		uint32_t BVHNode<BoundingVolume>::GetPotentialContactsWith(const BVHNode* other, PotentialContact* contacts, uint32_t limit) const
		{
			// ���û���ص����߳���������ƣ�����0
			if (!IsOverlapWith(other) || limit == 0) 
				return 0;

			// �����ߵ�����ͱ�ʾ�����ڵ��BV���ཻ��
			// ��������ڵ㶼���ӽڵ㣬��ô���п��ܲ���һ��ʵ����ײ
			if (IsLeaf() && other->IsLeaf())
			{
				contacts->bodies[0] = mRigidBody;
				contacts->bodies[1] = other->mRigidBody;
				return 1;
			}

			// �������һ��BV��Ҷ�ӽڵ�(˵����ǰ����ڵ㲻��Ҷ�ӽڵ�)
			// ����˵�������������Ҷ�ӽڵ㣬���ǵ�ǰ���BV�������
			// ���õ�ǰ����ڵ�������ӽڵ���ȥ����һ���ڵ����Ƿ���ڿ��ܷ�������ײ
			if (other->IsLeaf() || (!IsLeaf() && mBoundingVolume->GetVolume() >= other->mBoundingVolume->GetVolume()))
			{
				// �ݹ����ӽڵ�
				uint32_t count = mChildren[0]->GetPotentialContactsWith(other, contacts, limit);
				// ����Ѿ���⵽�㹻�����ײ���Ͳ��ټ����һ���ӽڵ���
				if (limit > count)
					return count + mChildren[1]->GetPotentialContactsWith(other, contacts + count, limit - count);
				else
					return count;
			}
			// �����ǰ���BV��Ҷ�ӽڵ�(˵����һ���ڵ㲻��Ҷ�ӽڵ�)
			// ����˵�������������Ҷ�ӽڵ㣬������һ��BV�������
			// ������һ���ڵ�������ӽڵ���ȥ�͵�ǰ����ڵ����Ƿ���ڿ��ܷ�������ײ
			else
			{
				// �ݹ����ӽڵ�
				uint32_t count = GetPotentialContactsWith(other->mChildren[0], contacts, limit);
				// ����Ѿ���⵽�㹻�����ײ���Ͳ��ټ����һ���ӽڵ���
				if (limit > count)
					return count + GetPotentialContactsWith(other->mChildren[1], contacts + count, limit - count);
				else
					return count;
			}
		}

		template<class BoundingVolume>
		bool BVHNode<BoundingVolume>::IsOverlapWith(const BVHNode* other) const
		{
			return mBoundingVolume->IsOverlapWith(other->mBoundingVolume);
		}
	}
}