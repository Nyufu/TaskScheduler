#pragma once

namespace Focus::Concurrency::TaskParameters {

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

static constexpr StackType DefaultStackType = StackType::Medium;
static constexpr Priority  DefaultPriority = Priority::Normal;

}