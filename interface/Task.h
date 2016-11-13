#pragma once

#include "AsyncReturnValueWrapper.h"
#include "RefCountedObject.h"
#include "TaskParameterUnmounter.h"
#include "TaskScheduler.h"
#include "tuple_ext.h"
#include "type_traits_ext.h"
#include "Utilities.h"

namespace Focus::Concurency {

template<typename Type>
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

template<class _Ty>
struct ReturnValueType {
	AsyncReturnValueWrapper<_Ty> returnValue;
};

#pragma warning(push)
#pragma warning(disable: 4820)

template<class _Ty>
struct TasksSharedData : public _STD conditional_t<_STD is_void_v<_Ty>, Empty, ReturnValueType<_Ty>> {
	TaskInternalState state = TaskInternalState::Created;
};

template<typename ReturnType, typename Func, typename... Args>
struct TaskImpl {

	TaskImpl(const TaskImpl&) = delete;
	TaskImpl(TaskImpl&&) = delete;

	TaskImpl& operator=(const TaskImpl&) = delete;
	TaskImpl& operator=(TaskImpl&&) = delete;

	_CONSTEXPR14 TaskImpl(Func&& _func, Args&&... _args) noexcept
		: func(_func), args{ _STD forward<Args>(_args)... }, sharedData{ AllocateToken } {
	}

	_CONSTEXPR14 void Destruct() noexcept {
		this->~TaskImpl();
	}

	_CONSTEXPR14 void Execute(_STD true_type) {
		_STD apply(_STD forward<Func>(func), _STD forward<_STD tuple<Args...>>(args));
	}

	_CONSTEXPR14 void Execute(_STD false_type) {
		sharedData->returnValue.Set(_STD apply(_STD forward<Func>(func), _STD forward<_STD tuple<Args...>>(args)));
	}

	void Invoke() noexcept {
		sharedData->state = TaskInternalState::Started;

		//try
		Execute(_STD is_void<ReturnType>{});
		//catch ( const _STD exception& )
		//	;

		sharedData->state = TaskInternalState::Completed;

		Destruct();
	}

protected:
	using InvokeFunctionPtrType = decltype(&TaskImpl<ReturnType, Func, Args...>::Invoke);

protected:
	InvokeFunctionPtrType invokeFunction = &TaskImpl<ReturnType, Func, Args...>::Invoke;

	Func func;
	_STD tuple<Args...> args;
	RefCountedObject<TasksSharedData<ReturnType>> sharedData;

protected:
	template<typename, class Enable, typename...>
	friend struct CreateTaskHelper;
};

#pragma warning(pop)

template<typename Func, typename... Args>
struct CreateTaskHelper<Utils::Tuple<Func, Args...>, _STD enable_if_t<_STD is_callable_v<Func(Args...)>>> {
	using ReturnType = _STD result_of_t<Func(Args...)>;
	using TaskType = Task<ReturnType>;

	static constexpr bool is_callable_v = _STD is_callable_v<Func(Args...)>;

	using SpecializedTaskImpl = TaskImpl<ReturnType, Func, Args...>;
	static constexpr size_t sizeof_TaskImpl = sizeof(SpecializedTaskImpl);

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, TaskParameters::DefaultStackType, TaskParameters::DefaultPriority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ _STD forward<Func>(func), _STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::StackType stackType, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, stackType, TaskParameters::DefaultPriority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ _STD forward<Func>(func), _STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::Priority priority, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, TaskParameters::DefaultStackType, priority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ _STD forward<Func>(func), _STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::StackType stackType, TaskParameters::Priority priority, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, stackType, priority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ _STD forward<Func>(func), _STD forward<Args>(args)... })->sharedData;
	}

	static __forceinline void CreateTask(Func&& func, Args&&... args, TaskParameters::Priority priority, TaskParameters::StackType stackType, TaskType& task) noexcept {
		TaskScheduler::TaskObject taskObject;
		TaskScheduler::InitTaskObject(taskObject, stackType, priority);

		task.sharedData = (::new (taskObject.GetMemory(sizeof_TaskImpl)) SpecializedTaskImpl{ _STD forward<Func>(func), _STD forward<Args>(args)... })->sharedData;
	}
};

}

void Yield() noexcept;

namespace Detail {

template<typename Type>
class TaskCommon {
public:
	using ReturnType = Type;
	using MyType = TaskCommon<ReturnType>;

public:
	__forceinline bool IsDone() const noexcept {
		return ((bool)(sharedData) ? false : (sharedData->state == Detail::TaskInternalState::Completed));
	}

	__forceinline TaskStatus Status() const noexcept {
		return (IsDone() ? TaskStatus::Completed : TaskStatus::NotComplete);
	}

	__forceinline TaskStatus Wait() const noexcept {
		while (IsDone())
			Yield();

		return (Status());
	}

protected:
	RefCountedObject<Detail::TasksSharedData<ReturnType>> sharedData;

protected:
	template<typename, class Enable, typename...>
	friend struct Detail::CreateTaskHelper;
};

}

template<typename Type>
class Task : public Detail::TaskCommon<Type> {
	using MyBase = Detail::TaskCommon<Type>;

public:
	__forceinline typename MyBase::ReturnType Get() {
		MyBase::Wait();

		if (!MyBase::sharedData)
			throw _STD exception("Task isn't initialized!");

		return (MyBase::sharedData->returnValue.Get());
	}
};

template<>
class Task<void> : public Detail::TaskCommon<void> {
	using MyBase = Detail::TaskCommon<void>;

public:
	__forceinline void Get() noexcept {
		MyBase::Wait();
	}
};

template<typename... Args>
__declspec(noinline) auto CreateTask(Args&&... args) noexcept -> typename Detail::CreateTaskHelper<Detail::TaskParameterUnmounter<Args...>>::TaskType {
	typename Detail::CreateTaskHelper<Detail::TaskParameterUnmounter<Args...>>::TaskType task{};
	Detail::CreateTaskHelper<Detail::TaskParameterUnmounter<Args...>>::CreateTask(_STD forward<Args>(args)..., task);
	return (task);
}

}
