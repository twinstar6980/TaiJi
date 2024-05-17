#pragma once

#include "../../../Type.hpp"
#include "../../../TypeUtil.hpp"
#include "../../../Type/Str.hpp"

namespace $TJ::$Util::$ResPkg::$PAK::$Struct {

	struct Header {

		static inline constexpr Sz kSz = 8;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // magic
			OSD(0x4, 4), // ver
		};

		IU32 $magic;
		IU64 $ver;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $magic,
				& $ver,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct ResInfo {

		StrBlock<IU8> $path;
		IU32 $sz;
		IU64 $time;

		auto sz() const {
			return $path.sz() + kTSz<IU32> +kTSz<IU64>;
		}

		auto & read(ByteStream & t) {
			$path.read(t);
			t >> $sz >> $time;
			return thiz;
		}
		auto & write(ByteStream & t) const {
			$path.write(t);
			t << $sz << $time;
			return thiz;
		}

	};

	namespace $EndFlag {

		inline constexpr Byte kNext(0x00);
		inline constexpr Byte kStop(0x80);

	}

}
