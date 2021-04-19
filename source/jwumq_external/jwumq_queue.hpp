//
//  jwumq_queue.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_queue_hpp
#define jwumq_queue_hpp

#include <stdio.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>

using namespace std;


class JwumqQueue
{
public:
	JwumqQueue(int size = 10000);
	~JwumqQueue(void);
	
	int Size();
//	void * Front();
	void * Pop(int timeout);
//	void * PopTry();
	bool Push(void * msg);
	void Clear();
	bool Empty();
	void SetSize(int max_size);
public:
	
private:
	queue <void *> q;
	mutex m;
	int max_size;
	
//	std::queue<T> _queue;
//	typename std::queue<T>::size_type _size_max;
	
//	mutex _mutex;
	condition_variable _fullQue;
	condition_variable _empty;
	
	atomic_bool _quit;
	atomic_bool _finished;
	
};

#endif /* jwumq_queue_hpp */
