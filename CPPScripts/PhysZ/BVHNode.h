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
			BoundingVolume mBoundingVolume;

			// parentΪ�ձ�ʾ�Ǹ��ڵ�
			BVHNode(BVHNode* parent, const BoundingVolume& boundingVolume, RigidBody* rigidBody);
			// ����������ݹ�ɾ�������ӽڵ�
			~BVHNode();

			// �Ƿ�ΪҶ�ӽڵ�
			bool IsLeaf() const;
			// �ڵ�ǰ�ڵ��²���һ�����壬�����ǰ�ڵ���Ҷ�ӽڵ������Ϊ���ڵ�Ȼ���ԭ��ǰ������¸�����2���ӽڵ�
			// �����ǰ�ڵ㲻��Ҷ�ӽڵ㣬�ͻ����²���Ҷ�ӽڵ�Ȼ�����
			void Insert(const BoundingVolume& volume, RigidBody* body);
			// ��⵱ǰ�ڵ��BV����Χ�����и���֮���Ǳ����ײ��д��һ��PotentialContact������
			// ��һ��������PotentialContact�����ַ���ڶ���������PotentialContact���鳤��
			// ����ֵ��ʵ��д���PotentialContact������Ҳ������ײ����(���ᳬ��limit)
			uint32_t GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const;

		private:
			// ����һ���ڵ��BV�Ƿ����ص�
			bool IsOverlapWith(const BVHNode* other) const;
			// �������ɵ�ǰ�ڵ��BV��Ĭ�ϻ�����ڵ�ݹ�����
			void RecalculateBoundingVolume(bool recurse = true);
			// ��ȡ��ǰ�ڵ��Χ�����и������һ���ڵ��Χ�����и���֮��Ǳ�ڵ���ײ
			// �ڶ���������PotentialContact�����ַ��������������PotentialContact���鳤��
			// (ʵ���ϵ������������ʱ�������������Ǵ���һ��������м俪ʼ��)
			// ����ֵ��ʵ��д���PotentialContact������Ҳ������ײ����(���ᳬ��limit)
			uint32_t GetPotentialContactsWith(const BVHNode* other, PotentialContact* contacts, uint32_t limit) const;
		};
	
		template<class BoundingVolume>
		BVHNode<BoundingVolume>::BVHNode(BVHNode* parent, const BoundingVolume& boundingVolume, RigidBody* rigidBody) :
			mParent(parent),
			mBoundingVolume(boundingVolume),
			mRigidBody(rigidBody)
		{
			mChildren[0] = mChildren[1] = nullptr;
		}

		template<class BoundingVolume>
		BVHNode<BoundingVolume>::~BVHNode()
		{
			if (mParent != nullptr)
			{
				// ���ҵ��ֵܽڵ�
				BVHNode* sibling = (mParent->mChildren[0] == this) ? mParent->mChildren[1] : mParent->mChildren[0];

				// ���ֵܽڵ�����ݸ��Ƶ����ڵ�
				mParent->mRigidBody      = sibling->mRigidBody;
				mParent->mChildren[0]    = sibling->mChildren[0];
				mParent->mChildren[1]    = sibling->mChildren[1];
				mParent->mBoundingVolume = sibling->mBoundingVolume;

				// Ȼ����ֵܽڵ�ɾ��
				sibling->mParent      = nullptr;
				sibling->mRigidBody   = nullptr;
				sibling->mChildren[0] = nullptr;
				sibling->mChildren[1] = nullptr;
				delete sibling;

				// ������¼��㸸�ڵ��BV
				mParent->RecalculateBoundingVolume();
			}
			
			// �ݹ�ɾ���ӽڵ㣬ɾ֮ǰ�ȰѸ��ڵ�������գ������ӽڵ�ɾ��ʱ�����ٴ����ֵܽڵ�
			if (mChildren[0] != nullptr)
			{
				mChildren[0]->mParent = nullptr;
				delete mChildren[0];
			}
			if (mChildren[1] != nullptr)
			{
				mChildren[1]->mParent = nullptr;
				delete mChildren[1];
			}
		}

		template<class BoundingVolume>
		bool BVHNode<BoundingVolume>::IsLeaf() const
		{
			return mRigidBody != nullptr;
		}

		template<class BoundingVolume>
		void BVHNode<BoundingVolume>::Insert(const BoundingVolume& volume, RigidBody* body)
		{
			// �����Ҷ�ӽڵ㣬������Ϊ���ڵ�
			if (IsLeaf())
			{
				// �õ�ǰ�ڵ���������һ���ӽڵ�
				mChildren[0] = new BVHNode(this, mBoundingVolume, mRigidBody);
				// ��������������һ���ӽڵ�
				mChildren[1] = new BVHNode(this, volume, body);

				// ɾ����ǰ�ڵ�ĸ���
				mRigidBody = nullptr;
				// �������ɵ�ǰ�ڵ�����и��ڵ��BV
				RecalculateBoundingVolume();
			}
			// �������Ҷ�ӽڵ㣬�����²���Ҷ�ӽڵ�Ȼ�����
			else
			{
				// �Ƚϲ��뵽�����ӽڵ���BV��������������������С���Ǹ�
				if (mChildren[0]->mBoundingVolume.GetGrowth(volume) < mChildren[1]->mBoundingVolume.GetGrowth(volume))
					mChildren[0]->Insert(volume, body);
				else
					mChildren[1]->Insert(volume, body);
			}
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

		template<class BoundingVolume>
		void BVHNode<BoundingVolume>::RecalculateBoundingVolume(bool recurse)
		{
			// Ҷ�ӽڵ�ֻ����һ�����壬Ŀǰ������ϵͳĬ�ϸ��岻��䣬�����������¼���BV��С
			if (!IsLeaf())
				mBoundingVolume = BoundingVolume(mChildren[0]->mBoundingVolume, mChildren[1]->mBoundingVolume);

			// �����Ҫ�ݹ飬�͵ݹ���¸��ڵ��BV
			if (mParent != nullptr && recurse)
				mParent->RecalculateBoundingVolume(true);
		}
	}
}