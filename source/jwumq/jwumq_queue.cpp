//
//  jwumq_queue.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_queue.hpp"


JwumqQueue::JwumqQueue(int size)
{
	max_size = size;
	_quit = ATOMIC_VAR_INIT(false);
	_finished = ATOMIC_VAR_INIT(false);
}

JwumqQueue::~JwumqQueue()
{
	Clear();
}
int JwumqQueue::Size()
{
	return (int)q.size();
}
bool JwumqQueue::Empty()
{
	bool empty = true;
	if(q.size() > 0)
	{
		empty = false;
	}
	return empty;
}
void JwumqQueue::SetSize(int max_size)
{
	if(max_size <= 0)
	{
		max_size = 100;
	}
	max_size = max_size;
}
//void * JwumqQueue::Front()
//{
//	void *msg = NULL;
//	m.lock();
//	if(!q.empty())
//	{
//		msg = q.front();
//	}
//	m.unlock();
//	return msg;
//}
void * JwumqQueue::Pop(int timeout)
{
	std::unique_lock<std::mutex> lock(m);
	void *msg = nullptr;
	while (!_quit)
	{
		if (!q.empty())
		{
			//data = std::move(_queue.front());
			msg = q.front();
			q.pop();
	
			_fullQue.notify_all();
			return msg;
		}
		else if (q.empty() && _finished)
		{
			return nullptr;
		}
		else
		{
			_empty.wait(lock);
		}
	}
	return nullptr;
	
//	void *msg = NULL;
//	m.lock();
//	if(!q.empty())
//	{
//		msg = q.front();
//		q.pop();
//	}
//	m.unlock();
//	return msg;
}
bool JwumqQueue::Push(void* msg)
{
	if(Size() >= max_size)
	{
		return false;
	}
//	m.lock();
//	q.push(msg);
//	m.unlock();
//	return true;
	
	std::unique_lock<std::mutex> lock(m);
	while (!_quit && !_finished)
	{
		if (q.size() < max_size)
		{
//			q.push(std::move(data));
			q.push(msg);
			_empty.notify_all();
			return true;
		}
//		else
//		{
//		// wait的时候自动释放锁，如果wait到了会获取锁
//			_fullQue.wait(lock);
//		}
	}
	
	return false;
}

void JwumqQueue::Clear()
{
	std::unique_lock<std::mutex> lock(m);
//	m.lock();
	queue<void *> empty;
	swap(empty, q);
//	m.unlock();
}

//void * JwumqQueue::PopTry()
//{
//	void *msg = NULL;
//	if (m.try_lock())
//	{
//		if(!q.empty())
//		{
//			msg = q.front();
//			q.pop();
//		}
//		m.unlock();
//	}
//	return msg;
//}


