#pragma once

#include <algorithm>
#include "Utilities.h"

namespace Focus::Concurency::Internal {

struct Fiber;

[[noreturn]] void FiberEntry(void* /*rcx*/, void* /*rdx*/, void* /*r8*/, void* /*r9*/, Fiber* fiber);

constexpr size_t SizeOfFastcallArguments = sizeof(void*) * 4;

constexpr size_t SizeOfFiberInternalFuncsArgs = ::std::max((sizeof(void*) * 4) + sizeof(Fiber*), SizeOfFastcallArguments);
constexpr size_t OffsetOfFiberInInternalFuncsArgs = sizeof(void*) * 4;

constexpr size_t SizeOfFiberInternalFuncsReserved = Utils::Align(SizeOfFiberInternalFuncsArgs, 0x10, Utils::AlignWay::Up); // 16 byte alignment.

}