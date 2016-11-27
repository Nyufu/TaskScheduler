#pragma once

#include "config.h"
#include <stdexcept>
#include <memory>

template<typename Ty>
struct ReferenceReturnValue {
	Ty& Get() {
		if (IsValid())
			return (Get(STD nothrow));
		throw STD runtime_error("Null dereference");
	}

	Ty& Get(STD nothrow_t) noexcept {
		return (*value);
	}

	void Set(Ty&& _value) noexcept {
		value = STD addressof(_value);
	}

	bool IsValid() const noexcept {
		return (value != nullptr);
	}

protected:
	STD add_pointer_t<Ty> value = nullptr;
};

template<typename Ty>
struct NonReferenceReturnValue {
public:
	Ty& Get() noexcept {
		return (Get(STD nothrow));
	}

	Ty& Get(STD nothrow_t) noexcept {
		return (*((Ty *)&value));
	}

	void Set(Ty&& _value)
#ifdef NDEBUG
		noexcept
#endif
	{
#ifdef _DEBUG
		if (assigned)
			throw STD runtime_error("Do not set 2 or more times the NonReferenceReturnValue's value.");
#endif
		assigned = (::new ((void*)&value) Ty(STD forward<Ty>(_value)) != nullptr);
	}

	bool IsValid() const noexcept {
		return assigned;
	}

	~NonReferenceReturnValue() noexcept {
		if (IsValid())
			Get().~Ty();
	}

protected:
	STD aligned_union_t<1, Ty> value = { 0 };
	bool assigned = false;
};

template<typename Ty>
struct AsyncReturnValueWrapper : public STD conditional_t<STD is_reference_v<Ty>, ReferenceReturnValue<Ty>, NonReferenceReturnValue<Ty>> {
};