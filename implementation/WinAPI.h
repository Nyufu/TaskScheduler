#pragma once

#define WIN32_LEAN_AND_MEAN

#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCOMM
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOGDI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOIME
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
#define NOMB
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NONLS
#define NOOPENFILE
#define NOPROFILER
#define NORASTEROPS
#define NOSCROLL
#define NOSERVICE
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOUSER
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define OEMRESOURCE

#pragma warning( push, 0 )

#include <Windows.h>
#include "ntpebteb.h"

#pragma warning( pop )
#undef Yield

#if (defined (FAST_WINAPI_CALL) && FAST_WINAPI_CALL)

#define GetCurrentFiber_()		(reinterpret_cast<PVOID>(__readgsqword(FIELD_OFFSET(NT_TIB, FiberData))))
#define GetCurrentThreadId_()	(reinterpret_cast<HANDLE>(reinterpret_cast<CLIENT_ID>(__readgsqword(FIELD_OFFSET(TEB, ClientId))).UniqueThread))
#define GetProcessHeap_()		(reinterpret_cast<HANDLE>(reinterpret_cast<PPEB>(__readgsqword(FIELD_OFFSET(TEB, ProcessEnvironmentBlock)))->ProcessHeap))
#define GetTeb_()				(reinterpret_cast<PTEB>(__readgsqword(FIELD_OFFSET(NT_TIB, Self))))

#else

#define GetCurrentFiber_()		(::GetCurrentFiber())
#define GetCurrentThreadId_()	(::GetCurrentThreadId())
#define GetProcessHeap_()		(::GetProcessHeap())
#define GetTeb_()				(reinterpret_cast<PTEB>(::NtCurrentTeb()))

#endif
