#include "../core/Entry/Shell.hpp"

extern "C" {

	__declspec(dllexport) void \u592A\u6975Test() {
		return;
	}

	__declspec(dllexport) char const * \u592A\u6975GetVerName() {
		return $TJ::$Shell::getVerName();
	}

	__declspec(dllexport) void \u592A\u6975RunEntry(unsigned const argc, char const * const * const argv, char const * const logDir) {
		$TJ::$Shell::runEntry(argc, argv, logDir);
		return;
	}

	__declspec(dllexport) void \u592A\u6975RunEntryBySpecialForWindows(unsigned const argc, char const * const * const argv) {
		$TJ::$Shell::runEntryBySpecialForWindows(argc, argv);
		return;
	}

}
