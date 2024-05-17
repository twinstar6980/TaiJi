# pragma once

# include "../../Type.h"
# include "../../MyType/OSD.h"

# include "../../UtilKit/DataBlockUtil.h"

namespace $TJ::$ResPkgUtil::$RSGPUtil::$Struct {
	namespace $ExistBit_CompressMethod {
		constexpr UI32 kCompressNormalData(2);
		constexpr UI32 kCompressAtlasData(1);
		constexpr UI32 kNone(0);
	}
	namespace $Header {
		constexpr SzI kBlockSize(92);
		constexpr SzI kMemberSum(12);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 4);
			constexpr OSD kHeaderType(0x4, 4);
			constexpr OSD kHeaderAndResInfoListSize(0x14, 4);
			constexpr OSD kResDataCompressMethod(0x10, 4);
			constexpr OSD kResInfoList_Ofs(0x4C, 4);
			constexpr OSD kResInfoList_Size(0x48, 4);
			constexpr OSD kNormalResData_Ofs(0x18, 4);
			constexpr OSD kNormalResData_Size(0x1C, 4);
			constexpr OSD kNormalResData_InitSize(0x20, 4);
			constexpr OSD kAtlasResData_Ofs(0x28, 4);
			constexpr OSD kAtlasResData_Size(0x2C, 4);
			constexpr OSD kAtlasResData_InitSize(0x30, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr FourCC kID(makeFourCC('p', 'g', 's', 'r'));
		}
		struct Member {
			FourCC _id;
			UI32 _headerType;
			UI32 _headerAndResInfoListSize;
			UI32 _resDataCompressMethod;
			UI32 _resInfoList_Ofs;
			UI32 _resInfoList_Size;
			UI32 _normalResData_Ofs;
			UI32 _normalResData_Size;
			UI32 _normalResData_InitSize;
			UI32 _atlasResData_Ofs;
			UI32 _atlasResData_Size;
			UI32 _atlasResData_InitSize;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _headerType,
						& _headerAndResInfoListSize,
						& _resDataCompressMethod,
						& _resInfoList_Ofs,
						& _resInfoList_Size,
						& _normalResData_Ofs,
						& _normalResData_Size,
						& _normalResData_InitSize,
						& _atlasResData_Ofs,
						& _atlasResData_Size,
						& _atlasResData_InitSize,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _headerType,
						& _headerAndResInfoListSize,
						& _resDataCompressMethod,
						& _resInfoList_Ofs,
						& _resInfoList_Size,
						& _normalResData_Ofs,
						& _normalResData_Size,
						& _normalResData_InitSize,
						& _atlasResData_Ofs,
						& _atlasResData_Size,
						& _atlasResData_InitSize,
				};
			}
		};
	}
	namespace $ResTypeIdx {
		constexpr UI32 kNormal(0x0);
		constexpr UI32 kAtlas(0x1);
	}
	namespace $ResDataInfo {
		constexpr SzI kBlockSize(12);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kResType(0x0, 4);
			constexpr OSD kOfs(0x4, 4);
			constexpr OSD kSize(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _resType;
			UI32 _ofs;
			UI32 _sz;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_resType,
						& _ofs,
						& _sz,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_resType,
						& _ofs,
						& _sz,
				};
			}
		};
	}
	namespace $ResAtlasInfo {
		constexpr SzI kBlockSize(20);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kIdx(0x0, 4);
			constexpr OSD kWidth(0xC, 4);
			constexpr OSD kHeight(0x10, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _idx;
			UI32 _w;
			UI32 _h;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return  new Void * const [kMemberSum] {
					&_idx,
						& _w,
						& _h,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_idx,
						& _w,
						& _h,
				};
			}
		};
	}
	struct ResInfoSlot {
		$ResDataInfo::Member _dataInfo;
		$ResAtlasInfo::Member _atlasInfo;

		auto getSz() const {
			auto sz = kSzNone;
			switch (_dataInfo._resType) {
				default: sz += kSzNone; break;
				case $ResTypeIdx::kAtlas: sz += _atlasInfo.getSz();
				case $ResTypeIdx::kNormal: sz += _dataInfo.getSz(); break;
			}
			return sz;
		}
		auto read(Byte const * const data) {
			$DataBlockUtil::read(data + kThFirst, _dataInfo);
			if (_dataInfo._resType == $ResTypeIdx::kAtlas)
				$DataBlockUtil::read(data + _dataInfo.getSz(), _atlasInfo);
			return;
		}
		auto write(Byte * const data) const {
			$DataBlockUtil::write(data + kThFirst, _dataInfo);
			if (_dataInfo._resType == $ResTypeIdx::kAtlas)
				$DataBlockUtil::write(data + _dataInfo.getSz(), _atlasInfo);
			return;
		}
	};
}
