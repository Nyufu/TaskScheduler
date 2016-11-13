#pragma once

#include "FiberPool.h"
#include "WinAPI.h"

#define GetFiberPool_()	          (reinterpret_cast< ::Focus::Concurency::Internal::FiberPool* >( reinterpret_cast<PPEB>(__readgsqword(FIELD_OFFSET(TEB, ProcessEnvironmentBlock)))->pContextData))
#define GetAddressofFiberPool_()  (reinterpret_cast< ::Focus::Concurency::Internal::FiberPool**>(&reinterpret_cast<PPEB>(__readgsqword(FIELD_OFFSET(TEB, ProcessEnvironmentBlock)))->pContextData))