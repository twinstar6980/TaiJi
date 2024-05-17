#pragma once

#include "../../../../Type.hpp"
#include "../../../../TypeUtil.hpp"
#include "../../../../UserLiteral.hpp"
#include "../../../../Type/ByteStream.hpp"
#include "../../../../Type/BlockFunc.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$RSGP::$Struct {

	inline constexpr FourCC kHeaderID(makeFourCC('p', 'g', 's', 'r'));

	struct Header {

		static inline constexpr Sz kSz = 92;
		static inline constexpr Sz kSum = 12;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // id
			OSD(0x4, 4), // headerType
			OSD(0x14, 4), // headerAndResInfoListSize
			OSD(0x10, 4), // resStoreMethod
			OSD(0x4C, 4), // resInfoListOfs
			OSD(0x48, 4), // resInfoListSz
			OSD(0x18, 4), // normalResDataOfs
			OSD(0x1C, 4), // normalResDataSz
			OSD(0x20, 4), // normalResDataOriSz
			OSD(0x28, 4), // atlasResDataOfs
			OSD(0x2C, 4), // atlasResDataSz
			OSD(0x30, 4), // atlasResDataOriSz
		};

		FourCC $id;
		IU32 $headerType;
		IU32 $headerAndResInfoSectSz;
		IU32 $resStoreMethod;
		IU32 $resInfoSectOfs;
		IU32 $resInfoSectSz;
		IU32 $normalResDataOfs;
		IU32 $normalResDataSz;
		IU32 $normalResDataOriSz;
		IU32 $atlasResDataOfs;
		IU32 $atlasResDataSz;
		IU32 $atlasResDataOriSz;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $headerType,
				& $headerAndResInfoSectSz,
				& $resStoreMethod,
				& $resInfoSectOfs,
				& $resInfoSectSz,
				& $normalResDataOfs,
				& $normalResDataSz,
				& $normalResDataOriSz,
				& $atlasResDataOfs,
				& $atlasResDataSz,
				& $atlasResDataOriSz,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	namespace $ResTypeIdx {

		inline constexpr IU32 kNormal(0x0);
		inline constexpr IU32 kAtlas(0x1);

	}

	struct ResDataInfo {

		static inline constexpr Sz kSz = 12;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // type
			OSD(0x4, 4), // ofs
			OSD(0x8, 4), // sz
		};

		IU32 $type;
		IU32 $ofs;
		IU32 $sz;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $type,
				& $ofs,
				& $sz,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct ResAtlasInfo {

		static inline constexpr Sz kSz = 20;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // idx
			OSD(0xC, 4), // w
			OSD(0x10, 4), // h
		};

		IU32 $idx;
		IU32 $w;
		IU32 $h;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $idx,
				& $w,
				& $h,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct ResInfoSlot {

		ResDataInfo $data;
		ResAtlasInfo $atlas;

		auto sz() const {
			auto sz = kSzN;
			switch ($data.$type) {
				default:
					throw VAMGException("RSGPStruct-ResInfoSlot", "data type error : %u", $data.$type);
					break;
				case $ResTypeIdx::kAtlas:
					sz += $atlas.kSz;
					[[fallthrough]];
				case $ResTypeIdx::kNormal:
					sz += $data.kSz;
					break;
			}
			return sz;
		}

		auto read(ByteStream & t) {
			$StaticBlock::read($data, t);
			if ($data.$type == $ResTypeIdx::kAtlas)
				$StaticBlock::read($atlas, t);
			return;
		}
		auto write(ByteStream & t) const {
			$StaticBlock::write($data, t);
			if ($data.$type == $ResTypeIdx::kAtlas)
				$StaticBlock::write($atlas, t);
			return;
		}

	};

}
