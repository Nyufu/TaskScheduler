#pragma once

#include "TaskParameters.h"
#include "Utilities.h"

namespace Focus::Concurrency::Detail {

template<typename Ty, typename... Types>
struct TaskParameterUnmounter_impl {
	using type = Utilities::TupleCat<Ty, typename TaskParameterUnmounter_impl<Types...>::type>;
};

template<typename Ty>
struct TaskParameterUnmounter_impl<Ty> {
	using type = Utilities::Tuple<Ty>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::StackType> {
	using type = Utilities::Tuple<>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::Priority> {
	using type = Utilities::Tuple<>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::StackType, TaskParameters::Priority> {
	using type = Utilities::Tuple<>;
};

template<>
struct TaskParameterUnmounter_impl<TaskParameters::Priority, TaskParameters::StackType> {
	using type = Utilities::Tuple<>;
};

template<typename... Types>
using TaskParameterUnmounter = typename TaskParameterUnmounter_impl<Types...>::type;

}