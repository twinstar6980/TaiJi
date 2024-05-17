# pragma once

# include "./Type.h"

# include <ctime>

namespace $TJ {
	using TimeInt = time_t;
	using TimeStruct = tm;
	using ClockInt = clock_t;

	inline auto getCurTimeInt() { return static_cast<TimeInt>(time(kNullPtr)); }
	inline auto getCurClockInt() { return static_cast<ClockInt>(clock()); }

	inline auto getCurTimeToSzI() { return static_cast<SzI>(getCurTimeInt()); }
}
