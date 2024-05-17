# pragma once

# include "../../Type.h"
# include "../../MyType/OSD.h"

# include "../../UtilKit/MemoryUtil.h"
# include "../../UtilKit/DataBlockUtil.h"
# include "../RSGPUtil/Struct.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$Struct {
	using CID128 = Ch[128];

	constexpr SzI kResID_Null(0);
	constexpr FourCC kLocID_Null(makeFourCC('\0', '\0', '\0', '\0'));

	Str const kSuffixOfCompositeShell("_CompositeShell");
	Str const kSuffixOfAutoPool("_AutoPool");

	inline auto SetCID128(CID128 & dst, Str const & src) {
		memCpy(&dst[kThFirst], src._str, src._len);
		memSet(&dst[src._len], szOf<CID128>() - src._len);
		return;
	}

	inline auto GetLocID(Ch * const & dst, FourCC const & src) {
		*reinterpret_cast<FourCC *>(dst) = cnvEndian(src);
		return;
	}
	inline auto GetLocID(Str & dst, FourCC const & src) {
		if (src == kLocID_Null)
			dst.setNull();
		else {
			dst.alloc(szOf<FourCC>());
			GetLocID(dst._str, src);
			dst._len += szOf<FourCC>();
		}
		return;
	}
	inline auto SetLocID(FourCC & dst, Ch const * const & src) {
		dst = cnvEndian(*reinterpret_cast<FourCC const *>(src));
		return;
	}
	inline auto SetLocID(FourCC & dst, Str const & src) {
		if (src.isNull())
			dst = kLocID_Null;
		else SetLocID(dst, src._str);
		return;
	}

	namespace $Header {
		constexpr SzI kBlockSize(108);
		constexpr SzI kMemberSum(24);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 4);
			constexpr OSD kHeaderType(0x4, 4);
			constexpr OSD kSize(0xC, 4);
			constexpr OSD kGroupIDList_Ofs(0x44, 4);
			constexpr OSD kGroupIDList_Size(0x40, 4);
			constexpr OSD kGroupInfoList_Ofs(0x38, 4);
			constexpr OSD kGroupInfoList_Size(0x3C, 4);
			constexpr OSD kGroupInfoList_Sum(0x34, 4);
			constexpr OSD kSubGroupIDList_Ofs(0x24, 4);
			constexpr OSD kSubGroupIDList_Size(0x20, 4);
			constexpr OSD kSubGroupInfoList_Ofs(0x2C, 4);
			constexpr OSD kSubGroupInfoList_Size(0x30, 4);
			constexpr OSD kSubGroupInfoList_Sum(0x28, 4);
			constexpr OSD kResPathList_Ofs(0x14, 4);
			constexpr OSD kResPathList_Size(0x10, 4);
			constexpr OSD kAtlasResInfoList_Ofs(0x58, 4);
			constexpr OSD kAtlasResInfoList_Size(0x5C, 4);
			constexpr OSD kAtlasResInfoList_Sum(0x54, 4);
			constexpr OSD kAutoPoolInfoList_Ofs(0x4C, 4);
			constexpr OSD kAutoPoolInfoList_Size(0x50, 4);
			constexpr OSD kAutoPoolInfoList_Sum(0x48, 4);
			constexpr OSD kResInfoList_Group_Ofs(0x60, 4);
			constexpr OSD kResInfoList_Res_Ofs(0x64, 4);
			constexpr OSD kResInfoList_Str_Ofs(0x68, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr FourCC kID(makeFourCC('1', 'b', 's', 'r'));
		}
		struct Member {
			FourCC _id;
			UI32 _headerType;
			UI32 _sz;
			UI32 _groupIDList_Ofs;
			UI32 _groupIDList_Size;
			UI32 _groupInfoList_Ofs;
			UI32 _groupInfoList_Size;
			UI32 _groupInfoList_Sum;
			UI32 _subGroupIDList_Ofs;
			UI32 _subGroupIDList_Size;
			UI32 _subGroupInfoList_Ofs;
			UI32 _subGroupInfoList_Size;
			UI32 _subGroupInfoList_Sum;
			UI32 _resPathList_Ofs;
			UI32 _resPathList_Size;
			UI32 _atlasResInfoList_Ofs;
			UI32 _atlasResInfoList_Size;
			UI32 _atlasResInfoList_Sum;
			UI32 _autoPoolInfoList_Ofs;
			UI32 _autoPoolInfoList_Size;
			UI32 _autoPoolInfoList_Sum;
			UI32 _resInfoList_Group_Ofs;
			UI32 _resInfoList_Res_Ofs;
			UI32 _resInfoList_Str_Ofs;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _headerType,
						& _sz,
						& _groupIDList_Ofs,
						& _groupIDList_Size,
						& _groupInfoList_Ofs,
						& _groupInfoList_Size,
						& _groupInfoList_Sum,
						& _subGroupIDList_Ofs,
						& _subGroupIDList_Size,
						& _subGroupInfoList_Ofs,
						& _subGroupInfoList_Size,
						& _subGroupInfoList_Sum,
						& _resPathList_Ofs,
						& _resPathList_Size,
						& _atlasResInfoList_Ofs,
						& _atlasResInfoList_Size,
						& _atlasResInfoList_Sum,
						& _autoPoolInfoList_Ofs,
						& _autoPoolInfoList_Size,
						& _autoPoolInfoList_Sum,
						& _resInfoList_Group_Ofs,
						& _resInfoList_Res_Ofs,
						& _resInfoList_Str_Ofs
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _headerType,
						& _sz,
						& _groupIDList_Ofs,
						& _groupIDList_Size,
						& _groupInfoList_Ofs,
						& _groupInfoList_Size,
						& _groupInfoList_Sum,
						& _subGroupIDList_Ofs,
						& _subGroupIDList_Size,
						& _subGroupInfoList_Ofs,
						& _subGroupInfoList_Size,
						& _subGroupInfoList_Sum,
						& _resPathList_Ofs,
						& _resPathList_Size,
						& _atlasResInfoList_Ofs,
						& _atlasResInfoList_Size,
						& _atlasResInfoList_Sum,
						& _autoPoolInfoList_Ofs,
						& _autoPoolInfoList_Size,
						& _autoPoolInfoList_Sum,
						& _resInfoList_Group_Ofs,
						& _resInfoList_Res_Ofs,
						& _resInfoList_Str_Ofs
				};
			}
		};
	}
	namespace $GroupInfo {
		constexpr SzI kBlockSize(1156);
		constexpr SzI kSubGroupUpperLimit(64);
		namespace $SubGroupInfoItem {
			constexpr SzI kBlockSize(16);
			constexpr SzI kMemberSum(3);
			namespace $MemberOSD {
				constexpr OSD kIdx(0x0, 4);
				constexpr OSD kResID(0x4, 4);
				constexpr OSD kLocID(0x8, 4);
			}
			extern OSD const * const kMemberOSDPtr[kMemberSum];
			struct Member {
				UI32 _idx;
				UI32 _resID;
				FourCC _locID;

				constexpr auto getSz() const { return kBlockSize; }
				constexpr auto getSum() const { return kMemberSum; }

				OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

				auto getPtr() {
					return new Void * const [kMemberSum] {
						&_idx,
							& _resID,
							& _locID,
					};
				}
				auto getPtr() const {
					return new Void const * const [kMemberSum] {
						&_idx,
							& _resID,
							& _locID,
					};
				}
			};
		}
		struct GroupInfo {
			CID128 _id;
			ListOfBlock<$SubGroupInfoItem::Member> _subGroupList;

			constexpr auto getSz() const { return kBlockSize; }

			auto & read(Byte const * const & data) {
				memCpy(&_id, data + kThFirst, szOf<CID128>());
				_subGroupList.allocThenNew(*(reinterpret_cast<UI32 const *>(data + szOf<CID128>() + ($SubGroupInfoItem::kBlockSize * kSubGroupUpperLimit))));
				$DataBlockUtil::readArr(data + szOf<CID128>(), _subGroupList);
				return *this;
			}
			auto & write(Byte * const & data) const {
				memCpy(data + kThFirst, &_id, szOf<CID128>());
				*(reinterpret_cast<UI32 *>(data + szOf<CID128>() + ($SubGroupInfoItem::kBlockSize * kSubGroupUpperLimit))) = _subGroupList._sz;
				$DataBlockUtil::writeArr(data + szOf<CID128>(), _subGroupList);
				memSet(data + szOf<CID128>() + _subGroupList.getSz(), ($SubGroupInfoItem::kBlockSize * kSubGroupUpperLimit) - _subGroupList.getSz());
				return *this;
			}
		};
	}
	namespace $SubGroupInfo {
		constexpr SzI kBlockSize(204);
		constexpr SzI kMemberSum(15);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 128);
			constexpr OSD kRSGPOfs(0x80, 4);
			constexpr OSD kRSGPSize(0x84, 4);
			constexpr OSD kIdx(0x88, 4);
			constexpr OSD kRSGPResDataCompressMethod(0x8C, 4);
			constexpr OSD kRSGPHeaderAndResInfoListSize(0x90, 4);
			constexpr OSD kRSGPNormalResData_Ofs(0x94, 4);
			constexpr OSD kRSGPNormalResData_Size(0x98, 4);
			constexpr OSD kRSGPNormalResData_InitSize(0x9C, 4);
			constexpr OSD kRSGPNormalResData_InitSize_2(0xA0, 4);
			constexpr OSD kRSGPAtlasResData_Ofs(0xA4, 4);
			constexpr OSD kRSGPAtlasResData_Size(0xA8, 4);
			constexpr OSD kRSGPAtlasResData_InitSize(0xAC, 4);
			constexpr OSD kRSGPAtlasResSum(0xC4, 4);
			constexpr OSD kRSGPAtlasResIdxBegin(0xC8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			CID128 _id;
			UI32 _rsgpOfs;
			UI32 _rsgpSize;
			UI32 _idx;
			UI32 _rsgpResDataCompressMethod;
			UI32 _rsgpHeaderAndResInfoListSize;
			UI32 _rsgpNormalResData_Ofs;
			UI32 _rsgpNormalResData_Size;
			UI32 _rsgpNormalResData_InitSize;
			UI32 _rsgpNormalResData_InitSize_2;
			UI32 _rsgpAtlasResData_Ofs;
			UI32 _rsgpAtlasResData_Size;
			UI32 _rsgpAtlasResData_InitSize;
			UI32 _rsgpAtlasResSum;
			UI32 _rsgpAtlasResIdxBegin;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _rsgpOfs,
						& _rsgpSize,
						& _idx,
						& _rsgpResDataCompressMethod,
						& _rsgpHeaderAndResInfoListSize,
						& _rsgpNormalResData_Ofs,
						& _rsgpNormalResData_Size,
						& _rsgpNormalResData_InitSize,
						& _rsgpNormalResData_InitSize_2,
						& _rsgpAtlasResData_Ofs,
						& _rsgpAtlasResData_Size,
						& _rsgpAtlasResData_InitSize,
						& _rsgpAtlasResSum,
						& _rsgpAtlasResIdxBegin,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _rsgpOfs,
						& _rsgpSize,
						& _idx,
						& _rsgpResDataCompressMethod,
						& _rsgpHeaderAndResInfoListSize,
						& _rsgpNormalResData_Ofs,
						& _rsgpNormalResData_Size,
						& _rsgpNormalResData_InitSize,
						& _rsgpNormalResData_InitSize_2,
						& _rsgpAtlasResData_Ofs,
						& _rsgpAtlasResData_Size,
						& _rsgpAtlasResData_InitSize,
						& _rsgpAtlasResSum,
						& _rsgpAtlasResIdxBegin,
				};
			}
		};
	}
	namespace $AutoPoolInfo {
		constexpr SzI kBlockSize(152);
		constexpr SzI kMemberSum(4);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 128);
			constexpr OSD kAtlasResData_Ofs(0x80, 4);
			constexpr OSD kAtlasResData_Size(0x84, 4);
			constexpr OSD kUnKnown_1(0x88, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr UI32 kUnKnown_1(0x1);
		}
		struct Member {
			CID128 _id;
			UI32 _atlasResData_Ofs;
			UI32 _atlasResData_Size;
			UI32 _unKnown_1; // unknown, always 1

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _atlasResData_Ofs,
						& _atlasResData_Size,
						& _unKnown_1,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _atlasResData_Ofs,
						& _atlasResData_Size,
						& _unKnown_1,
				};
			}
		};
	}
	namespace $TexFmtIdx {
		constexpr SzI kNonCompress(0x0);
		constexpr SzI kCompressEd_PVRTC4(0x1E);
		constexpr SzI kCompressEd_ETC1_PLUS_ALPHA(0x93);
	}
	namespace $AtlasResInfo {
		constexpr SzI kBlockSize(16);
		constexpr SzI kMemberSum(4);
		namespace $MemberOSD {
			constexpr OSD kWidth(0x0, 4);
			constexpr OSD kHeight(0x4, 4);
			constexpr OSD kTexFmt(0xC, 4);
			constexpr OSD kUnKnown_w4(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _w;
			UI32 _h;
			UI32 _texFmt;
			UI32 _unKnown_w4; // something unknown, always width * 4 ?

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_w,
						& _h,
						& _texFmt,
						& _unKnown_w4,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_w,
						& _h,
						& _texFmt,
						& _unKnown_w4,
				};
			}
		};
	}
	namespace $AtlasResInfo_Append {
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kAlphaSize(0x0, 4);
			constexpr OSD kUnKnown_100(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr UI32 kUnKnown_100(100);
		}
		struct Member {
			UI32 _alphaSize;
			UI32 _unKnown_100; // something unknown, always 100 ?

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_alphaSize,
						& _unKnown_100,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_alphaSize,
						& _unKnown_100,
				};
			}
		};
	}
	namespace $StrIdxInfo {
		constexpr SzI kBlockSize(4);
		constexpr SzI kMemberSum(1);
		namespace $MemberOSD {
			constexpr OSD kIdx(0x0, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _idx;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_idx,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_idx,
				};
			}
		};
	}
	struct StrIdxInfoSlot {
		$StrIdxInfo::Member _strIdxInfo;

		constexpr auto getSz() const {
			return _strIdxInfo.getSz();
		}
		auto & read(Byte const * const & data) {
			$DataBlockUtil::read(data, _strIdxInfo);
			return *this;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data, _strIdxInfo);
			return *this;
		}
	};

	namespace $ResList_Res_AppendMember {
		constexpr SzI kBlockSize(12);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kKey_Ofs(0x0, 4);
			constexpr OSD kUnKnown(0x4, 4);
			constexpr OSD kVal_Ofs(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _keyOfs;
			UI32 _unKnown;
			UI32 _valOfs;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_keyOfs,
						& _unKnown,
						& _valOfs,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_keyOfs,
						& _unKnown,
						& _valOfs,
				};
			}
		};
	}
	namespace $ResList_Res_ExpandMember {
		constexpr SzI kBlockSize(18);
		constexpr SzI kMemberSum(11);
		namespace $MemberOSD {
			constexpr OSD kUnKnown_A(0x0, 2);
			constexpr OSD kFlag(0x2, 2);
			constexpr OSD kPaddingX(0x4, 2);
			constexpr OSD kPaddingY(0x6, 2);
			constexpr OSD kX(0x8, 2);
			constexpr OSD kY(0xA, 2);
			constexpr OSD kWidth(0xC, 2);
			constexpr OSD kHeight(0xE, 2);
			constexpr OSD kUnKnown_C(0x10, 2);
			constexpr OSD kUnKnown_D(0x12, 2);
			constexpr OSD kParent_Ofs(0x14, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI16 _unKnown_A;
			UI16 _flag;
			SI16 _paddingX;
			SI16 _paddingY;
			UI16 _x;
			UI16 _y;
			UI16 _w;
			UI16 _h;
			UI16 _unKnown_C;
			UI16 _unKnown_D;
			UI32 _parent_Ofs;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_unKnown_A,
						& _flag,
						& _paddingX,
						& _paddingY,
						& _x,
						& _y,
						& _w,
						& _h,
						& _unKnown_C,
						& _unKnown_D,
						& _parent_Ofs,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_unKnown_A,
						& _flag,
						& _paddingX,
						& _paddingY,
						& _x,
						& _y,
						& _w,
						& _h,
						& _unKnown_C,
						& _unKnown_D,
						& _parent_Ofs,
				};
			}
		};
	}
	namespace $ResList_Res_Base {
		constexpr SzI kBlockSize(28);
		constexpr SzI kMemberSum(8);
		namespace $MemberOSD {
			constexpr OSD kUnKnown(0x0, 4);
			constexpr OSD kType(0x4, 2);
			constexpr OSD kAppendBlock_RelaOfs(0x6, 2);
			constexpr OSD kAppendMember_Ofs(0x8, 4);
			constexpr OSD kExpandMember_Ofs(0xC, 4);
			constexpr OSD kID_Ofs(0x10, 4);
			constexpr OSD kPath_Ofs(0x14, 4);
			constexpr OSD kAppendMember_Sum(0x18, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		extern Str const kTypeStr[8];
		struct Member {
			UI32 _unKnown;
			UI16 _type;
			UI16 _appendBlock_RelaOfs;
			UI32 _appendMember_Ofs;
			UI32 _expandMember_Ofs;
			UI32 _id_Ofs;
			UI32 _path_Ofs;
			UI32 _appendMember_Sum;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_unKnown,
						& _type,
						& _appendBlock_RelaOfs,
						& _appendMember_Ofs,
						& _expandMember_Ofs,
						& _id_Ofs,
						& _path_Ofs,
						& _appendMember_Sum,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_unKnown,
						& _type,
						& _appendBlock_RelaOfs,
						& _appendMember_Ofs,
						& _expandMember_Ofs,
						& _id_Ofs,
						& _path_Ofs,
						& _appendMember_Sum,
				};
			}
		};
	}
	struct ResList_Res {
		$ResList_Res_Base::Member _info;
		$ResList_Res_ExpandMember::Member _expandMember;
		ListOfBlock<$ResList_Res_AppendMember::Member> _appendMember;

		auto getSz() const {
			return static_cast<SzI>(ternary_oper(_info._expandMember_Ofs == kThNone, _info.getSz() + _appendMember.getSz(), _info.getSz() + _expandMember.getSz() + _appendMember.getSz()));
		}
		auto & read(Byte const * const & data, SzI const & absPos) {
			$DataBlockUtil::read(data + kThFirst, _info);
			if (_info._expandMember_Ofs != kThNone)
				$DataBlockUtil::read(data + (_info._expandMember_Ofs - absPos), _expandMember);
			_appendMember.allocThenNew(_info._appendMember_Sum);
			$DataBlockUtil::readArr(data + (_info._appendMember_Ofs - absPos), _appendMember);
			return *this;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			$DataBlockUtil::writeArr(data + _info.getSz(), _appendMember);
			return *this;
		}
	};
	namespace $ResList_ResSlot {
		constexpr SzI kBlockSize(4);
		constexpr SzI kMemberSum(1);
		namespace $MemberOSD {
			constexpr OSD kInfo_Ofs(0x0, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _info_Ofs;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_info_Ofs,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_info_Ofs,
				};
			}
		};
	}
	namespace $ResList_SubGroup_Base {
		constexpr SzI kBlockSize(16);
		constexpr SzI kMemberSum(4);
		namespace $MemberOSD {
			constexpr OSD kID_Ofs(0x8, 4);
			constexpr OSD kRes_Sum(0xC, 4);
			constexpr OSD kResID(0x0, 4);
			constexpr OSD kLocID(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _id_Ofs;
			UI32 _res_Sum;
			UI32 _resID;
			FourCC _locID;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id_Ofs,
						& _res_Sum,
						& _resID,
						& _locID,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id_Ofs,
						& _res_Sum,
						& _resID,
						& _locID,
				};
			}
		};
	}
	struct ResList_SubGroup {

		$ResList_SubGroup_Base::Member _info;

		ListOfBlock<$ResList_ResSlot::Member> _res;

		auto getSz() const { return static_cast<SzI>(_info.getSz() + _res.getSz()); }

		auto & read(Byte const * const & data) {
			$DataBlockUtil::read(data + kThFirst, _info);
			_res.allocThenNew(_info._res_Sum);
			$DataBlockUtil::readArr(data + _info.getSz(), _res);
			return *this;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			$DataBlockUtil::writeArr(data + _info.getSz(), _res);
			return *this;
		}

	};
	namespace $ResList_Group_Base {
		constexpr SzI kBlockSize(12);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kID_Ofs(0x0, 4);
			constexpr OSD kSubGroup_Sum(0x4, 4);
			constexpr OSD kUnKnown(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _id_Ofs;
			UI32 _subGroup_Sum;
			UI32 _unKnown;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id_Ofs,
						& _subGroup_Sum,
						& _unKnown,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id_Ofs,
						& _subGroup_Sum,
						& _unKnown,
				};
			}
		};
	}
	struct ResList_Group {

		$ResList_Group_Base::Member _info;

		ListOfBlock<ResList_SubGroup> _subGroup;

		auto getSz() const { return static_cast<SzI>(_info.getSz() + _subGroup.getSz()); }

		auto & read(Byte const * const & data) {
			$DataBlockUtil::read(data + kThFirst, _info);
			_subGroup.read(data + _info.getSz(), _info._subGroup_Sum);
			return *this;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			_subGroup.write(data + _info.getSz());
			return *this;
		}

	};
}
