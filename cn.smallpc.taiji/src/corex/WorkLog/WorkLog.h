# pragma once

# include "../Type.h"

# include <stdexcept>

namespace $TJ::$WorkLog {

	using Exception = std::exception;

	/*
	struct LogItem {
		Ch * _content;
		Bool _newLine;
		Ch * _tag;
	};
	struct WorkLog {
		std::vector<std::string> logs;

		auto push(std::string const & log) {
			logs.push_back(log);
			return;
		}
		auto push(char const * log) {
			push(std::string(log));
			return;
		}
		auto saveAs(std::string const & fn) {
			std::ofstream of(fn, std::ios::out);
			for (auto & log : logs)
				of << log << '\n';
			of.close();
			return;
		}
		auto saveAs(char const * fn) {
			saveAs(std::string(fn));
			return;
		}
		auto saveAs() {
			saveAs("1");
		}
	};*/


	void wLog(char const * fmt, ...);
	void wLogLn(char const * fmt, ...);
	void wCmt(char const * fmt, ...);
	void wMsg(char const * fmt, ...);
	void wWarn(char const * fmt, ...);
	void wErr(char const * fmt, ...);
}

namespace $TJ {
	using namespace $WorkLog;
}
