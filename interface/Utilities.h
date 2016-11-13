#pragma once

#include "config.h"
#include <type_traits>

#define UNUSED(expr) { (void)expr; }

namespace Focus::Utils {

enum class AlignWay {
	Up,
	Down
};

template<typename _Ty>
constexpr _STD enable_if_t<!_STD is_pointer<_Ty>::value, _Ty> Align(_Ty value, _STD make_unsigned_t<_Ty> align = 0x10, AlignWay alignWay = AlignWay::Down) {
	return ((value + (alignWay == AlignWay::Up ? (align - 1) : 0)) & ~(align - 1));
}

template<typename _Ty>
constexpr _STD enable_if_t<_STD is_pointer<_Ty>::value, _Ty> Align(_Ty value, uintptr_t align = 0x10, AlignWay alignWay = AlignWay::Down) {
	return (reinterpret_cast<_Ty>((reinterpret_cast<uintptr_t>(value) + (alignWay == AlignWay::Up ? (align - 1) : 0)) & ~(align - 1)));
}

template<typename... _Types>
struct Tuple {
};

template<typename _Ty>
struct Tuple<_Ty> {
};

template<typename _Ty, typename _Types>
struct TupleCat_impl;

template<typename _Ty, typename... _Types>
struct TupleCat_impl<_Ty, Tuple<_Types...>> {
	using type = Tuple<_Ty, _Types...>;
};

template<typename _Ty, typename _Types>
using TupleCat = typename TupleCat_impl<_Ty, _Types>::type;

}
