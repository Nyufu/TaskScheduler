#pragma once

#include "config.h"
#include <cstdint>
#include <type_traits>

namespace Focus::Utilities {

enum class AlignWay {
	Up,
	Down
};

template<typename Ty>
constexpr STD enable_if_t<!STD is_pointer<Ty>::value, Ty> Align(Ty value, STD make_unsigned_t<Ty> align = 0x10, AlignWay alignWay = AlignWay::Down) {
	return ((value + (alignWay == AlignWay::Up ? (align - 1) : 0)) & ~(align - 1));
}

template<typename Ty>
constexpr STD enable_if_t<STD is_pointer<Ty>::value, Ty> Align(Ty value, uintptr_t align = 0x10, AlignWay alignWay = AlignWay::Down) {
	return (reinterpret_cast<Ty>((reinterpret_cast<uintptr_t>(value) + (alignWay == AlignWay::Up ? (align - 1) : 0)) & ~(align - 1)));
}

template<typename... Types>
struct Tuple {
};

template<typename Ty>
struct Tuple<Ty> {
};

template<typename Ty, typename Types>
struct TupleCat_impl;

template<typename Ty, typename... Types>
struct TupleCat_impl<Ty, Tuple<Types...>> {
	using type = Tuple<Ty, Types...>;
};

template<typename Ty, typename Types>
using TupleCat = typename TupleCat_impl<Ty, Types>::type;

}
