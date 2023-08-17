#include "BVHNode.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		BVHNode::BVHNode(BVHNode* parent, const BoundingSphere& boundingVolume, RigidBody* rigidBody) :
			mParent(parent),
			mBoundingVolume(boundingVolume),
			mRigidBody(rigidBody)
		{
			rigidBody->mBVHNode = this;
			mChildren[0] = mChildren[1] = nullptr;
		}

		BVHNode::~BVHNode()
		{
			if (mParent != nullptr)
			{
				// ���ҵ��ֵܽڵ�
				BVHNode* sibling = (mParent->mChildren[0] == this) ? mParent->mChildren[1] : mParent->mChildren[0];

				// ���ֵܽڵ�����ݸ��Ƶ����ڵ�
				mParent->mRigidBody = sibling->mRigidBody;
				mParent->mChildren[0] = sibling->mChildren[0];
				mParent->mChildren[1] = sibling->mChildren[1];
				mParent->mBoundingVolume = sibling->mBoundingVolume;

				// ������ڵ��и��壬ͬ�����¸����BVHNodeָ��
				if (mParent->mRigidBody)
					mParent->mRigidBody->mBVHNode = mParent;

				// Ȼ����ֵܽڵ�ɾ��
				sibling->mParent = nullptr;
				sibling->mRigidBody = nullptr;
				sibling->mChildren[0] = nullptr;
				sibling->mChildren[1] = nullptr;
				delete sibling;

				// ������¼��㸸�ڵ��BV
				mParent->UpdateBoundingVolume();
			}

			// ����и��壬���������˵�ǰ�ڵ㣬���������ñ���Ұָ��
			if (mRigidBody && mRigidBody->mBVHNode == this)
				mRigidBody->mBVHNode = nullptr;

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

		bool BVHNode::IsLeaf() const
		{
			return mRigidBody != nullptr;
		}

		void BVHNode::UpdateBoundingVolume(bool recurse)
		{
			// Ҷ�ӽڵ�ֻ����һ�����壬Ŀǰ������ϵͳĬ�ϸ��岻��䣬�����������¼���BV��С
			if (!IsLeaf())
				mBoundingVolume = BoundingSphere(mChildren[0]->mBoundingVolume, mChildren[1]->mBoundingVolume);

			// �����Ҫ�ݹ飬�͵ݹ���¸��ڵ��BV
			if (mParent != nullptr && recurse)
				mParent->UpdateBoundingVolume(true);
		}

		void BVHNode::Insert(const BoundingSphere& volume, RigidBody* body)
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
				UpdateBoundingVolume();
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

		uint32_t BVHNode::GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const
		{
			if (IsLeaf() || limit == 0)
				return 0;

			return mChildren[0]->GetPotentialContactsWith(mChildren[1], contacts, limit);
		}

		uint32_t BVHNode::GetPotentialContactsWith(const BVHNode* other, PotentialContact* contacts, uint32_t limit) const
		{
			// ���û���ص����߳���������ƣ�����0
			if (!IsOverlapWith(other) || limit == 0)
				return 0;

			// �����ߵ�����ͱ�ʾ�����ڵ��BV���ཻ��
			// ��������ڵ㶼���ӽڵ㣬��ô���п��ܲ���һ��ʵ����ײ
			if (IsLeaf() && other->IsLeaf())
			{
				contacts->mRigidBodies[0] = mRigidBody;
				contacts->mRigidBodies[1] = other->mRigidBody;
				return 1;
			}

			// �������һ��BV��Ҷ�ӽڵ�(˵����ǰ����ڵ㲻��Ҷ�ӽڵ�)
			// ����˵�������������Ҷ�ӽڵ㣬���ǵ�ǰ���BV�������
			// ���õ�ǰ����ڵ�������ӽڵ���ȥ����һ���ڵ����Ƿ���ڿ��ܷ�������ײ
			if (other->IsLeaf() || (!IsLeaf() && mBoundingVolume.GetVolume() >= other->mBoundingVolume.GetVolume()))
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

		bool BVHNode::IsOverlapWith(const BVHNode* other) const
		{
			return mBoundingVolume.IsOverlapWith(other->mBoundingVolume);
		}
	}
}