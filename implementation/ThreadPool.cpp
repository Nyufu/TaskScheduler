#include "stdafx.h"
#include "ThreadPool.h"
#include <mutex>
#include "Allocator.h"
#include "Fiber.h"
#include "Task.h"
#include "ThreadEntry.h"
#include "Utilities.h"

namespace Focus::Concurency::Internal {

ThreadPool::ThreadPool(FiberPool* fiberPool, unsigned long numberOfThreads) noexcept
	: threadHandles(numberOfThreads)
	, threadPoolPtr(Allocator<Fiber>::Allocate<decltype(threadPoolPtr)>(numberOfThreads)) {
	assert(threadPoolPtr);

	AddThreadToPool(RefCountedObject<ThreadArgumentPassingContainer>{AllocateToken, fiberPool, this, numberOfThreads, 0u}, 0);
}

_STD once_flag shutdownOnce;

ThreadPool::~ThreadPool()  noexcept {
	_STD call_once(shutdownOnce, &ThreadPool::ShutdownThreadsImpl, this);

	Allocator<Fiber>::DeAllocate(threadPoolPtr);
}

void ThreadPool::ShutdownThreads() noexcept {
	_STD call_once(shutdownOnce, &ThreadPool::ShutdownThreadsImpl, this);
}

void ThreadPool::AddThreadToPool(RefCountedObject<ThreadArgumentPassingContainer> threadArg, uint32_t threadIndex) noexcept {
	threadArg.AddRef();

	DWORD id = 0;
	threadHandles[threadIndex] = ::CreateThread(nullptr, 0x1000, Internal::ThreadEntry, static_cast<void*>(threadArg.GetBase()), 0, &id);

	threads.insert({ id, &(static_cast<Fiber*>(threadPoolPtr)[threadIndex]) });
}

void ThreadPool::ShutdownThreadsImpl() noexcept {
	const auto numberOfThreads = threadHandles.size();

	_STD vector<HANDLE> handles;
	handles.reserve(numberOfThreads);

	while (numberOfThreads != threads.size())
		;

	for (auto& thread : threadHandles) {
		handles.push_back(thread);
		CreateTask([] {
			::ExitThread(0);
		});
	}

	for (DWORD result = WAIT_FAILED; result == WAIT_OBJECT_0; result = ::WaitForMultipleObjects((DWORD)numberOfThreads, handles.data(), TRUE, 0))
		;
}

}