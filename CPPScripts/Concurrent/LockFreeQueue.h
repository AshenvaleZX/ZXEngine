#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace Concurrent
	{
		// ��֧�ֵ����߳�Push�������߳�Pop����������
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
				// �����̰߳�ȫ��PopHeadȡ��ͷ�ڵ�
				Node* oldHead = PopHead();
				// ����õ����ǿ�ָ�룬˵������Ϊ�գ�Popʧ��
				if (!oldHead)
				{
					return shared_ptr<T>();
				}
				// �ɹ�ȡ��ͷ�ڵ㣬��ʱͷ�ڵ��Ѿ��Ӷ����ﵯ���ˣ����������޸Ĳ�Ӱ����б����������̰߳�ȫ��
				shared_ptr<T> const res(oldHead->data);
				delete oldHead;
				return res;
			}

			void Push(T newValue)
			{
				// ֻ��һ���̻߳�Push���޸�tailָ�룬��һ��Pop���߳�ֻ���ȡtail�����Բ�����������������
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
				// ֻ��һ���߳�Pop������Ҳ��ֻ��һ���̻߳��޸�head��Ҳ�Ͳ�������������������
				Node* const oldHead = head.load();
				// ���head��tail��ȣ�˵������Ϊ��
				if (oldHead == tail.load())
				{
					return nullptr;
				}
				head.store(oldHead->next);
				// ���ﷵ�ص�head�Ѿ��Ӷ����ﵯ���ˣ��������õ�����������޸Ĳ�Ӱ����б���
				return oldHead;
			}
		};
	}
}