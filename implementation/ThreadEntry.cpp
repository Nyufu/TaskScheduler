#include "stdafx.h"
#include "ThreadEntry.h"
#include "FiberPool.h"
#include "Fiber.h"

namespace Focus::Concurrency::Internal {

class ThreadPoolVisibiltyHelper : protected ThreadPool {
protected:
	friend DWORD WINAPI Internal::ThreadEntry(LPVOID);
};

DWORD WINAPI ThreadEntry(LPVOID lpParameter) {
	Fiber* nextFiber = nullptr;

	{
		auto threadArg = (reinterpret_cast<RefCountedObject<ThreadPoolVisibiltyHelper::ThreadArgumentPassingContainer>&>(lpParameter));
		threadArg.RemoveRef();

		for (;;) {
			auto threadIndex = threadArg->threadIndex.fetch_add(1) + 1;
			if (threadIndex >= threadArg->threadCount)
				break;

			static_cast<ThreadPoolVisibiltyHelper*>(threadArg->threadPool)->AddThreadToPool(threadArg, threadIndex);
		}

		nextFiber = threadArg->fiberPool->GetNextFiber();
	}

	SwitchToFocusFiber(nextFiber);
}

}