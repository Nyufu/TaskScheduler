#include "stdafx.h"
#include <cassert>
#include "FiberPoolInternal.h"
#include "Fiber.h"
#include "Utilities.h"

static_assert((sizeof(uint32_t) == sizeof(DWORD)) && ::std::is_unsigned_v<uint32_t> && ::std::is_unsigned_v<DWORD>, "The uint32_t and DWORD don't equal.");
static_assert(::std::is_same_v<size_t, DWORD64>, "The size_t and DWORD64 don't equal.");
static_assert(::std::is_same_v<size_t, uintptr_t>, "The size_t and uintptr_t don't equal.");

namespace Focus::Concurency::Internal {

constexpr bool _pageGuardUsed = _analyze;

constexpr TaskParameters::Priority  priorities[] = { TaskParameters::Priority::High, TaskParameters::Priority::Normal, TaskParameters::Priority::Low };
constexpr TaskParameters::StackType stackTypes[] = { TaskParameters::StackType::Large, TaskParameters::StackType::Medium, TaskParameters::StackType::Small };

void FiberPool::Initialize(const TaskScheduler::InitDesc& fiberPoolInitDesc, uint32_t numberOfThreads) {
	assert(GetFiberPool_() == nullptr && "The FiberPool alredy initialized or shit happened and a windows used our ptr storage.");

	SYSTEM_INFO systemInfo;
	::GetSystemInfo(&systemInfo);

	*(GetAddressofFiberPool_()) = ::new FiberPool(fiberPoolInitDesc, systemInfo.dwPageSize, numberOfThreads == 0 ? systemInfo.dwNumberOfProcessors : numberOfThreads);
}

#pragma warning(push)
#pragma warning(disable : 4355)

FiberPool::FiberPool(const TaskScheduler::InitDesc& fiberPoolInitDesc, uint32_t pageSize, uint32_t numberOfThreads)
	: scheduledQueues{
		{ fiberPoolInitDesc.largeTaskCount + fiberPoolInitDesc.mediumTaskCount + fiberPoolInitDesc.smallTaskCount },
		{ fiberPoolInitDesc.largeTaskCount + fiberPoolInitDesc.mediumTaskCount + fiberPoolInitDesc.smallTaskCount },
		{ fiberPoolInitDesc.largeTaskCount + fiberPoolInitDesc.mediumTaskCount + fiberPoolInitDesc.smallTaskCount } }
	, emptyQueues{
		{ fiberPoolInitDesc.largeTaskCount },
		{ fiberPoolInitDesc.mediumTaskCount },
		{ fiberPoolInitDesc.smallTaskCount } }
#if !(defined (BUSY_WAIT) && BUSY_WAIT)
	, newFiberEvent{ ::CreateEvent(nullptr, false, false, nullptr) }
#endif
	, threadPool{ this, numberOfThreads }
#if (defined (_ANALYZE) && _ANALYZE)
	, fibers{ fiberPoolInitDesc.largeTaskCount + fiberPoolInitDesc.mediumTaskCount + fiberPoolInitDesc.smallTaskCount }
#endif
{
	const size_t sizeOfGuardPage = _pageGuardUsed ? pageSize : 0;

	const size_t differentStackSizes[] = {
	  fiberPoolInitDesc.largeStackSize  ? (Utils::Align(fiberPoolInitDesc.largeStackSize,  pageSize, Utils::AlignWay::Up) + sizeOfGuardPage) : 0,
	  fiberPoolInitDesc.mediumStackSize ? (Utils::Align(fiberPoolInitDesc.mediumStackSize, pageSize, Utils::AlignWay::Up) + sizeOfGuardPage) : 0,
	  fiberPoolInitDesc.smallStackSize  ? (Utils::Align(fiberPoolInitDesc.smallStackSize,  pageSize, Utils::AlignWay::Up) + sizeOfGuardPage) : 0
	};

	const size_t stackPoolSize = (
		  differentStackSizes[0] * fiberPoolInitDesc.largeTaskCount
		+ differentStackSizes[1] * fiberPoolInitDesc.mediumTaskCount
		+ differentStackSizes[2] * fiberPoolInitDesc.smallTaskCount
		);

	stackPoolPtr = ::VirtualAlloc(nullptr, stackPoolSize, MEM_COMMIT, PAGE_READWRITE); assert(stackPoolPtr);

	const size_t fiberCounts[] = { fiberPoolInitDesc.largeTaskCount, fiberPoolInitDesc.mediumTaskCount, fiberPoolInitDesc.smallTaskCount };

	for (size_t stackTypeIndex = 0, currentFibersAddress = reinterpret_cast<uintptr_t>(stackPoolPtr); stackTypeIndex < 3; stackTypeIndex++) {
		const auto fiberCount = fiberCounts[stackTypeIndex];
		const auto currentFibersSize = differentStackSizes[stackTypeIndex];
		const auto priority = priorities[stackTypeIndex];
		const auto stackType = stackTypes[stackTypeIndex];

		for (size_t index = 0; index < fiberCount; index++, currentFibersAddress += currentFibersSize) {
			if (_pageGuardUsed) {
				DWORD oldProtect = 0;
				auto result = ::VirtualProtect(reinterpret_cast<void*>(currentFibersAddress), sizeOfGuardPage, PAGE_NOACCESS, &oldProtect); assert(result); UNUSED(result);
			}

			auto stackLimit = currentFibersAddress + sizeOfGuardPage;
			auto stackBase = currentFibersAddress + currentFibersSize - sizeof(Fiber); assert((stackBase & 0x0F) == 0); // stackBase must be aligned on a 16-byte boundary

			auto addressOfFiber = reinterpret_cast<Fiber*>(stackBase);

			addressOfFiber->StackLimit = stackLimit;
			addressOfFiber->StackBase = stackBase;
			//addressOfFiber->DeallocationStack = addressOfFiber->StackLimit;

			addressOfFiber->Priority = priority;
			addressOfFiber->StackType = stackType;

#if (defined (_ANALYZE) && _ANALYZE)
			fibers[index] = addressOfFiber;
#endif
			ResetFiber(this, addressOfFiber);
		}
	}
}

#pragma warning( pop )

FiberPool::~FiberPool() noexcept {
	threadPool.ShutdownThreads();
	*GetAddressofFiberPool_() = nullptr;
#if !(defined (BUSY_WAIT) && BUSY_WAIT)
	{
		auto result = ::CloseHandle(newFiberEvent); assert(result); UNUSED(result);
	}
#endif
	auto result = ::VirtualFree(stackPoolPtr, 0, MEM_RELEASE); assert(result); UNUSED(result);
}

Fiber* FiberPool::GetNextFiber() noexcept {
	Fiber* fiber = nullptr;
	for (;;) {
		if (scheduledQueues[(int)TaskParameters::Priority::High].try_dequeue(fiber)) {
			fiber->Priority = TaskParameters::Priority::High;
			return fiber;
		}

		if (scheduledQueues[(int)TaskParameters::Priority::Normal].try_dequeue(fiber)) {
			fiber->Priority = TaskParameters::Priority::Normal;
			return fiber;
		}

		if (scheduledQueues[(int)TaskParameters::Priority::Low].try_dequeue(fiber)) {
			fiber->Priority = TaskParameters::Priority::Low;
			return fiber;
		}

#if !(defined (BUSY_WAIT) && BUSY_WAIT)
		auto result = ::WaitForSingleObject(newFiberEvent, INFINITE); UNUSED(result);
#endif
	}
}

Fiber* FiberPool::GetEmptyFiber(TaskParameters::StackType stackType) noexcept {
	Fiber* fiber = nullptr;
	for (;;) {

		if (emptyQueues[(int)stackType].try_dequeue(fiber))
			return fiber;

		Yield();
	}
}

bool FiberPool::AddFiber(Fiber* fiber, TaskParameters::Priority priority) noexcept {
	assert(fiber);

	auto result = scheduledQueues[(int)(priority)].enqueue(fiber); assert(result);

#if !(defined (BUSY_WAIT) && BUSY_WAIT)
	result = ::SetEvent(newFiberEvent) == TRUE; assert(result);
#endif

	return result;
}

void FiberPool::RecyclingFiber(Fiber* fiber) noexcept {
	assert(fiber);

	auto result = emptyQueues[(int)(fiber->StackType)].enqueue(fiber); assert(result); UNUSED(result);
}

struct Releaser {
	~Releaser() noexcept {
		::delete GetFiberPool_();
	}
};

static Releaser releaser;
}