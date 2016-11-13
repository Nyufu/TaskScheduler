#include "stdafx.h"
#include "Fiber.h"
#include <cassert>
#include "FiberEntry.h"
#include "FiberPoolInternal.h"
#include "Utilities.h"

namespace Focus::Concurency::Internal {

void InitFiberOnCurrentThread(Fiber* fiber) noexcept {
	assert(fiber);

	auto teb = GetTeb_();

	fiber->StackBase = teb->NtTib.StackBase;
	fiber->StackLimit = teb->NtTib.StackLimit;
	//fiber->ActivationContextStackPointer = reinterpret_cast<DWORD64>(teb->ActivationContextStackPointer);
	//fiber->DeallocationStack = reinterpret_cast<DWORD64>(teb->DeallocationStack);
	//fiber->FlsData = reinterpret_cast<DWORD64>(teb->FlsData);

	teb->NtTib.FiberData = reinterpret_cast<DWORD64>(fiber);
}

void ResetFiber(FiberPool* fiberPool, Fiber* fiber) noexcept {
	assert(fiberPool);
	assert(fiber);

	fiber->Context.Rip = reinterpret_cast<DWORD64>(Internal::FiberEntry);
	fiber->Context.MxCsr = 0x1F80;	// The default MXCSR value at reset is 1F80H. http://x86.renejeschke.de/html/file_module_x86_id_151.html

	fiberPool->RecyclingFiber(fiber);
}

void ResetFiber(Fiber* fiber) noexcept {
	ResetFiber(GetFiberPool_(), fiber);
}

void* GetMemory(Fiber* fiber, size_t size) noexcept {
	assert(fiber);
	assert((fiber->StackBase & 0x0F) == 0);

	const auto addressOfExecuableObject = Utils::Align(fiber->StackBase - size); // 16 byte alignment.
	const auto addressOfFunctionArguments = addressOfExecuableObject - Internal::SizeOfFiberInternalFuncsReserved;

	const auto addressOfFiberArgument = addressOfFunctionArguments + Internal::OffsetOfFiberInInternalFuncsArgs;
	*reinterpret_cast<Fiber**>(addressOfFiberArgument) = fiber;

	fiber->Context.Rsp = addressOfFunctionArguments - 0x10;
	fiber->AddressOfExecuableObject = reinterpret_cast<void*>(addressOfExecuableObject);

	return reinterpret_cast<void*>(addressOfExecuableObject);
}

Fiber* StowCurrentFiberAndGiveNew(Fiber* fiber) {
	assert(fiber);

	GetFiberPool_()->AddFiber(fiber, fiber->Priority);

	auto nextFiber = GetFiberPool_()->GetNextFiber();

	assert(nextFiber);

	return nextFiber;
}

}