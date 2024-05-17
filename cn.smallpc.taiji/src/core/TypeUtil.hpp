#pragma once

#include "./Type.hpp"
#include "./Const.hpp"

#include <type_traits>

#if defined ENABLE_CONCEPTS

#include <concepts>

#endif

namespace $TJ {

#if defined ENABLE_CONCEPTS

	template<typename T>
	concept CTIntU = requires() {
		requires(kTIsIntU<T>);
	};
	template<typename T>
	concept CTInt = requires() {
		requires(kTIsInt<T>);
	};

#else

#define CTIntU typename
#define CTInt typename

#endif

	// const-cast r-value-ref to l-value-ref
	template <typename T>
	inline auto & crv(T const & t) {
		return const_cast<T &>(t);
	}

	inline constexpr auto makeFourCC(Ch const & a, Ch const & b, Ch const & c, Ch const & d) {
		return static_cast<FourCC>(a << 0 | b << 8 | c << 16 | d << 24);
	}

	template <typename T>
	inline auto vSwp(T & a, T & b) {
		auto tmp = std::move(a);
		a = b;
		b = tmp;
		return;
	}
	template <typename T>
	inline auto vMov(T & dst, T & src, T const & dflt) {
		dst = src;
		src = dflt;
		return;
	}
	template <typename T>
	inline auto pMov(T *& dst, T *& src) {
		return vMov(dst, src, static_cast<T *>(kNullPtr));
	}

	template <typename T>
	inline auto vCalcDiff(T const & a, T const & b) -> T {
		return a > b ? a - b : b - a;
	}

	template <typename T>
	inline auto & vMin(T const & a, T const & b) {
		return a > b ? b : a;
	}
	template <typename T>
	inline auto & vMax(T const & a, T const & b) {
		return a > b ? a : b;
	}

	template <CTInt T>
	inline constexpr auto vRevEndian(T const & t) -> T {
	#if defined PLATFORM_ANDROID
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wshift-count-overflow"
	#endif
		auto const p = reinterpret_cast<Byte const *>(&t);
		if constexpr (kTSz<T> == 1)
			return p[0] << 0;
		if constexpr (kTSz<T> == 2)
			return p[0] << 8 | p[1] << 0;
		if constexpr (kTSz<T> == 4)
			return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3] << 0;
		if constexpr (kTSz<T> == 8)
			return p[0] << 56 | p[1] << 48 | p[2] << 40 | p[3] << 32 | p[4] << 24 | p[5] << 16 | p[6] << 8 | p[7] << 0;
	#if defined PLATFORM_ANDROID
	#pragma clang diagnostic pop
	#endif
	}

	template <typename T>
	inline auto getByteN(T const * const & src, Sz const & i) -> Byte {
		return reinterpret_cast<Byte const * const>(src)[i];
	}

	inline auto calcEnoughSz(Sz const & sz, Sz const & unit) {
		return static_cast<Sz>(sz % unit == kSzN ? sz : ((sz / unit + kSz1) * unit));
	}

	inline constexpr auto getCStrLen(ConstCStr const & s) {
		assert(s != kNullPtr);
		auto i = kNo1;
		while (!$ChType::isNull(s[i]))
			++i;
		return i;
	}

	template <CTIntU T>
	constexpr auto cnvCStrToBin(ConstCStr const & s, Sz const & len) {
		auto r = T(0);
		auto validCnt = kSzN;
		for (auto i = kNo1; i < len && validCnt <= kTSz<T, 8>; ++i) {
			if (s[i] == '-')
				continue;
			assert($ChType::isBinNum(s[i]));
			r <<= 1;
			if (s[i] == '1')
				r |= 1;
			++validCnt;
		}
		return r;
	}

	template <typename T1, typename T2>
	struct Pair {

		T1 $a;
		T2 $b;

		auto & set(T1 const & a, T2 const & b) {
			$a = a;
			$b = b;
			return thiz;
		}
		auto & setA(T1 const & a) {
			$a = a;
			return thiz;
		}
		auto & setB(T2 const & b) {
			$b = b;
			return thiz;
		}

		auto operator==(Pair const & t) const {
			return $a == t.$a && $b == t.$b;
		}
		auto operator!=(Pair const & t) const { return !operator==(t); }

		implicit Pair() = default;
		implicit Pair(Pair const &) = default;
		explicit Pair(T1 const & a, T2 const & b) : $a(a), $b(b) {}

	};

	template <typename T>
	using TypePair = Pair<T, T>;

	template <typename T>
	struct ExistShell : Pair<Bool, T> {

		Bool & $state;
		T & $data;

		auto & operator=(ExistShell const & t) {
			Pair<Bool, T>::operator=(t);
			return thiz;
		}

		implicit ExistShell() :
			Pair<Bool, T>(), $state(Pair<Bool, T>::$a), $data(Pair<Bool, T>::$b) {
		}
		implicit ExistShell(ExistShell const & t) :
			Pair<Bool, T>(t), $state(Pair<Bool, T>::$a), $data(Pair<Bool, T>::$b) {
		}

	};

	struct OSD {

		Sz $ofs;
		Sz $sz;

		constexpr OSD(Sz const & ofs = kNoX, Sz const & sz = kSzN) :
			$ofs(ofs), $sz(sz) {
		}

	};

}
