#pragma once

#include "../../../Type.hpp"
#include "../../../Type/List.hpp"

namespace $TJ::$Util::$Data::$Hasher::$FNV {

	template <typename T>
	struct FNVParm {};

	template <>
	struct FNVParm<IU32> {

		static constexpr IU32 kOffset = 2166136261;
		static constexpr IU32 kPrime = 16777619;

	};

	template <>
	struct FNVParm<IU64> {

		static constexpr IU64 kOffset = 14695981039346656037;
		static constexpr IU64 kPrime = 1099511628211;

	};

	enum class FNVType : IU8 { kFNV1, kFNV1a };

	template <typename T, FNVType type>
	inline auto hash(List<Byte> const & t) {
		auto r = FNVParm<T>::kOffset;
		if constexpr (type == FNVType::kFNV1) {
			for (auto const & e : t) {
				r *= FNVParm<T>::kPrime;
				r ^= static_cast<IU8>(e);
			}
		} else if constexpr (type == FNVType::kFNV1a) {
			for (auto const & e : t) {
				r ^= static_cast<IU8>(e);
				r *= FNVParm<T>::kPrime;
			}
		} else {
			static_assert_failed("fnv type error");
		}
		return r;
	}

	namespace $FNV1 {

		inline auto hash32(List<Byte> const & t) {
			return hash<IU32, FNVType::kFNV1>(t);
		}
		inline auto hash64(List<Byte> const & t) {
			return hash<IU64, FNVType::kFNV1>(t);
		}

	}

	namespace $FNV1a {

		inline auto hash32(List<Byte> const & t) {
			return hash<IU32, FNVType::kFNV1a>(t);
		}
		inline auto hash64(List<Byte> const & t) {
			return hash<IU64, FNVType::kFNV1a>(t);
		}

	}

}
