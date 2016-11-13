#pragma once
#include "Config.h"
#include <atomic>
#include "Allocator.h"

template<class _Ty, class _Alloc = Allocator<_Ty>>
class LockFreeRingBuffer {

	static_assert(_STD is_pod_v<_Ty>, "This ringbufffer stil doesn't work with non POD types.");

private:
	LockFreeRingBuffer() = delete;

	LockFreeRingBuffer(const LockFreeRingBuffer&) = delete;
	LockFreeRingBuffer(LockFreeRingBuffer&&) = delete;

	LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;
	LockFreeRingBuffer& operator=(LockFreeRingBuffer&&) = delete;

public:
	_CONSTEXPR14 LockFreeRingBuffer(uint32_t size) noexcept;
	~LockFreeRingBuffer() noexcept;

	_CONSTEXPR14 bool enqueue(_Ty value) noexcept;

	_CONSTEXPR14 bool try_dequeue(_Ty& value) noexcept;

	_CONSTEXPR14 size_t size_approx() const noexcept;

protected:
	const size_t capacity;

	_Ty* const data;

	_STD atomic_uint64_t reserver;
	_STD atomic_uint64_t last;
	_STD atomic_uint64_t first;
};

#if defined(__clang__) && __clang__
#define lzcnt64(x) __builtin_clzll(x)
#else
#define lzcnt64(x) __lzcnt64(x)
#endif

template<class _Ty, class _Alloc>
_CONSTEXPR14 LockFreeRingBuffer<_Ty, _Alloc>::LockFreeRingBuffer(uint32_t size) noexcept
	: capacity{ (2ull << lzcnt64(size)) - 1 }
	, data{ size ? _Alloc::Allocate(2ull << lzcnt64(size)) : nullptr }
	, reserver{ 0 }
	, last{ 0 }
	, first{ 0 } {
	assert((size != 0 && data != nullptr) || (size == 0 && data == nullptr));
}

template<class _Ty, class _Alloc>
LockFreeRingBuffer<_Ty, _Alloc>::~LockFreeRingBuffer() noexcept {
	_Alloc::DeAllocate(data);
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 bool LockFreeRingBuffer<_Ty, _Alloc>::enqueue(_Ty value) noexcept {
	const auto mask = capacity;

	uint64_t currentReserver = 0;

	do {
		currentReserver = reserver.load();

		if ((last.load() & mask) == ((currentReserver + 1) & mask))
			return false;

	} while (!reserver.compare_exchange_weak(currentReserver, currentReserver + 1));

	data[currentReserver & mask] = value;

	for (auto expectedFirst = currentReserver; !first.compare_exchange_weak(expectedFirst, currentReserver + 1); expectedFirst = currentReserver)
		;

	return true;
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 bool LockFreeRingBuffer<_Ty, _Alloc>::try_dequeue(_Ty& value) noexcept {
	const auto mask = capacity;
	const auto ptr = data;

	auto currentLast = last.load(_STD memory_order_acquire);

	do {
		if (first.load(_STD memory_order_acquire) == currentLast)
			return false;

		value = ptr[currentLast & mask];

	} while (!last.compare_exchange_weak(currentLast, currentLast + 1, _STD memory_order_release, _STD memory_order_relaxed));

	return true;
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 size_t LockFreeRingBuffer<_Ty, _Alloc>::size_approx() const noexcept {
	return first.load(_STD memory_order_relaxed) - last.load(_STD memory_order_relaxed);
}
