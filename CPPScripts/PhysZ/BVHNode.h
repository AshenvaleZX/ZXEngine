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
			BoundingVolume* mBoundingVolume;

			BVHNode(BVHNode* parent, BoundingVolume* boundingVolume, RigidBody* rigidBody):
			~BVHNode() {};

			// 是否为叶子节点
			bool IsLeaf() const;
			void Insert(const RigidBody* body, const BoundingVolume* volume);
			// 检测当前节点的BV所包围的所有刚体之间的潜在碰撞，写入一个PotentialContact数组中
			// 第一个参数是PotentialContact数组地址，第二个参数是PotentialContact数组长度
			// 返回值是实际写入的PotentialContact数量，也就是碰撞数量(不会超过limit)
			uint32_t GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const;

		private:
			bool IsOverlapWith(const BVHNode* other) const;
			void RecalculateBoundingVolume(bool recurse = true);
			// 获取当前节点包围的所有刚体和另一个节点包围的所有刚体之间潜在的碰撞
			// 第二个参数是PotentialContact数组地址，第三个参数是PotentialContact数组长度
			// (实际上调用这个函数的时候传入的数组可能是从另一个数组的中间开始的)
			// 返回值是实际写入的PotentialContact数量，也就是碰撞数量(不会超过limit)
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
	}
}