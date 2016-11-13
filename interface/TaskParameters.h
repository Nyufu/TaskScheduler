#pragma once

namespace Focus::Concurency {

struct TaskParameters {
	enum class StackType {
		Large,
		Medium,
		Small
	};

	enum class Priority {
		High,
		Normal,
		Low
	};

	TaskParameters() = default;

	TaskParameters(StackType stackType_)
		: stackType(stackType_) {
	}

	TaskParameters(Priority priority_)
		: priority(priority_) {
	}

	TaskParameters(StackType stackType_, Priority priority_)
		: stackType(stackType_)
		, priority(priority_) {
	}

	TaskParameters(Priority priority_, StackType stackType_)
		: stackType(stackType_)
		, priority(priority_) {
	}

	StackType stackType = DefaultStackType;
	Priority priority   = DefaultPriority;

	constexpr static StackType DefaultStackType = StackType::Medium;
	constexpr static Priority  DefaultPriority  = Priority::Normal;
};

}