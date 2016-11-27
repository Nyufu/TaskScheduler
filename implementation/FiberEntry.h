#pragma once

#include "Config.h"
#include <algorithm>
#include "Utilities.h"

namespace Focus::Concurrency::Internal {

struct Fiber;

[[noreturn]] void FiberEntry(void* /*rcx*/, void* /*rdx*/, void* /*r8*/, void* /*r9*/, Fiber* fiber);

constexpr size_t SizeOfFastcallArguments = sizeof(void*) * 4;

constexpr size_t SizeOfFiberInternalFuncsArgs = STD max((sizeof(void*) * 4) + sizeof(Fiber*), SizeOfFastcallArguments);
constexpr size_t OffsetOfFiberInInternalFuncsArgs = sizeof(void*) * 4;

constexpr size_t SizeOfFiberInternalFuncsReserved = Utilities::Align(SizeOfFiberInternalFuncsArgs, 0x10, Utilities::AlignWay::Up); // 16 byte alignment.

}