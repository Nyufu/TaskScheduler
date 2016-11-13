#pragma once
#include "Config.h"
#include <cassert>
#include <type_traits>

#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
#include "WinAPI.h"
#endif

template<class _Ty>
struct Allocator {
	_CONSTEXPR14 static _Ty* Allocate(size_t size) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		return reinterpret_cast<_Ty*>(::HeapAlloc(GetProcessHeap_(), (_debug ? HEAP_ZERO_MEMORY : 0ul), (size * sizeof(_Ty))));
#else
		return ::new (_STD nothrow) _Ty[size];
#endif
	}

	template<class _Rty>
	_CONSTEXPR14 static _Rty Allocate(size_t size) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		return reinterpret_cast<_Rty>(::HeapAlloc(GetProcessHeap_(), (_debug ? HEAP_ZERO_MEMORY : 0ul), (size * sizeof(_Ty))));
#else
		return reinterpret_cast<_Rty>(::new (_STD nothrow) _Ty[size]);
#endif
	}

	_CONSTEXPR14 static void DeAllocate(_Ty* const ptr) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto result = ::HeapFree(GetProcessHeap_(), 0ul, reinterpret_cast<void*>(ptr)); assert(result); (void)result;
#else
		::delete[] ptr;
#endif
	}

	template<class _Rty>
	_CONSTEXPR14 static void DeAllocate(_Rty const ptr) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto result = ::HeapFree(GetProcessHeap_(), 0ul, reinterpret_cast<void*>(ptr)); assert(result); (void)result;
#else
		::delete[] reinterpret_cast<_Ty*>(ptr);
#endif
	}
};