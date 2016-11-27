#pragma once

#include "WinAPI.h"

namespace Focus::Concurrency::Internal {

DWORD WINAPI ThreadEntry(LPVOID lpParameter);

}
