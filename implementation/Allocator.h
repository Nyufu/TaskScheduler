#pragma once

#include "Config.h"

#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
#include "WinAPI.h"
#endif

#include <cassert>
#include <new>
#include <type_traits>

template<typename Ty, typename Enable = void>
struct Allocator {
};

template<typename Ty>
struct Allocator<Ty, STD enable_if_t<STD is_pod_v<Ty>>> {
	static Ty* Allocate(STD size_t size) {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto ptr = ::HeapAlloc(GetProcessHeap_(), (_debug ? HEAP_ZERO_MEMORY : 0ul), (size * sizeof(Ty)));
		if (ptr == nullptr)
			throw STD bad_alloc{};
		return static_cast<Ty*>(ptr);
#else
		return ::new Ty[size];
#endif
	}

	template<typename RTy, typename = STD enable_if_t<STD is_pointer_v<RTy>>>
	static RTy Allocate(STD size_t size) {
		return reinterpret_cast<RTy>(Allocate(size));
	}

	static void DeAllocate(Ty* const ptr) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto result = ::HeapFree(GetProcessHeap_(), 0ul, reinterpret_cast<void*>(ptr)); assert(result); UNUSED(result);
#else
		::delete[] ptr;
#endif
	}

	template<typename RTy, typename = STD enable_if_t<STD is_pointer_v<RTy>>>
	static void DeAllocate(RTy const ptr) noexcept {
		DeAllocate(reinterpret_cast<Ty*>(ptr));
	}
};

template<typename Ty>
struct Allocator<Ty, STD enable_if_t<!STD is_pod_v<Ty>>> {
	static Ty* Allocate(STD size_t size) {
		return ::new Ty[size];
	}

	template<typename RTy, typename = STD enable_if_t<STD is_pointer_v<RTy>>>
	static RTy Allocate(STD size_t size) {
		return reinterpret_cast<RTy>(Allocate(size));
	}

	static void DeAllocate(Ty* const ptr) noexcept {
		::delete[] ptr;
	}

	template<typename RTy, typename = STD enable_if_t<STD is_pointer_v<RTy>>>
	static void DeAllocate(RTy const ptr) noexcept {
		DeAllocate(reinterpret_cast<Ty*>(ptr));
	}
};