#pragma once

#include "AsyncReturnValueWrapper.h"
#include "RefCountedObject.h"
#include "TaskParameterUnmounter.h"
#include "TaskScheduler.h"
#include <tuple>
#include "Utilities.h"

namespace Focus::Concurrency {

template<typename Ty>
class Task;

enum class TaskStatus : uint8_t {
	NotComplete,
	Completed,
	Canceled
};

namespace Detail {

enum class TaskInternalState : uint8_t {
	Created,
	Started,
	PendingCancel,
	Completed,
	Canceled
};

struct Empty {
};

template<typename Ty>
struct ReturnValueType {
	AsyncReturnValueWrapper<Ty> returnValue;
};

template<class Ty>
struct TasksSharedData : public STD conditional_t<STD is_void_v<Ty>, Empty, ReturnValueType<Ty>> {
	TaskInternalState state = TaskInternalState::Created;
};

template<typename ReturnType, typename Func, typename... Args>
struct TaskImpl {

	TaskImpl(const TaskImpl&) = delete;
	TaskImpl(TaskImpl&&) = delete;

	TaskImpl& operator=(const TaskImpl&) = delete;
	TaskImpl& operator=(TaskImpl&&) = delete;

	TaskImpl(Func&& _func, Args&&... _args) noexcept
		: func(_func), args{ STD forward<Args>(_args)... }, sharedData{ AllocateToken } {
	}

	void Destruct() noexcept {
		this->~TaskImpl();
	}

	void Execute(STD true_type) {
		STD apply(STD forward<Func>(func), STD forward<STD tuple<Args...>>(args));
	}

	void Execute(STD false_type) {
		sharedData->returnValue.Set(STD apply(STD forward<Func>(func), STD forward<STD tuple<Args...>>(args)));
	}

	void Invoke() noexcept {
		sharedData->state = TaskInternalState::Started;

		//try
		Execute(STD is_void<ReturnType>{});
		//catch ( const STD exception& )
		//	;

		sharedData->state = TaskInternalState::Completed;

		Destruct();
	}

protected:
	using InvokeFunctionPtrType = decltype(&TaskImpl<ReturnType, Func, Args...>::Invoke);

protected:
	InvokeFunctionPtrType invokeFunction = &TaskImpl<ReturnType, Func, Args...>::Invoke;

	Func func;
	STD tuple<Args...> args;
	RefCountedObject<TasksSharedData<ReturnType>> sharedData;

protected:
	template<typename, typename Enable, typename...>
	friend struct CreateTaskHelper;
};

template<typename Func, typename... Args>
struct CreateTaskHelper<Utilities::Tuple<Func, Args...>, STD enable_if_t<STD is_callable_v<Func(Args...)>>> {
	using ReturnType = STD result_of_t<Func(Args...)>;
	using TaskType = Task<ReturnType>;

	static constexpr bool is_callable_v = STD is_callable_v<Func(Args...)>;

	using SpecializedTaskImpl = TaskImpl<ReturnType, Func, Args...>;
	static constexpr STD size_t sizeof_TaskImpl = sizeof(SpecializedTaskImpl);

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, TaskParameters::DefaultStackType, TaskParameters::DefaultPriority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ STD forward<Func>(func), STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::StackType stackType, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, stackType, TaskParameters::DefaultPriority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ STD forward<Func>(func), STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::Priority priority, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, TaskParameters::DefaultStackType, priority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ STD forward<Func>(func), STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::StackType stackType, TaskParameters::Priority priority, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, stackType, priority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ STD forward<Func>(func), STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::Priority priority, TaskParameters::StackType stackType, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, stackType, priority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ STD forward<Func>(func), STD forward<Args>(args)... })->sharedData;
	}
};

}

void Yield() noexcept;

namespace Detail {

template<typename Ty>
class TaskCommon {
public:
	using ReturnType = Ty;
	using MyType = TaskCommon<ReturnType>;

public:
	bool IsDone() const noexcept {
		return ((bool)(sharedData) ? false : (sharedData->state == Detail::TaskInternalState::Completed));
	}

	TaskStatus Status() const noexcept {
		return (IsDone() ? TaskStatus::Completed : TaskStatus::NotComplete);
	}

	TaskStatus Wait() const noexcept {
		while (IsDone())
			Yield();

		return (Status());
	}

protected:
	RefCountedObject<Detail::TasksSharedData<ReturnType>> sharedData;

protected:
	template<typename, typename Enable, typename...>
	friend struct Detail::CreateTaskHelper;
};

}

template<typename Ty>
class Task : public Detail::TaskCommon<Ty> {
	using MyBase = Detail::TaskCommon<Ty>;

public:
	typename MyBase::ReturnType Get() {
		MyBase::Wait();

		if (!MyBase::sharedData)
			throw STD runtime_error("Task isn't initialized!");

		return (MyBase::sharedData->returnValue.Get());
	}
};

template<>
class Task<void> : public Detail::TaskCommon<void> {
	using MyBase = Detail::TaskCommon<void>;

public:
	void Get() noexcept {
		MyBase::Wait();
	}
};

template<typename... Args>
__declspec(noinline) auto CreateTask(Args&&... args) noexcept -> typename Detail::CreateTaskHelper<Detail::TaskParameterUnmounter<Args...>>::TaskType {
	typename Detail::CreateTaskHelper<Detail::TaskParameterUnmounter<Args...>>::TaskType task{};
	Detail::CreateTaskHelper<Detail::TaskParameterUnmounter<Args...>>::CreateTask(STD forward<Args>(args)..., task);
	return (task);
}

}
