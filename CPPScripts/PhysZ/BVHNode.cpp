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
				// 先找到兄弟节点
				BVHNode* sibling = (mParent->mChildren[0] == this) ? mParent->mChildren[1] : mParent->mChildren[0];

				// 把兄弟节点的数据复制到父节点
				mParent->mRigidBody = sibling->mRigidBody;
				mParent->mChildren[0] = sibling->mChildren[0];
				mParent->mChildren[1] = sibling->mChildren[1];
				mParent->mBoundingVolume = sibling->mBoundingVolume;

				// 如果父节点有刚体，同步更新刚体的BVHNode指针
				if (mParent->mRigidBody)
					mParent->mRigidBody->mBVHNode = mParent;

				// 然后把兄弟节点删除
				sibling->mParent = nullptr;
				sibling->mRigidBody = nullptr;
				sibling->mChildren[0] = nullptr;
				sibling->mChildren[1] = nullptr;
				delete sibling;

				// 最后重新计算父节点的BV
				mParent->UpdateBoundingVolume();
			}

			// 如果有刚体，并且引用了当前节点，就消除引用避免野指针
			if (mRigidBody && mRigidBody->mBVHNode == this)
				mRigidBody->mBVHNode = nullptr;

			// 递归删除子节点，删之前先把父节点引用清空，这样子节点删除时无需再处理兄弟节点
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
			// 叶子节点只包含一个刚体，目前的物理系统默认刚体不会变，所以无需重新计算BV大小
			if (!IsLeaf())
				mBoundingVolume = BoundingSphere(mChildren[0]->mBoundingVolume, mChildren[1]->mBoundingVolume);

			// 如果需要递归，就递归更新父节点的BV
			if (mParent != nullptr && recurse)
				mParent->UpdateBoundingVolume(true);
		}

		void BVHNode::Insert(const BoundingSphere& volume, RigidBody* body)
		{
			// 如果是叶子节点，就升级为父节点
			if (IsLeaf())
			{
				// 用当前节点数据生成一个子节点
				mChildren[0] = new BVHNode(this, mBoundingVolume, mRigidBody);
				// 用新数据生成另一个子节点
				mChildren[1] = new BVHNode(this, volume, body);

				// 删掉当前节点的刚体
				mRigidBody = nullptr;
				// 重新生成当前节点和所有父节点的BV
				UpdateBoundingVolume();
			}
			// 如果不是叶子节点，就向下查找叶子节点然后插入
			else
			{
				// 比较插入到两个子节点后的BV增长量，插入增长量更小的那个
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
			// 如果没有重叠或者超过检测限制，返回0
			if (!IsOverlapWith(other) || limit == 0)
				return 0;

			// 代码走到这里就表示两个节点的BV是相交的
			// 如果两个节点都是子节点，那么就有可能产生一个实际碰撞
			if (IsLeaf() && other->IsLeaf())
			{
				contacts->mRigidBodies[0] = mRigidBody;
				contacts->mRigidBodies[1] = other->mRigidBody;
				return 1;
			}

			// 如果另外一个BV是叶子节点(说明当前这个节点不是叶子节点)
			// 或者说如果两个都不是叶子节点，但是当前这个BV体积更大
			// 就拿当前这个节点的两个子节点再去和另一个节点检测是否存在可能发生的碰撞
			if (other->IsLeaf() || (!IsLeaf() && mBoundingVolume.GetVolume() >= other->mBoundingVolume.GetVolume()))
			{
				// 递归检测子节点
				uint32_t count = mChildren[0]->GetPotentialContactsWith(other, contacts, limit);
				// 如果已经检测到足够多的碰撞，就不再检测另一个子节点了
				if (limit > count)
					return count + mChildren[1]->GetPotentialContactsWith(other, contacts + count, limit - count);
				else
					return count;
			}
			// 如果当前这个BV是叶子节点(说明另一个节点不是叶子节点)
			// 或者说如果两个都不是叶子节点，但是另一个BV体积更大
			// 就拿另一个节点的两个子节点再去和当前这个节点检测是否存在可能发生的碰撞
			else
			{
				// 递归检测子节点
				uint32_t count = GetPotentialContactsWith(other->mChildren[0], contacts, limit);
				// 如果已经检测到足够多的碰撞，就不再检测另一个子节点了
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