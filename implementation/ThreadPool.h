#pragma once

#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include "RefCountedObject.h"

namespace Focus::Concurency::Internal {

struct Fiber;
class FiberPool;

class ThreadPool {
public:
	struct ThreadArgumentPassingContainer {
		FiberPool*			fiberPool;
		ThreadPool*			threadPool;
		uint32_t			threadCount;
		volatile uint32_t	threadIndex;
	};

private:
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;

	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

public:
	ThreadPool(FiberPool* fiberPool, unsigned long numberOfThreads) noexcept;
	~ThreadPool() noexcept;

	void ShutdownThreads() noexcept;

protected:
	void AddThreadToPool(RefCountedObject<ThreadArgumentPassingContainer>, uint32_t threadIndex) noexcept;

	void ShutdownThreadsImpl() noexcept;

protected:
	::concurrency::concurrent_unordered_map<DWORD, Fiber*>	threads;
	::concurrency::concurrent_vector<HANDLE>				threadHandles;

	void* threadPoolPtr = nullptr;
};

}