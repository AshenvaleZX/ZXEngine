#pragma once
#include "../pubh.h"

// 线程安全的有锁队列，整体思路：
// 队列由单向链表实现，链表中的元素都是unique_ptr<Node>，head是表头
// tail是个原始指针，不在链表里，指向链表的最后一个元素
// 链表里有一个虚位节点，永远在尾部，也就是tail指向的节点，所以当队列为空时链表里也有一个节点，即tail就会指向haed节点
// 这样设计的好处是完全分离tail和head，即使是在队列为空的情况下也是如此
// 这样一来Push只会访问tail，Pop只会修改head和短暂的读取tail做一下比较
// Push和Pop不会再同时访问链表上的同一个节点了，head和tail就可以各自用独立的mutex来保护了，增强了并发性
// Class的内部有2个锁，但是所有操作都是先对head加锁，再对tail加锁，严格保证了加锁顺序，所以不会出现死锁

namespace ZXEngine
{
	namespace Concurrent
	{
		// 支持多线程Push，多线程Pop的有锁队列
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
				// 调用线程安全的私有函数TryPopHead取出头节点，这个公有函数无需加锁
				unique_ptr<Node> oldHead = TryPopHead();
				// 如果得到的是空指针，说明队列为空，Pop失败，返回空指针
				return oldHead ? oldHead->data : shared_ptr<T>();
			}

			bool TryPop(T& value)
			{
				// 调用线程安全的私有函数TryPopHead取出头节点并填入模板数据，这个公有函数无需加锁
				unique_ptr<Node> const oldHead = TryPopHead(value);
				// 这里直接返回oldHead，如果oldHead是空指针就相当于返回了false
				return oldHead;
			}

			shared_ptr<T> WaitAndPop()
			{
				// 调用线程安全的私有函数WaitPopHead取出头节点
				// 头节点返回回来的时候，已经线程安全地从队列移除了，所以这个公有函数无需加锁
				unique_ptr<Node> const oldHead = WaitPopHead();
				// 直接取数据返回
				return oldHead->data;
			}

			void WaitAndPop(T& value)
			{
				// 调用线程安全的私有函数WaitPopHead取出头节点并填充模板数据
				unique_ptr<Node> const oldHead = WaitPopHead(value);
			}

			void Push(T newValue)
			{
				// 这两行代码不访问队列，所以无需在互斥锁保护下执行，提高了并发程度
				shared_ptr<T> newData(make_shared<T>(std::move(newValue)));
				unique_ptr<Node> p(new Node());
				// 这里为了及时解锁，加了个花括号，花括号结束时tailLock就会析构，立刻自动解锁
				{
					// Push函数只会访问tail，所以只需要在访问tail的这几行代码加锁即可保证整个队列的线程安全
					std::lock_guard<std::mutex> tailLock(tailMutex);
					tail->data = newData;
					Node* const newTail = p.get();
					tail->next = std::move(p);
					tail = newTail;
				}
				// 通知等待在dataCondition上的线程，队列里有新数据了
				// 因为一次Push只添加了一个数据，所以只需要通知一个线程来取数据
				dataCondition.notify_one();
			}

			bool Empty()
			{
				// head和tail指向相同节点时，队列为空
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
				// 获取tail指针，虽然只有一句return，但是还是要加锁
				// 否则可能会在tail所指向的节点正在被Push函数修改到一半时，将tail返回
				std::lock_guard<std::mutex> tailLock(tailMutex);
				// 返回的时候会将tail指针复制一份再返回，这个复制操作也在互斥锁的保护下进行
				// 不过好像对指针的读写操作本身就是原子的
				return tail;
			}

			unique_ptr<Node> PopHead()
			{
				// 这个函数取出当前的head，然后将head指向下一个节点
				// 函数未加锁，将加锁操作留给了调用者
				unique_ptr<Node> oldHead = std::move(head);
				head = std::move(oldHead->next);
				return oldHead;
			}

			std::unique_lock<std::mutex> WaitForData()
			{
				// 先用互unique_lock锁住head节点，并使用条件变量等待队列里有可取的数据
				// 这里使用unique_lock是因为等待过程中可能会解锁然后使线程阻塞或等待，等待结束后再次加锁
				// 而lock_guard只能在构造时加锁，析构时解锁，无法在中间解锁，所以需要可以随时手动解锁和加锁的unique_lock
				std::unique_lock<std::mutex> headLock(headMutex);
				dataCondition.wait(headLock, [&] { return head.get() != GetTail(); });
				// 将unique_lock的所有权转移给调用者，让调用者可以在同一个锁的保护下执行其它操作
				return std::move(headLock);
			}

			unique_ptr<Node> WaitPopHead()
			{
				// 等待队列被重新填入数据，WaitForData会将对head添加的互斥锁返回
				// 这里拿到之前的锁后，通过直接初始化的方式将锁转移到了本函数的headLock变量上
				// 这里没有写成headLock = WaitForData()的形式，因为这是拷贝初始化，会发生一次数据拷贝
				std::unique_lock<std::mutex> headLock(WaitForData());
				// 这里调用PopHead函数，在互斥锁的保护下将head节点后移，并获取原head节点
				return PopHead();
			}
			
			unique_ptr<Node> WaitPopHead(T& value)
			{
				// 这里的加锁操作同上
				std::unique_lock<std::mutex> headLock(WaitForData());
				// 在互斥锁的保护下读取head节点的数据
				value = std::move(*head->data);
				// 然后调用PopHead函数，在互斥锁的保护下将head节点后移，并获取原head节点
				return PopHead();
			}

			unique_ptr<Node> TryPopHead()
			{
				// 先加对head节点加锁
				std::lock_guard<std::mutex> headLock(headMutex);
				// 如果队列为空，返回空指针
				if (head.get() == GetTail())
				{
					return unique_ptr<Node>();
				}
				// 否则调用PopHead函数，在互斥锁的保护下将head节点后移，并获取原head节点
				return PopHead();
			}

			unique_ptr<Node> TryPopHead(T& value)
			{
				// 先加对head节点加锁
				std::lock_guard<std::mutex> headLock(headMutex);
				// 如果队列为空，返回空指针
				if (head.get() == GetTail())
				{
					return unique_ptr<Node>();
				}
				// 队列不为空时，在锁的保护下读取head节点的数据
				value = std::move(*head->data);
				// 然后调用PopHead函数，也锁的保护下将head节点后移，并获取原head节点
				return PopHead();
			}
		};
	}
}