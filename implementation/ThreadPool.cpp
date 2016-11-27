#include "stdafx.h"
#include "ThreadPool.h"
#include "Allocator.h"
#include "Fiber.h"
#include "Task.h"
#include "ThreadEntry.h"
#include "Utilities.h"
#include <vector>

namespace Focus::Concurrency::Internal {

ThreadPool::ThreadPool(FiberPool* fiberPool, unsigned long numberOfThreads) noexcept
	: threadHandles{ numberOfThreads }
	, threadPoolPtr{ Allocator<Fiber>::Allocate<decltype(threadPoolPtr)>(numberOfThreads) } {
	assert(threadPoolPtr);

	AddThreadToPool(RefCountedObject<ThreadArgumentPassingContainer>{AllocateToken, fiberPool, this, numberOfThreads, 0u}, 0u);
}

ThreadPool::~ThreadPool()  noexcept {
	STD call_once(shutdownFlag, &ThreadPool::ShutdownThreadsImpl, this);

	Allocator<Fiber>::DeAllocate(threadPoolPtr);
}

void ThreadPool::ShutdownThreads() noexcept {
	STD call_once(shutdownFlag, &ThreadPool::ShutdownThreadsImpl, this);
}

void ThreadPool::AddThreadToPool(RefCountedObject<ThreadArgumentPassingContainer> threadArg, uint32_t threadIndex) noexcept {
	threadArg.AddRef();

	DWORD id = 0;
	threadHandles[threadIndex] = ::CreateThread(nullptr, 0x1000ull, Internal::ThreadEntry, static_cast<LPVOID>(&*threadArg), 0ul, &id);

	threads.insert({ id, &(static_cast<Fiber*>(threadPoolPtr)[threadIndex]) });
}

void ThreadPool::ShutdownThreadsImpl() noexcept {
	ExitProcess(0);

	//const auto numberOfThreads = threadHandles.size();
	//
	//STD vector<HANDLE> handles;
	//handles.reserve(numberOfThreads);
	//
	//while (numberOfThreads != threads.size())
	//	;
	//
	//for (auto& thread : threadHandles) {
	//	handles.push_back(thread);
	//	CreateTask([] {
	//		::ExitThread(0);
	//	});
	//}
	//
	//for (DWORD result = WAIT_FAILED; result == WAIT_OBJECT_0; result = ::WaitForMultipleObjects((DWORD)numberOfThreads, handles.data(), TRUE, 0))
	//	;
}

}