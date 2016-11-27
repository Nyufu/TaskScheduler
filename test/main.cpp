#include "stdafx.h"
#include "Task.h"

#include <iostream>

//#pragma optimize( "", off )  
//__declspec(noinline) void dummy() {
//}
//#pragma optimize( "", on )   


namespace BuildTest {
namespace FLMOIF {

int foo() {
	std::cout << "Task 2\n";
	//dummy();
	return 5;
}

void Test() {
	auto task = Focus::Concurrency::CreateTask(foo);

	std::cout << "Task 1 Step 1.\n";
	//dummy();
	std::cout << "Task 1 will be yielded.\n";
	Focus::Concurrency::Yield();
	std::cout << "Task 1 revived.\n";
	//dummy();
	std::cout << "Task 2 result " << task.Get() << "\n";
}

void BuildTestMethod() {
	Focus::Concurrency::TaskScheduler::Initialize(4);

	auto event_ = ::CreateEvent(nullptr, false, false, nullptr);

	auto task = Focus::Concurrency::CreateTask([&](int k, auto f) {
		std::cout << "print task parameters: " << k << ", " << f << "\n";
		//dummy();
		Test();
		SetEvent(event_);
		return 5;
	}, 55, "char dfg", Focus::Concurrency::TaskParameters::Priority::Low, Focus::Concurrency::TaskParameters::StackType::Large);

	::WaitForSingleObject(event_, INFINITE);
}

}
}

int main() {
	BuildTest::FLMOIF::BuildTestMethod();

	return 0;
}