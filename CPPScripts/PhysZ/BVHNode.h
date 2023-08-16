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
			// 父节点
			BVHNode* mParent;
			// 子节点
			BVHNode* mChildren[2];
			// 该节点下的刚体(通常叶子节点才有)
			RigidBody* mRigidBody;
			// 包含该节点和所有子节点下的对象的BoundingVolume
			BoundingVolume mBoundingVolume;

			// parent为空表示是根节点
			BVHNode(BVHNode* parent, const BoundingVolume& boundingVolume, RigidBody* rigidBody);
			// 析构函数会递归删除所有子节点
			~BVHNode();

			// 是否为叶子节点
			bool IsLeaf() const;
			// 在当前节点下插入一个刚体，如果当前节点是叶子节点会升级为父节点然后把原当前刚体和新刚体变成2个子节点
			// 如果当前节点不是叶子节点，就会向下查找叶子节点然后插入
			void Insert(const BoundingVolume& volume, RigidBody* body);
			// 检测当前节点的BV所包围的所有刚体之间的潜在碰撞，写入一个PotentialContact数组中
			// 第一个参数是PotentialContact数组地址，第二个参数是PotentialContact数组长度
			// 返回值是实际写入的PotentialContact数量，也就是碰撞数量(不会超过limit)
			uint32_t GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const;

		private:
			// 与另一个节点的BV是否有重叠
			bool IsOverlapWith(const BVHNode* other) const;
			// 重新生成当前节点的BV，默认会向根节点递归生成
			void RecalculateBoundingVolume(bool recurse = true);
			// 获取当前节点包围的所有刚体和另一个节点包围的所有刚体之间潜在的碰撞
			// 第二个参数是PotentialContact数组地址，第三个参数是PotentialContact数组长度
			// (实际上调用这个函数的时候传入的数组可能是从另一个数组的中间开始的)
			// 返回值是实际写入的PotentialContact数量，也就是碰撞数量(不会超过limit)
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
				// 先找到兄弟节点
				BVHNode* sibling = (mParent->mChildren[0] == this) ? mParent->mChildren[1] : mParent->mChildren[0];

				// 把兄弟节点的数据复制到父节点
				mParent->mRigidBody      = sibling->mRigidBody;
				mParent->mChildren[0]    = sibling->mChildren[0];
				mParent->mChildren[1]    = sibling->mChildren[1];
				mParent->mBoundingVolume = sibling->mBoundingVolume;

				// 然后把兄弟节点删除
				sibling->mParent      = nullptr;
				sibling->mRigidBody   = nullptr;
				sibling->mChildren[0] = nullptr;
				sibling->mChildren[1] = nullptr;
				delete sibling;

				// 最后重新计算父节点的BV
				mParent->RecalculateBoundingVolume();
			}
			
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

		template<class BoundingVolume>
		bool BVHNode<BoundingVolume>::IsLeaf() const
		{
			return mRigidBody != nullptr;
		}

		template<class BoundingVolume>
		void BVHNode<BoundingVolume>::Insert(const BoundingVolume& volume, RigidBody* body)
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
				RecalculateBoundingVolume();
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
			// 如果没有重叠或者超过检测限制，返回0
			if (!IsOverlapWith(other) || limit == 0) 
				return 0;

			// 代码走到这里就表示两个节点的BV是相交的
			// 如果两个节点都是子节点，那么就有可能产生一个实际碰撞
			if (IsLeaf() && other->IsLeaf())
			{
				contacts->bodies[0] = mRigidBody;
				contacts->bodies[1] = other->mRigidBody;
				return 1;
			}

			// 如果另外一个BV是叶子节点(说明当前这个节点不是叶子节点)
			// 或者说如果两个都不是叶子节点，但是当前这个BV体积更大
			// 就拿当前这个节点的两个子节点再去和另一个节点检测是否存在可能发生的碰撞
			if (other->IsLeaf() || (!IsLeaf() && mBoundingVolume->GetVolume() >= other->mBoundingVolume->GetVolume()))
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

		template<class BoundingVolume>
		bool BVHNode<BoundingVolume>::IsOverlapWith(const BVHNode* other) const
		{
			return mBoundingVolume->IsOverlapWith(other->mBoundingVolume);
		}

		template<class BoundingVolume>
		void BVHNode<BoundingVolume>::RecalculateBoundingVolume(bool recurse)
		{
			// 叶子节点只包含一个刚体，目前的物理系统默认刚体不会变，所以无需重新计算BV大小
			if (!IsLeaf())
				mBoundingVolume = BoundingVolume(mChildren[0]->mBoundingVolume, mChildren[1]->mBoundingVolume);

			// 如果需要递归，就递归更新父节点的BV
			if (mParent != nullptr && recurse)
				mParent->RecalculateBoundingVolume(true);
		}
	}
}