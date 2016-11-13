#pragma once

#include "Config.h"
#include <tuple>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
#undef new
_STD_BEGIN

#if !(defined (STD_HAS_APPLY) && STD_HAS_APPLY)

namespace detail 
{

template<class _Callable, class _Tuple, size_t... _Indices>
_CONSTEXPR14 decltype(auto) apply_impl(_Callable&& _Obj, _Tuple&& _Tpl, index_sequence<_Indices...>) 
{
	return invoke(forward<_Callable>(_Obj), get<_Indices>(forward<_Tuple>(_Tpl))...);
}

}

template<class _Callable, class _Tuple>
_CONSTEXPR14 decltype(auto) apply(_Callable&& _Obj, _Tuple&& _Tpl) 
{
	return detail::apply_impl(forward<_Callable>(_Obj), forward<_Tuple>(_Tpl),
		make_index_sequence<tuple_size_v<decay_t<_Tuple>>>{});
}

#endif

#if !(defined (STD_HAS_TUPLE_CDR) && STD_HAS_TUPLE_CDR)

// copied from http://stackoverflow.com/a/14854294/2471710

template<class _Tuple, typename Seq>
struct tuple_cdr_impl;

template<class _Tuple, size_t _Index0, size_t... _Indices>
struct tuple_cdr_impl<_Tuple, index_sequence<_Index0, _Indices...>> 
{
	using type = tuple<tuple_element_t<_Indices, _Tuple>...>;
};

template<class _Tuple>
struct tuple_cdr
	: tuple_cdr_impl<_Tuple, make_index_sequence<tuple_size_v<_Tuple>>> 
{
};

template<class _Ty>
using tuple_cdr_t = typename tuple_cdr<_Ty>::type;

template<class _Tuple, size_t _Index0, size_t... _Indices>
_CONSTEXPR14 tuple_cdr_t<remove_reference_t<_Tuple>>
cdr_impl(_Tuple&& _Tpl, index_sequence<_Index0, _Indices...>) 
{
	return make_tuple(get<_Indices>(_Tpl)...);
}

template<class _Tuple>
_CONSTEXPR14 tuple_cdr_t<remove_reference_t<_Tuple>>
cdr(_Tuple&& _Tpl) 
{
	return cdr_impl(forward<_Tuple>(_Tpl),
		make_index_sequence<tuple_size_v<decay_t<_Tuple>>>{});
}

#endif

_STD_END
#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
