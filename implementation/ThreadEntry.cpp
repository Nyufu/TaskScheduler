#include "stdafx.h"
#include "ThreadEntry.h"
#include "FiberPool.h"
#include "Fiber.h"

namespace Focus::Concurency::Internal {

#pragma warning(push)
#pragma warning(disable : 4623 4625 4626 5026 5027)

class ThreadPoolVisibiltyHelper : public ThreadPool {
protected:
	friend DWORD WINAPI Internal::ThreadEntry(LPVOID);
};

#pragma warning(pop)

DWORD WINAPI ThreadEntry(LPVOID lpParameter) {
	Fiber* nextFiber = nullptr;

	{
		auto threadArg = (reinterpret_cast<RefCountedObject<ThreadPool::ThreadArgumentPassingContainer>&>(lpParameter));
		threadArg.RemoveRef();

		auto threadIndex = _InterlockedIncrement(&(threadArg->threadIndex)) - 1;

		if (threadIndex < threadArg->threadCount - 1)
			static_cast<ThreadPoolVisibiltyHelper*>(threadArg->threadPool)->AddThreadToPool(threadArg, threadIndex);

		nextFiber = threadArg->fiberPool->GetNextFiber();
	}

	SwitchToFocusFiber(nextFiber);
}

}