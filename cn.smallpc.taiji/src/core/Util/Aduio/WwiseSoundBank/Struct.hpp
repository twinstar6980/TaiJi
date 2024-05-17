#pragma once

#include "../../../Type.hpp"
#include "../../../TypeUtil.hpp"
#include "../../../Type/BlockFunc.hpp"
#include "../../../Type/Str.hpp"
#include "../../Data/Hasher/FNV.hpp"
#include "../WaveStruct.hpp"

namespace $TJ::$Util::$Audio::$WwiseSoundBank::$Struct {

	using $WaveStruct::ChunkSign;

	namespace $ChunkID {

		inline constexpr FourCC kBKHD(makeFourCC('B', 'K', 'H', 'D'));
		inline constexpr FourCC kDIDX(makeFourCC('D', 'I', 'D', 'X'));
		inline constexpr FourCC kDATA(makeFourCC('D', 'A', 'T', 'A'));
		inline constexpr FourCC kSTID(makeFourCC('S', 'T', 'I', 'D'));
		inline constexpr FourCC kSTMG(makeFourCC('S', 'T', 'M', 'G'));
		inline constexpr FourCC kHIRC(makeFourCC('H', 'I', 'R', 'C'));

	}

	namespace $ChunkIDStr {

		inline Str const kBKHD("BKHD"_r);
		inline Str const kDIDX("DIDX"_r);
		inline Str const kDATA("DATA"_r);
		inline Str const kSTID("STID"_r);
		inline Str const kSTMG("STMG"_r);
		inline Str const kHIRC("HIRC"_r);

	}

	struct BKHD_Begin {

		static inline constexpr Sz kSz = 16;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // ver
			OSD(0x4, 4), // id
		};

		IU32 $ver;
		IU32 $id;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $ver,
				& $id,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct DIDX_Sub {

		static inline constexpr Sz kSz = 12;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // id
			OSD(0x4, 4), // ofs
			OSD(0x8, 4), // sz
		};

		IU32 $id;
		IU32 $ofs;
		IU32 $sz;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $ofs,
				& $sz,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct STID_Begin {

		static inline constexpr Sz kSz = 8;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // unk_1
			OSD(0x4, 4), // itemSum
		};

		IU32 $unk_1;
		IU32 $itemSum;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $unk_1,
				& $itemSum,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct STID_Sub {

		IU32 $id;
		StrBlock<IU8> $name;

		auto sz() const {
			return  static_cast<Sz>(kTSz<IU32> +$name.sz());
		}

		auto & read(ByteStream & t) {
			t >> $id;
			$name.read(t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			t << $id;
			$name.write(t);
			return thiz;
		}

		auto & read(ByteSection const & t) {
			return read(ByteStream().ref(t).setPosBeg());
		}
		auto & write(ByteSection & t) const {
			return write(ByteStream().ref(t).setPosBeg());
		}

	};

	struct HIRC_Begin {

		static inline constexpr Sz kSz = 4;
		static inline constexpr Sz kSum = 1;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // sz
		};

		IU32 $sz;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $sz,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct HIRC_Sub_Begin {

		static inline constexpr Sz kSz = 9;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 1), // ty
			OSD(0x1, 4), // sz
			OSD(0x5, 4), // id
		};

		IU8 $ty;
		IU32 $sz;
		IU32 $id;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $ty,
				& $sz,
				& $id,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

}
