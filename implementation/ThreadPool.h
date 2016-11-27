#pragma once

#include <atomic>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <mutex>
#include "RefCountedObject.h"

namespace Focus::Concurrency::Internal {

struct Fiber;
class FiberPool;

class ThreadPool {
protected:
	struct ThreadArgumentPassingContainer {
		FiberPool*			fiberPool;
		ThreadPool*			threadPool;
		const uint32_t		threadCount;
		STD atomic_uint32_t	threadIndex;

		ThreadArgumentPassingContainer(FiberPool* fiberPool_, ThreadPool* threadPool_, uint32_t threadCount_, uint32_t threadIndex_)
			: fiberPool{ fiberPool_ }, threadPool{ threadPool_ }, threadCount{ threadCount_ }, threadIndex{ threadIndex_ } {
		}
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

	STD once_flag shutdownFlag;
};

}