#pragma once

#include <cassert>
#include <iostream>

#include "./Ver.hpp"
#include "./Platform.hpp"
#include "./Exception.hpp"
#include "./Log/Log.hpp"

using std::cout;
using std::endl;

#if defined PLATFORM_ANDROID

#define static_assert_failed(msg) 

#else

#define static_assert_failed(msg) static_assert(false, msg)

#endif

#if defined PLATFORM_WINDOWS

//#define ENABLE_CONCEPTS
#define USE_STD_FILESYSTEM

#endif
