#pragma once
#include "../pubh.h"

// �̰߳�ȫ���������У�����˼·��
// �����ɵ�������ʵ�֣������е�Ԫ�ض���unique_ptr<Node>��head�Ǳ�ͷ
// tail�Ǹ�ԭʼָ�룬���������ָ����������һ��Ԫ��
// ��������һ����λ�ڵ㣬��Զ��β����Ҳ����tailָ��Ľڵ㣬���Ե�����Ϊ��ʱ������Ҳ��һ���ڵ㣬��tail�ͻ�ָ��haed�ڵ�
// ������Ƶĺô�����ȫ����tail��head����ʹ���ڶ���Ϊ�յ������Ҳ�����
// ����һ��Pushֻ�����tail��Popֻ���޸�head�Ͷ��ݵĶ�ȡtail��һ�±Ƚ�
// Push��Pop������ͬʱ���������ϵ�ͬһ���ڵ��ˣ�head��tail�Ϳ��Ը����ö�����mutex�������ˣ���ǿ�˲�����
// Class���ڲ���2�������������в��������ȶ�head�������ٶ�tail�������ϸ�֤�˼���˳�����Բ����������

namespace ZXEngine
{
	namespace Concurrent
	{
		// ֧�ֶ��߳�Push�����߳�Pop����������
		template<typename T>
		class Queue
		{
		private:
			struct Node
			{
				shared_ptr<T> data;
				unique_ptr<Node> next;
			};

		public:
			Queue() : head(new Node()), tail(head.get()) {};
			Queue(const Queue& other) = delete;
			Queue& operator= (const Queue& other) = delete;

			shared_ptr<T> TryPop()
			{
				// �����̰߳�ȫ��˽�к���TryPopHeadȡ��ͷ�ڵ㣬������к����������
				unique_ptr<Node> oldHead = TryPopHead();
				// ����õ����ǿ�ָ�룬˵������Ϊ�գ�Popʧ�ܣ����ؿ�ָ��
				return oldHead ? oldHead->data : shared_ptr<T>();
			}

			bool TryPop(T& value)
			{
				// �����̰߳�ȫ��˽�к���TryPopHeadȡ��ͷ�ڵ㲢����ģ�����ݣ�������к����������
				unique_ptr<Node> const oldHead = TryPopHead(value);
				// ����ֱ�ӷ���oldHead�����oldHead�ǿ�ָ����൱�ڷ�����false
				return oldHead;
			}

			shared_ptr<T> WaitAndPop()
			{
				// �����̰߳�ȫ��˽�к���WaitPopHeadȡ��ͷ�ڵ�
				// ͷ�ڵ㷵�ػ�����ʱ���Ѿ��̰߳�ȫ�شӶ����Ƴ��ˣ�����������к����������
				unique_ptr<Node> const oldHead = WaitPopHead();
				// ֱ��ȡ���ݷ���
				return oldHead->data;
			}

			void WaitAndPop(T& value)
			{
				// �����̰߳�ȫ��˽�к���WaitPopHeadȡ��ͷ�ڵ㲢���ģ������
				unique_ptr<Node> const oldHead = WaitPopHead(value);
			}

			void Push(T newValue)
			{
				// �����д��벻���ʶ��У����������ڻ�����������ִ�У�����˲����̶�
				shared_ptr<T> newData(make_shared<T>(std::move(newValue)));
				unique_ptr<Node> p(new Node());
				// ����Ϊ�˼�ʱ���������˸������ţ������Ž���ʱtailLock�ͻ������������Զ�����
				{
					// Push����ֻ�����tail������ֻ��Ҫ�ڷ���tail���⼸�д���������ɱ�֤�������е��̰߳�ȫ
					std::lock_guard<std::mutex> tailLock(tailMutex);
					tail->data = newData;
					Node* const newTail = p.get();
					tail->next = std::move(p);
					tail = newTail;
				}
				// ֪ͨ�ȴ���dataCondition�ϵ��̣߳�����������������
				// ��Ϊһ��Pushֻ�����һ�����ݣ�����ֻ��Ҫ֪ͨһ���߳���ȡ����
				dataCondition.notify_one();
			}

			bool Empty()
			{
				// head��tailָ����ͬ�ڵ�ʱ������Ϊ��
				std::lock_guard<std::mutex> headLock(headMutex);
				return (head.get() == GetTail());
			}

		private:
			Node* tail;
			unique_ptr<Node> head;
			std::mutex tailMutex;
			std::mutex headMutex;
			std::condition_variable dataCondition;

			Node* GetTail()
			{
				// ��ȡtailָ�룬��Ȼֻ��һ��return�����ǻ���Ҫ����
				// ������ܻ���tail��ָ��Ľڵ����ڱ�Push�����޸ĵ�һ��ʱ����tail����
				std::lock_guard<std::mutex> tailLock(tailMutex);
				// ���ص�ʱ��Ὣtailָ�븴��һ���ٷ��أ�������Ʋ���Ҳ�ڻ������ı����½���
				// ���������ָ��Ķ�д�����������ԭ�ӵ�
				return tail;
			}

			unique_ptr<Node> PopHead()
			{
				// �������ȡ����ǰ��head��Ȼ��headָ����һ���ڵ�
				// ����δ���������������������˵�����
				unique_ptr<Node> oldHead = std::move(head);
				head = std::move(oldHead->next);
				return oldHead;
			}

			std::unique_lock<std::mutex> WaitForData()
			{
				// ���û�unique_lock��סhead�ڵ㣬��ʹ�����������ȴ��������п�ȡ������
				// ����ʹ��unique_lock����Ϊ�ȴ������п��ܻ����Ȼ��ʹ�߳�������ȴ����ȴ��������ٴμ���
				// ��lock_guardֻ���ڹ���ʱ����������ʱ�������޷����м������������Ҫ������ʱ�ֶ������ͼ�����unique_lock
				std::unique_lock<std::mutex> headLock(headMutex);
				dataCondition.wait(headLock, [&] { return head.get() != GetTail(); });
				// ��unique_lock������Ȩת�Ƹ������ߣ��õ����߿�����ͬһ�����ı�����ִ����������
				return std::move(headLock);
			}

			unique_ptr<Node> WaitPopHead()
			{
				// �ȴ����б������������ݣ�WaitForData�Ὣ��head��ӵĻ���������
				// �����õ�֮ǰ������ͨ��ֱ�ӳ�ʼ���ķ�ʽ����ת�Ƶ��˱�������headLock������
				// ����û��д��headLock = WaitForData()����ʽ����Ϊ���ǿ�����ʼ�����ᷢ��һ�����ݿ���
				std::unique_lock<std::mutex> headLock(WaitForData());
				// �������PopHead�������ڻ������ı����½�head�ڵ���ƣ�����ȡԭhead�ڵ�
				return PopHead();
			}
			
			unique_ptr<Node> WaitPopHead(T& value)
			{
				// ����ļ�������ͬ��
				std::unique_lock<std::mutex> headLock(WaitForData());
				// �ڻ������ı����¶�ȡhead�ڵ������
				value = std::move(*head->data);
				// Ȼ�����PopHead�������ڻ������ı����½�head�ڵ���ƣ�����ȡԭhead�ڵ�
				return PopHead();
			}

			unique_ptr<Node> TryPopHead()
			{
				// �ȼӶ�head�ڵ����
				std::lock_guard<std::mutex> headLock(headMutex);
				// �������Ϊ�գ����ؿ�ָ��
				if (head.get() == GetTail())
				{
					return unique_ptr<Node>();
				}
				// �������PopHead�������ڻ������ı����½�head�ڵ���ƣ�����ȡԭhead�ڵ�
				return PopHead();
			}

			unique_ptr<Node> TryPopHead(T& value)
			{
				// �ȼӶ�head�ڵ����
				std::lock_guard<std::mutex> headLock(headMutex);
				// �������Ϊ�գ����ؿ�ָ��
				if (head.get() == GetTail())
				{
					return unique_ptr<Node>();
				}
				// ���в�Ϊ��ʱ�������ı����¶�ȡhead�ڵ������
				value = std::move(*head->data);
				// Ȼ�����PopHead������Ҳ���ı����½�head�ڵ���ƣ�����ȡԭhead�ڵ�
				return PopHead();
			}
		};
	}
}