#pragma once

#define STD				::std::
#define UNUSED(expr)	{ (void)expr; }

#include <crtversion.h>
#include <yvals.h>

#ifdef _MSC_FULL_VER
#	if (_VC_CRT_BUILD_VERSION < 24322)
#		define STD_HAS_IS_CALLABLE 0
#	else
#		define STD_HAS_IS_CALLABLE 1
#	endif

#	if (_VC_CRT_BUILD_VERSION < 24603)
#		define STD_HAS_APPLY 0
#	else
#		define STD_HAS_APPLY 1
#	endif
#else
#endif

#define FAST_WINAPI_CALL	1
#define WINAPI_ALLOCATOR	0
#define BUSY_WAIT			0

#if defined(_DEBUG)
#	define _ANALYZE			1
#endif

constexpr int _debug =
#if defined(_DEBUG)
1;
#else
0;
#endif

constexpr int _analyze =
#if defined(_ANALYZE)
1;
#else
0;
#endif

constexpr int busy_wait =
#if defined(BUSY_WAIT)
1;
#else
0;
#endif
