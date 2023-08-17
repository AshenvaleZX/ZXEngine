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
			// 父节点
			BVHNode* mParent;
			// 子节点
			BVHNode* mChildren[2];
			// 该节点下的刚体(通常叶子节点才有)
			RigidBody* mRigidBody;
			// 包含该节点和所有子节点下的对象的BoundingVolume
			BoundingSphere mBoundingVolume;

			// parent为空表示是根节点
			BVHNode(BVHNode* parent, const BoundingSphere& boundingVolume, RigidBody* rigidBody);
			// 析构函数会递归删除所有子节点
			~BVHNode();

			// 是否为叶子节点
			bool IsLeaf() const;
			// 更新当前节点的BV，默认会向根节点递归更新
			void UpdateBoundingVolume(bool recurse = true);
			// 在当前节点下插入一个刚体，如果当前节点是叶子节点会升级为父节点然后把原当前刚体和新刚体变成2个子节点
			// 如果当前节点不是叶子节点，就会向下查找叶子节点然后插入
			void Insert(const BoundingSphere& volume, RigidBody* body);
			// 检测当前节点的BV所包围的所有刚体之间的潜在碰撞，写入一个PotentialContact数组中
			// 第一个参数是PotentialContact数组地址，第二个参数是PotentialContact数组长度
			// 返回值是实际写入的PotentialContact数量，也就是碰撞数量(不会超过limit)
			uint32_t GetPotentialContacts(PotentialContact* contacts, uint32_t limit) const;

		private:
			// 与另一个节点的BV是否有重叠
			bool IsOverlapWith(const BVHNode* other) const;
			// 获取当前节点包围的所有刚体和另一个节点包围的所有刚体之间潜在的碰撞
			// 第二个参数是PotentialContact数组地址，第三个参数是PotentialContact数组长度
			// (实际上调用这个函数的时候传入的数组可能是从另一个数组的中间开始的)
			// 返回值是实际写入的PotentialContact数量，也就是碰撞数量(不会超过limit)
			uint32_t GetPotentialContactsWith(const BVHNode* other, PotentialContact* contacts, uint32_t limit) const;
		};
	}
}