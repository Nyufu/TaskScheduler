#pragma once

#include "TaskParameters.h"

namespace Focus::Concurrency {

namespace Detail {

template<typename, class Enable = void, typename...>
struct CreateTaskHelper;

}

class TaskScheduler {
public:
	struct InitDesc {
		uint32_t largeStackSize;
		uint32_t largeTaskCount;

		uint32_t mediumStackSize;
		uint32_t mediumTaskCount;

		uint32_t smallStackSize;
		uint32_t smallTaskCount;
	};

protected:
	struct TaskObject {
		~TaskObject();
		void* GetMemory(STD size_t sizeOfMemmory) const;

		void* data;
		TaskParameters::Priority priority;
		uint8_t padding[4];
	};

public:
	static void Initialize(const InitDesc& initDesc = InitDesc{ 512 * 1024 , 32, 32 * 1024 , 32, 4 * 1024 , 32 }, uint32_t numberOfThreads = 0);
	static void Initialize(uint32_t numberOfThreads);

protected:
	static void InitTaskObject(TaskScheduler::TaskObject& taskObject, TaskParameters::StackType stackType, TaskParameters::Priority priority);

private:
	TaskScheduler() = delete;

	TaskScheduler(const TaskScheduler&) = delete;
	TaskScheduler(TaskScheduler&&) = delete;

	TaskScheduler& operator=(const TaskScheduler&) = delete;
	TaskScheduler& operator=(TaskScheduler&&) = delete;

protected:
	template<typename, class Enable, typename...>
	friend struct Detail::CreateTaskHelper;
};

}