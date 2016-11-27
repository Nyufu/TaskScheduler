#pragma once

#include "config.h"
#include <intrin.h>
#include <utility>

template<class Ty>
struct RefCountedObject;

template<class Ty, typename Enable = void>
class RefCountedObjectBase {
	static_assert(STD is_class_v<Ty>, "The Ty can only be class or struct.");
};

template<class Ty>
class RefCountedObjectBase<Ty, STD enable_if_t<STD is_class_v<Ty>>> : public Ty {
public:
	typedef Ty Base;

public:
	template<typename... Args>
	RefCountedObjectBase(Args&&... args) noexcept
		: Base{ STD forward<Args>(args)... } {
	}

	RefCountedObjectBase(const RefCountedObjectBase&) = delete;
	RefCountedObjectBase(RefCountedObjectBase&&) = delete;

	RefCountedObjectBase& operator=(const RefCountedObjectBase&) = delete;
	RefCountedObjectBase& operator=(RefCountedObjectBase&&) = delete;

protected:
	void Incref() noexcept {
		_InterlockedIncrement16(&uses);
	}

	void Decref() noexcept {
		if (_InterlockedDecrement16(&uses) == 0)
			DeleteThis();
	}

private:
	void DeleteThis() const noexcept {
		::delete this;
	}

private:
	volatile short uses = 1;

protected:
	friend RefCountedObject<Ty>;
};

struct AllocateTokenType {
};

constexpr AllocateTokenType AllocateToken{};

template<class Ty>
struct RefCountedObject {
public:
	typedef RefCountedObject<Ty> MyTy;
	typedef RefCountedObjectBase<Ty> PtrTy;
	typedef typename PtrTy::Base ValueTy;

public:
	RefCountedObject() noexcept = default;

	template<typename... Args>
	RefCountedObject(AllocateTokenType, Args&&... args) noexcept
		: ptr{ ::new (STD nothrow) PtrTy{ STD forward<Args>(args)... } } {
	}

	RefCountedObject(const MyTy& other) noexcept : ptr{ other.ptr } {
		AddRef();
	}

	RefCountedObject(MyTy&& right) noexcept : ptr{ right.ptr } {
		right.ptr = nullptr;
	}

	MyTy& operator=(const MyTy& other) noexcept {
		if (ptr != other.ptr) {
			RemoveRef();
			ptr = other.ptr;
			AddRef();
		}

		return (*this);
	}

	MyTy& operator=(MyTy&& right) noexcept {
		STD swap(ptr, right.ptr);
		return (*this);
	};

	~RefCountedObject() noexcept {
		RemoveRef();
	}

	void AddRef() noexcept {
		if (ptr)
			ptr->Incref();
	}

	void RemoveRef() noexcept {
		if (ptr)
			ptr->Decref();
	}

	PtrTy* operator->() noexcept {
		return (ptr);
	}

	const PtrTy* operator->() const noexcept {
		return (ptr);
	}

	STD add_lvalue_reference_t<ValueTy> operator*() noexcept {
		return (*ptr);
	}

	STD add_lvalue_reference_t<const ValueTy> operator*() const noexcept {
		return (*ptr);
	}

	explicit operator bool() const noexcept {
		return (ptr != nullptr);
	}

protected:
	PtrTy* ptr = nullptr;
};