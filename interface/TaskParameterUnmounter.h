#pragma once

#include "TaskParameters.h"
#include "Utilities.h"

namespace Focus::Concurency::Detail {

template<class _Ty, class... _Args>
struct TaskParameterUnmounter_impl {
	using type = Utils::TupleCat<_Ty, typename TaskParameterUnmounter_impl<_Args...>::type>;
};

template<class _Ty>
struct TaskParameterUnmounter_impl<_Ty> {
	using type = Utils::Tuple<_Ty>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::StackType> {
	using type = Utils::Tuple<>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::Priority> {
	using type = Utils::Tuple<>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::StackType, TaskParameters::Priority> {
	using type = Utils::Tuple<>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::Priority, TaskParameters::StackType> {
	using type = Utils::Tuple<>;
};

template<class... _Args>
using TaskParameterUnmounter = typename TaskParameterUnmounter_impl<_Args...>::type;

}