#include "D:/Program Files (x86)/Visual Leak Detector/include/vld.h"

#pragma comment(lib, "lib.windows.dynamic.lib")

#include <cstdlib>

#include <windows.h>

#include "../../src/lib/windows.hpp"

int main(int argc, char * argv[]) {
	::SetConsoleCP(CP_UTF8);
	::SetConsoleOutputCP(CP_UTF8);
	::\u592A\u6975RunEntryBySpecialForWindows(argc - 1, argv + 1);
	std::system("pause");
	return 0;
}
