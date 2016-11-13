#include "stdafx.h"
#include "Task.h"

#include <iostream>
#include <thread>
#include "TaskScheduler.h"

using namespace std::chrono_literals;

namespace BuildTest {
namespace FLMOIF {

int foo() {
	std::cout << "valami 3\n";
	return 5;
}

void Test() {
	auto task = Focus::Concurency::CreateTask(foo);

	std::cout << "valami\n" << task.Get();;
	Focus::Concurency::Yield();
	std::cout << "valami 2\n";
}

void BuildTestMethod() {
	Focus::Concurency::TaskScheduler::Initialize(1);

	auto event_ = ::CreateEvent(nullptr, false, false, nullptr);

	auto task = Focus::Concurency::CreateTask([&](int k, auto f) {
		std::cout << k << f << "\n";
		Test();
		SetEvent(event_);
		return 5;
	}, 55, "char dfg", Focus::Concurency::TaskParameters::Priority::Normal, Focus::Concurency::TaskParameters::StackType::Large);

	::WaitForSingleObject(event_, INFINITE);
}

}
}

int main() {
	BuildTest::FLMOIF::BuildTestMethod();

	return 0;
}