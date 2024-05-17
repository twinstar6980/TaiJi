#pragma once

#include <cassert>

#define myAssert(expr) assert(!(expr))
#define pretest(expr) myAssert(expr)
#define precond(expr) assert(expr)

#define goif if
#define elif else if
#define ifcexp if constexpr
#define goifcexp if constexpr
#define elifcexp else if constexpr
#define while_nonstop while (true)
#define for_cnt(var, init, target) for (auto var = init; var < target; ++var)
#define for_cnt_ev(var, init, target) for (var = init; var < target; ++var)
#define for_cnt_lt(var, init, target) for (auto var = init; var > target; --var)
#define for_cnt_lt_ev(var, init, target) for (var = init; var > target; --var)
#define for_iter(var, list) for (auto var : list)
#define for_citer(var, list) for (auto const var : list)
#define for_riter(var, list) for (auto & var : list)
#define for_criter(var, list) for (auto const & var : list)

#define tmpl_st(tn) template<typename tn>
#define tmpl_dt(tn1, tn2) template<typename tn1, typename tn2>

#define ternary_oper(bool_expr, result_true, result_false) ((bool_expr) ? (result_true) : (result_false))

#if defined _WIN32

#define PLATFORM_WINDOWS

#else

#define PLATFORM_LINUX

#define PLATFORM_ANDROID

# if defined __arm__
#define ABI_ARM
#define ABI_NAME "arm"
# elif defined __aarch64__
#define ABI_ARM64
#define ABI_NAME "arm64"
# elif defined __i386__
#define ABI_X86
#define ABI_NAME "x86"
# elif defined __x86_64__
#define ABI_X64
#define ABI_NAME "x64"
# else
#define ABI_UNKNOWN
#define ABI_NAME "unknown"
# endif

#endif

#define VER_ULTIMATE
//#define VER_TRIAL
//#define VER_SPECIAL

# if defined PLATFORM_ANDROID
#define DISABLE_PAM_FUNC_ON_ANDROID
#endif

// #define CORE_NAME "太極"
#define CORE_NAME "\xE5\xA4\xAA\xE6\xA5\xB5"

#define CORE_VER_CODE_MAJOR 2
#define CORE_VER_CODE_MINOR 1
#define CORE_VER_CODE_PATCH 0

#define CORE_VER_CODE { CORE_VER_CODE_MAJOR, CORE_VER_CODE_MINOR, CORE_VER_CODE_PATCH }

#define CORE_VER_NAME "2.1.0"

#if defined VER_ULTIMATE
#define CORE_VER_TYPE "ultimate"
#elif defined VER_SPECIAL
#define CORE_VER_TYPE "special"
#else
#define CORE_VER_TYPE "trial"
#endif

//#define ENABLE_COUNTRY_VALIDATE
