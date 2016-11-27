#include "stdafx.h"
#include "TaskScheduler.h"
#include <mutex>
#include "Fiber.h"
#include "FiberPoolInternal.h"

namespace Focus::Concurrency {

STD once_flag onceFlag;

void TaskScheduler::Initialize(const InitDesc& initDesc, uint32_t numberOfThreads) {
	STD call_once(onceFlag, Internal::FiberPool::Initialize, initDesc, numberOfThreads);
}

void TaskScheduler::Initialize(uint32_t numberOfThreads) {
	STD call_once(onceFlag, Internal::FiberPool::Initialize, InitDesc{ 512 * 1024 , 32, 32 * 1024 , 32, 4 * 1024 , 32 }, numberOfThreads);
}

TaskScheduler::TaskObject::~TaskObject() {
	GetFiberPool_()->AddFiber(static_cast<Internal::Fiber*>(data), priority);
}

void* TaskScheduler::TaskObject::GetMemory(size_t sizeOfMemmory) const {
	return Internal::GetMemory(static_cast<Internal::Fiber*>(data), sizeOfMemmory);
}

void TaskScheduler::InitTaskObject(TaskScheduler::TaskObject& taskObject, TaskParameters::StackType stackType, TaskParameters::Priority priority) {
	taskObject.data = static_cast<void*>(GetFiberPool_()->GetEmptyFiber(stackType));
	taskObject.priority = priority;
}

}