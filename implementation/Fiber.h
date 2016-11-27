#pragma once

#include <csetjmp>
#include <cstdint>
#include "TaskParameters.h"
#include "WinAPI.h"

namespace Focus::Concurrency::Internal {

typedef _JUMP_BUFFER Context;

class FiberPool;

struct _VCRT_ALIGN(16) Fiber {
	Context		Context;

	DWORD64		StackBase;						// 0x100	offset in TEB: 0x08
	DWORD64		StackLimit;						// 0x108	offset in TEB: 0x10
	//DWORD64		ActivationContextStackPointer;	// 0x110	offset in TEB: 0x2c8
	//DWORD64		DeallocationStack;				// 0x118	offset in TEB: 0x1478
	//DWORD64		FlsData;						// 0x120	offset in TEB: 0x17c8

	void*	    AddressOfExecuableObject;		// 0x128

	TaskParameters::Priority	Priority;						// 0x130
	TaskParameters::StackType	StackType;						// 0x134

	//uint8_t		padding[8];
};

static_assert((sizeof(Fiber) & 0x0F) == 0, "The size of Fiber should be multiple of 16 byte");

void InitFiberOnCurrentThread(Fiber*) noexcept;
void ResetFiber(FiberPool*, Fiber*) noexcept;
void ResetFiber(Fiber*) noexcept;
void* GetMemory(Fiber*, size_t) noexcept;

extern "C" {

	extern Fiber* _cdecl StowCurrentFiberAndGiveNew(Fiber*);

	[[noreturn]]
	extern void _cdecl SwitchToFocusFiber(Fiber*);
	extern void _cdecl SwitchToFocusFiberAndSavePrevious(Fiber*);

	extern void _cdecl Yield();

}

}

#define GetFiber() (reinterpret_cast< ::Focus::Concurrency::Internal::Fiber*>(GetCurrentFiber_()))
