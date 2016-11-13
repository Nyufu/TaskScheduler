#include "stdafx.h"
#include "FiberEntry.h"
#include "Fiber.h"
#include "FiberPoolInternal.h"

namespace Focus::Concurency::Internal {

struct TaskImplClone {
	using MemberFunctionPtrType = void (TaskImplClone::*)();

	MemberFunctionPtrType invokeFunction;
};

void FiberEntry(void* /*rcx*/, void* /*rdx*/, void* /*r8*/, void* /*r9*/, Fiber* fiber) {
	assert(fiber);

	auto execuableObject = static_cast<TaskImplClone*>(fiber->AddressOfExecuableObject);
	_STD invoke(execuableObject->invokeFunction, execuableObject);

	ResetFiber(fiber);

	SwitchToFocusFiber(GetFiberPool_()->GetNextFiber());
}

}