#pragma once

#include "config.h"
#include <intrin.h>
#include <utility>

template<class _Ty>
struct RefCountedObject;

template<class _Ty, class Enable = void>
class RefCountedObjectBase {
	static_assert(_STD is_class_v<_Ty>, "The _Ty can only be class or struct.");
};

#pragma warning( push )
#pragma warning(disable : 4820)

template<class _Ty>
class RefCountedObjectBase<_Ty, _STD enable_if_t<_STD is_class_v<_Ty>>> : public _Ty {
public:
	typedef _Ty _Base;

public:
	template<class ...Args>
	__forceinline RefCountedObjectBase(Args... args) noexcept
		: _Base{ _STD forward<Args>(args)... } {
		//static_assert(_STD is_nothrow_constructible_v<_Base, Args...>, "The RefCountedObject now suppports only nothrow constructible _Ty ");
	}

	RefCountedObjectBase(const RefCountedObjectBase& _Other) = delete;
	RefCountedObjectBase(RefCountedObjectBase&& _Other) = delete;

	RefCountedObjectBase& operator=(const RefCountedObjectBase& _Other) = delete;
	RefCountedObjectBase& operator=(RefCountedObjectBase&& _Other) = delete;

protected:
	__forceinline void Incref() noexcept {
		_InterlockedIncrement16(&uses);
	}

	__forceinline void Decref() noexcept {
		if (_InterlockedDecrement16(&uses) == 0)
			DeleteThis();
	}

private:
	__forceinline void DeleteThis() const noexcept {
		::delete this;
	}

private:
	volatile short uses = 1;

protected:
	friend RefCountedObject<_Ty>;
};

#pragma warning( pop )

struct AllocateTokenType {
};

constexpr AllocateTokenType AllocateToken{};

template<class _Ty>
struct RefCountedObject {
public:
	typedef RefCountedObject<_Ty> _Myt;
	typedef RefCountedObjectBase<_Ty> _Ptrt;
	typedef typename _Ptrt::_Base _Valuet;

public:
	RefCountedObject() noexcept = default;

	template<class ...Args>
	__forceinline RefCountedObject(AllocateTokenType, Args... args) noexcept
		: ptr{ ::new (_STD nothrow) _Ptrt{ _STD forward<Args>(args)... } } {
	}

	__forceinline RefCountedObject(const _Myt& _Other) noexcept : ptr{ _Other.ptr } {
		if (ptr)
			ptr->Incref();
	}

	__forceinline RefCountedObject(_Myt&& _Right) noexcept : ptr{ _Right.ptr } {
		_Right.ptr = nullptr;
	}

	__forceinline _Myt& operator=(const _Myt& _Other) noexcept {
		if (ptr != _Other.ptr) {
			if (ptr)
				ptr->Decref();

			ptr = _Other.ptr;

			if (ptr)
				ptr->Incref();
		}

		return (*this);
	}

	__forceinline _Myt& operator=(_Myt&& _Right) noexcept {
		_STD swap(ptr, _Right.ptr);
		return (*this);
	};

	__forceinline ~RefCountedObject() noexcept {
		if (ptr)
			ptr->Decref();
	}

	__forceinline void AddRef() noexcept {
		if (ptr)
			ptr->Incref();
	}

	__forceinline void RemoveRef() noexcept {
		if (ptr)
			ptr->Decref();
	}

	__forceinline _Ptrt* GetBase() noexcept {
		return (ptr);
	}

	__forceinline _Ptrt* operator->() noexcept {
		return (ptr);
	}

	__forceinline const _Ptrt* operator->() const noexcept {
		return (ptr);
	}

	__forceinline _STD add_lvalue_reference_t<_Valuet> operator*() noexcept {
		return (*ptr);
	}

	__forceinline _STD add_lvalue_reference_t<const _Valuet> operator*() const noexcept {
		return (*ptr);
	}

	__forceinline explicit operator bool() const noexcept {
		return (ptr != nullptr);
	}

protected:
	_Ptrt* ptr = nullptr;
};