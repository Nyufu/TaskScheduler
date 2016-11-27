#pragma once

#include <vector>
#include "LockFreeRingBuffer.h"
#include "TaskScheduler.h"
#include "ThreadPool.h"

namespace Focus::Concurrency::Internal {

struct Fiber;

class FiberPool {
public:
	static void Initialize(const TaskScheduler::InitDesc& fiberPoolInitDesc, uint32_t numberOfThreads = 0);

	static void* operator new(STD size_t size);
	static void operator delete(void* ptr);

private:
	FiberPool(const TaskScheduler::InitDesc& fiberPoolInitDesc, uint32_t pageSize, uint32_t numberOfThreads);

	FiberPool(const FiberPool&) = delete;
	FiberPool(FiberPool&&) = delete;

	FiberPool& operator=(const FiberPool&) = delete;
	FiberPool& operator=(FiberPool&&) = delete;

public:
	~FiberPool() noexcept;

	Fiber* GetNextFiber() noexcept;
	Fiber* GetEmptyFiber(TaskParameters::StackType stackType = TaskParameters::StackType::Medium) noexcept;
	bool   AddFiber(Fiber* fiber, TaskParameters::Priority priority = TaskParameters::Priority::Normal) noexcept;
	void   RecyclingFiber(Fiber* fiber) noexcept;

private:
	LockFreeRingBuffer<Fiber*> scheduledQueues[3];
	LockFreeRingBuffer<Fiber*> emptyQueues[3];

#if !(defined (BUSY_WAIT) && BUSY_WAIT)
	HANDLE newFiberEvent;
#endif

	ThreadPool threadPool;

#if (defined (_ANALYZE) && _ANALYZE)
	_STD vector<Fiber*> fibers;
#endif

	void* stackPoolPtr = nullptr;
};

}