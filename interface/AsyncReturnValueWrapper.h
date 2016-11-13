#pragma once

#include "config.h"
#include <type_traits>
#include <memory>

template< class _Ty >
struct ReferenceReturnValue {
	_CONSTEXPR14 _Ty& Get() {
		if (IsValid())
			return (Get(_STD nothrow));
		throw _STD exception("Null dereference");
	}

	_CONSTEXPR14 _Ty& Get(_STD nothrow_t) noexcept {
		return (*value);
	}

	_CONSTEXPR14 void Set(_Ty&& _value) noexcept {
		value = _STD addressof(_value);
	}

	_CONSTEXPR14 bool IsValid() const noexcept {
		return (value != nullptr);
	}

protected:
	_STD add_pointer_t< _Ty > value = nullptr;
};

#pragma warning(push)
#pragma warning(disable: 4820)

template< class _Ty >
struct NonReferenceReturnValue {
public:
	_CONSTEXPR14 _Ty& Get() noexcept {
		return (Get(_STD nothrow));
	}

	_CONSTEXPR14 _Ty& Get(_STD nothrow_t) noexcept {
		return (*((_Ty *)&value));
	}

	_CONSTEXPR14 void Set(_Ty&& _value)
#ifdef NDEBUG
		noexcept
#endif
	{
#ifdef _DEBUG
		if (assigned)
			throw _STD exception("Do not set 2 or more times the NonReferenceReturnValue's value.");
#endif
		assigned = ::new ((void*)&value) _Ty(_STD forward< _Ty >(_value)) != nullptr;
	}

	_CONSTEXPR14 bool IsValid() const noexcept {
		return assigned;
	}

	~NonReferenceReturnValue() noexcept {
		if (IsValid())
			Get().~_Ty();
	}

protected:
	_STD aligned_union_t<1, _Ty> value = { 0 };
	bool assigned = false;
};

#pragma warning(pop)

template< class _Ty >
struct AsyncReturnValueWrapper : public _STD conditional_t< _STD is_reference_v< _Ty >, ReferenceReturnValue< _Ty >, NonReferenceReturnValue< _Ty > > {
};