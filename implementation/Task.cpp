#include "stdafx.h"
#include "Task.h"
#include "Fiber.h"

namespace Focus::Concurrency {

void Yield() noexcept {
	Internal::Yield();
}

}