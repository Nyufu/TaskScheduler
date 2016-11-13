#include "stdafx.h"
#include "Task.h"
#include "Fiber.h"

namespace Focus::Concurency {

void Yield() noexcept {
	Internal::Yield();
}

}