#pragma once

#include "./Common.hpp"

#include "./Platform.hpp"

#include <cstdint>
#include <type_traits>

// special user keyword

// implicit construct
#define implicit
// reference this no pointer
#define thiz (*this)
// reference function, then return thiz
#define retwrf(expr) return (expr), thiz
// getter function
#define fun_getter(name) auto & $$##name() const { return $##name; }
// setter function
#define fun_setter(name, type) auto & $$$##name(##type const & t) { $##name = t; return thiz; }
// setter function with default argument
#define fun_setter_dflt(name, type, dflt) auto & $$$##name(##type const & t = dflt) { $##name = t; return thiz; }

#define declaration
#define definition

namespace $TJ {

	using NullPtr = decltype(nullptr);

	using Void = void;

	using Bool = bool;

	using Byte = unsigned char;

	using Ch = char;
#if defined PLATFORM_ANDROID
	using Ch8 = char;
#else
	using Ch8 = char;//char8_t;
#endif
	using Ch16 = char16_t;
	using Ch32 = char32_t;
	using ChW = wchar_t;

	using IU8 = uint8_t;
	using IS8 = int8_t;

	using IU16 = uint16_t;
	using IS16 = int16_t;

	using IU32 = uint32_t;
	using IS32 = int32_t;

	using IU64 = uint64_t;
	using IS64 = int64_t;

	using F32 = float;
	using F64 = double;

	using Sz = IU32;
	using SSz = IS32;

	using FourCC = IU32;

	using CStr = Ch *;
	using ConstCStr = Ch const *;

	using Size = Sz;
	using Char = Ch;

	// TT : TT = Type-This of class , must define in class private using
	// TS : TS = Type-Super of class , must define in class private using
	// TRR : Type-Template Remove Ref
	// TRC : Type-Template Remove Const
	// TRP : Type-Template Remove Pointer

	template <typename T>
	using TRR = typename std::remove_reference<T>::type;

	template <typename T>
	using TRC = typename std::remove_const<T>::type;

	template <typename T>
	using TRP = typename std::remove_pointer<T>::type;

	template <typename T>
	using TDecay = typename std::decay<T>::type;

}
