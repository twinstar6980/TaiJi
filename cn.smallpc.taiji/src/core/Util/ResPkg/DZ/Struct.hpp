#pragma once

#include "../../../Type.hpp"
#include "../../../TypeUtil.hpp"

namespace $TJ::$Util::$ResPkg::$DZ::$Struct {

	struct Header {

		static inline constexpr Sz kSz = 8;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // id
			OSD(0x6, 2), // dirSum
			OSD(0x4, 2), // nameSum
		};

		FourCC $id;
		IU16 $dirSum;
		IU16 $nameSum;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $dirSum,
				& $nameSum,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct ResPathInfo {

		static inline constexpr Sz kSz = 6;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 2), // dirIdx
			OSD(0x2, 2), // nameIdx
			OSD(0x4, 2), // end
		};

		IU16 $dirIdx;
		IU16 $nameIdx;
		IU16 $end;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $dirIdx,
				& $nameIdx,
				& $end,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct ResDataInfo {

		static inline constexpr Sz kSz = 16;
		static inline constexpr Sz kSum = 4;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // ofs
			OSD(0x4, 4), // sz
			OSD(0x8, 4), // sz2
			OSD(0xC, 4), // flag
		};

		IU32 $ofs;
		IU32 $sz;
		IU32 $sz2;
		IU32 $flag;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $ofs,
				& $sz,
				& $sz2,
				& $flag,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	// TODO
	struct UnKnownBlock {

		static inline constexpr Sz kSz = 4;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 2), // unk1
			OSD(0x2, 2), // unkE
		};

		IU16 $unk1; // always 1
		IU16 $unkE; // sum of resource

		auto addr() const {
			return new Void const * const [kSum] {\
				& $unk1,
				& $unkE,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	using LZMAProps = Byte[5];

	struct CompressHeader512 {

		static inline constexpr Sz kSz = 13;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 5), // props
			OSD(0x5, 4), // sz
			OSD(0x9, 4), // null, maybe ofs
		};

		LZMAProps $props;
		IU32 $sz;
		IU32 $null; // always = { 0x0, 0x0, 0x0, 0x0 }

		auto addr() const {
			return new Void const * const [kSum] {\
				& $props,
				& $sz,
				& $null,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct CompressHeader8 {

		static inline constexpr Sz kSz = 8;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // sz
			OSD(0x4, 4), // null
		};

		IU32 $sz;
		IU32 $null; // always = { 0x0, 0x0, 0x0, 0x0 }

		auto addr() const {
			return new Void const * const [kSum] {\
				& $sz,
				& $null,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	namespace $Const {

		namespace $Header {

			inline constexpr FourCC kID = makeFourCC('D', 'T', 'R', 'Z');

		}

		namespace $ResPathInfo {

			inline constexpr IU16 kEnd(0xFFFF);

		}

		namespace $UnKnownBlock {

			inline constexpr IU16 kUnKnown1(1);

		}

	}

}
