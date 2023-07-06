#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace Concurrent
	{
		// 仅支持单个线程Push，单个线程Pop的无锁队列
		template<typename T>
		class LockFreeQueue
		{
		private:
			struct Node
			{
				shared_ptr<T> data;
				Node* next = nullptr;
			};

		public:
			LockFreeQueue() : head(new Node()), tail(head.load()) {};
			LockFreeQueue(const LockFreeQueue& other) = delete;
			LockFreeQueue& operator= (const LockFreeQueue& other) = delete;

			~LockFreeQueue()
			{
				while (Node* const oldHead = head.load())
				{
					head.store(oldHead->next);
					delete oldHead;
				}
			}

			shared_ptr<T> Pop()
			{
				// 调用线程安全的PopHead取出头节点
				Node* oldHead = PopHead();
				// 如果得到的是空指针，说明队列为空，Pop失败
				if (!oldHead)
				{
					return shared_ptr<T>();
				}
				// 成功取出头节点，此时头节点已经从队列里弹出了，可以随意修改不影响队列本身，并且是线程安全的
				shared_ptr<T> const res(oldHead->data);
				delete oldHead;
				return res;
			}

			void Push(T newValue)
			{
				// 只有一个线程会Push，修改tail指针，另一个Pop的线程只会读取tail，所以不存在条件竞争问题
				shared_ptr<T> newData(make_shared<T>(std::move(newValue)));
				Node* p = new Node();
				Node* const oldTail = tail.load();
				oldTail->data.swap(newData);
				oldTail->next = p;
				tail.store(p);
			}

		private:
			std::atomic<Node*> head;
			std::atomic<Node*> tail;

			Node* PopHead()
			{
				// 只有一个线程Pop，所以也就只有一个线程会修改head，也就不存在条件竞争问题了
				Node* const oldHead = head.load();
				// 如果head和tail相等，说明队列为空
				if (oldHead == tail.load())
				{
					return nullptr;
				}
				head.store(oldHead->next);
				// 这里返回的head已经从队列里弹出了，调用者拿到后可以随意修改不影响队列本身
				return oldHead;
			}
		};
	}
}