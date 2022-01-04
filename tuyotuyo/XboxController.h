#pragma once
#if !_WINDLL
#include "CrossFunc.h"
#include "ViGEm/Client.h"
#include <vector>
#include <queue>
#include <Xinput.h>
#include <thread>
#include <condition_variable>
#include <mutex>

using std::queue;
using std::pair;

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
	SafeQueue(void)
		: q()
		, m()
		, c()
	{}

	~SafeQueue(void)
	{}

	// Add an element to the queue.
	void enqueue(T t)
	{
		std::lock_guard<std::mutex> lock(m);
		q.push(t);
		c.notify_one();
	}

	// Get the "front"-element.
	// If the queue is empty, wait till a element is avaiable.
	T dequeue(void)
	{
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
		{
			// release lock as long as the wait and reaquire it afterwards.
			c.wait(lock);
		}
		T val = q.front();
		q.pop();
		return val;
	}

	bool empty(void)
	{
		return q.empty();
	}

private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};

class XboxController
{
private:
	bool isThreadOn;
	int inputFrame;
	int lastInputFrame;
	bool isSucceedFirstSucceed;
	std::mutex isSucceedFirstSucceed_mutex;
	std::thread th;
	
	PVIGEM_CLIENT client;
	PVIGEM_TARGET pad;
	int initController();
public:
	
	SafeQueue<pair<int, int>> inputQueue;
	XboxController();
	~XboxController();
	/// <summary>
	/// input action to queue
	/// </summary>
	/// <param name="input"></param>
	/// <param name="delay">1delay = 16ms</param>
	void inputAction(int input, int delay);

	void performActionThread();

	/// <summary>
	/// realAction in queue
	/// </summary>
	void performAction();
	void isReadyToInputTrue();
	void isReadyToInputFalse();
};
#endif
