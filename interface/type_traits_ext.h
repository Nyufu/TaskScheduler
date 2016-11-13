#pragma once

#include "Config.h"
#include <type_traits>

#if !(defined (STD_HAS_IS_CALLABLE) && STD_HAS_IS_CALLABLE)

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
_STD_BEGIN

namespace detail
{

template<typename T>
using always_void = void;

template<typename Expr, typename Enable = void>
struct is_callable_impl
	: false_type
{
};

template<class _Callable, typename ...Args>
struct is_callable_impl<_Callable(Args...), always_void<result_of_t<_Callable(Args...)>>>
	: true_type
{
};

}

template<typename Expr>
struct is_callable
	: detail::is_callable_impl<Expr>
{
};

template<class _Fty>
  constexpr bool is_callable_v = is_callable<_Fty>::value;

_STD_END
#pragma warning(pop)
#pragma pack(pop)
#endif