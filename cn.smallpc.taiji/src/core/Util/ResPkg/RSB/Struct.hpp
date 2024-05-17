#pragma once

#include "../../../Type.hpp"
#include "../../../TypeUtil.hpp"
#include "../../../UserLiteral.hpp"
#include "../../../Type/ByteStream.hpp"
#include "../../../Type/Str.hpp"
#include "../../../Type/BlockFunc.hpp"
#include "./Common.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$Struct {

	inline constexpr FourCC kHeaderID(makeFourCC('1', 'b', 's', 'r'));

	struct Header {

		static inline constexpr Sz kSz = 108;
		static inline constexpr Sz kSum = 24;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // id
			OSD(0x4, 4), // headerType
			OSD(0xC, 4), // sz
			OSD(0x44, 4), // groupIDListOfs
			OSD(0x40, 4), // groupIDListSz
			OSD(0x38, 4), // groupInfoListOfs
			OSD(0x3C, 4), // groupInfoListSz
			OSD(0x34, 4), // groupInfoListSum
			OSD(0x24, 4), // subGroupIDListOfs
			OSD(0x20, 4), // subGroupIDListSz
			OSD(0x2C, 4), // subGroupInfoListOfs
			OSD(0x30, 4), // subGroupInfoListSz
			OSD(0x28, 4), // subGroupInfoListSum
			OSD(0x14, 4), // resPathListOfs
			OSD(0x10, 4), // resPathListSz
			OSD(0x58, 4), // atlasResInfoListOfs
			OSD(0x5C, 4), // atlasResInfoListSz
			OSD(0x54, 4), // atlasResInfoListSum
			OSD(0x4C, 4), // autoPoolInfoListOfs
			OSD(0x50, 4), // autoPoolInfoListSz
			OSD(0x48, 4), // autoPoolInfoListSum
			OSD(0x60, 4), // resInfoList_Group_Ofs
			OSD(0x64, 4), // resInfoList_Res_Ofs
			OSD(0x68, 4), // resInfoList_Str_Ofs
		};

		FourCC $id;
		IU32 $headerType;
		IU32 $sz;
		IU32 $groupIDListOfs;
		IU32 $groupIDListSz;
		IU32 $groupInfoListOfs;
		IU32 $groupInfoListSz;
		IU32 $groupInfoListSum;
		IU32 $subGroupIDListOfs;
		IU32 $subGroupIDListSz;
		IU32 $subGroupInfoListOfs;
		IU32 $subGroupInfoListSz;
		IU32 $subGroupInfoListSum;
		IU32 $resPathListOfs;
		IU32 $resPathListSz;
		IU32 $atlasResInfoListOfs;
		IU32 $atlasResInfoListSz;
		IU32 $atlasResInfoListSum;
		IU32 $autoPoolInfoListOfs;
		IU32 $autoPoolInfoListSz;
		IU32 $autoPoolInfoListSum;
		IU32 $resInfoList_Group_Ofs;
		IU32 $resInfoList_Res_Ofs;
		IU32 $resInfoList_Str_Ofs;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $headerType,
				& $sz,
				& $groupIDListOfs,
				& $groupIDListSz,
				& $groupInfoListOfs,
				& $groupInfoListSz,
				& $groupInfoListSum,
				& $subGroupIDListOfs,
				& $subGroupIDListSz,
				& $subGroupInfoListOfs,
				& $subGroupInfoListSz,
				& $subGroupInfoListSum,
				& $resPathListOfs,
				& $resPathListSz,
				& $atlasResInfoListOfs,
				& $atlasResInfoListSz,
				& $atlasResInfoListSum,
				& $autoPoolInfoListOfs,
				& $autoPoolInfoListSz,
				& $autoPoolInfoListSum,
				& $resInfoList_Group_Ofs,
				& $resInfoList_Res_Ofs,
				& $resInfoList_Str_Ofs,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct SubGroupInfoItem {

		static inline constexpr Sz kSz = 16;
		static inline constexpr Sz kSum = 3;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // idx
			OSD(0x4, 4), // res
			OSD(0x8, 4), // loc
		};

		IU32 $idx;
		IU32 $res;
		FourCC $loc;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $idx,
				& $res,
				& $loc,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct GroupInfo {

		static inline constexpr Sz kSz = 1156;
		static inline constexpr Sz kSubGroupLimit = 64;

		CID128 $id;
		List<SubGroupInfoItem> $subGroup;

		constexpr auto sz() const {
			return kSz;
		}

		auto & read(ByteStream & t) {
			t >> $id;
			$subGroup.allocThenUse(t.posGo(kSubGroupLimit * SubGroupInfoItem::kSz).rd<IU32>());
			t.posBack<IU32>().posBack(kSubGroupLimit * SubGroupInfoItem::kSz);
			$StaticBlock::read($subGroup, t);
			t.posGo<IU32>();
			return thiz;
		}
		auto & write(ByteStream & t) const {
			t << $id;
			$StaticBlock::write($subGroup, t);
			Range(t.curData(), (kSubGroupLimit - $subGroup.$sz) * SubGroupInfoItem::kSz).set(kByteNull);
			t.posGo((kSubGroupLimit - $subGroup.$sz) * SubGroupInfoItem::kSz);
			t << IU32($subGroup.$sz);
			return thiz;
		}

		auto & read(ByteSection const & t) {
			return read(ByteStream().ref(t).setPosBeg());
		}
		auto & write(ByteSection & t) const {
			return write(ByteStream().ref(t).setPosBeg());
		}

	};

	struct SubGroupInfo {

		static inline constexpr Sz kSz = 204;
		static inline constexpr Sz kSum = 15;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 128), // id
			OSD(0x80, 4), // ofs
			OSD(0x84, 4), // sz
			OSD(0x88, 4), // idx
			OSD(0x8C, 4), // resStoreMethod
			OSD(0x90, 4), // headerAndResInfoSectSz
			OSD(0x94, 4), // normalResDataOfs
			OSD(0x98, 4), // normalResDataSz
			OSD(0x9C, 4), // normalResDataOriSz
			OSD(0xA0, 4), // normalResDataOriSz2
			OSD(0xA4, 4), // atlasResDataOfs
			OSD(0xA8, 4), // atlasResDataSz
			OSD(0xAC, 4), // atlasResDataOriSz
			OSD(0xC4, 4), // atlasResSum
			OSD(0xC8, 4), // atlasResIdxBeg
		};

		CID128 $id;
		IU32 $ofs;
		IU32 $sz;
		IU32 $idx;
		IU32 $resStoreMethod;
		IU32 $headerAndResInfoSectSz;
		IU32 $normalResDataOfs;
		IU32 $normalResDataSz;
		IU32 $normalResDataOriSz;
		IU32 $normalResDataOriSz2;
		IU32 $atlasResDataOfs;
		IU32 $atlasResDataSz;
		IU32 $atlasResDataOriSz;
		IU32 $atlasResSum;
		IU32 $atlasResIdxBeg;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $ofs,
				& $sz,
				& $idx,
				& $resStoreMethod,
				& $headerAndResInfoSectSz,
				& $normalResDataOfs,
				& $normalResDataSz,
				& $normalResDataOriSz,
				& $normalResDataOriSz2,
				& $atlasResDataOfs,
				& $atlasResDataSz,
				& $atlasResDataOriSz,
				& $atlasResSum,
				& $atlasResIdxBeg,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct AutoPoolInfo {

		static inline constexpr Sz kSz = 152;
		static inline constexpr Sz kSum = 4;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 128), // id
			OSD(0x80, 4), // atlasResDataOfs
			OSD(0x84, 4), // atlasResDataSz
			OSD(0x88, 4), // unk_1
		};

		CID128 $id;
		IU32 $atlasResDataOfs;
		IU32 $atlasResDataSz;
		IU32 $unk_1; // unknown, always 1

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $atlasResDataOfs,
				& $atlasResDataSz,
				& $unk_1,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	namespace $TexFmtIdx {

		constexpr Sz kNonCompress(0x0);
		constexpr Sz kCompressEd_PVRTC4(0x1E);
		constexpr Sz kCompressEd_ETC1_PLUS_ALPHA(0x93);

	}

	struct AtlasResInfo {

		static inline constexpr Sz kSz = 16;
		static inline constexpr Sz kSum = 4;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // w
			OSD(0x4, 4), // h
			OSD(0xC, 4), // fmt
			OSD(0x8, 4), // unk_w4
		};

		IU32 $w;
		IU32 $h;
		IU32 $fmt;
		IU32 $unk_w4; // something unknown, always width * 4 ?

		auto addr() const {
			return new Void const * const [kSum] {\
				& $w,
				& $h,
				& $fmt,
				& $unk_w4,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct AtlasResExpandInfo {

		static inline constexpr Sz kSz = 8;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // alphaSz
			OSD(0x4, 4), // unk_100
		};

		IU32 $alphaSz;
		IU32 $unk_100; // always 100 ?

		auto addr() const {
			return new Void const * const [kSum] {\
				& $alphaSz,
				& $unk_100,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct StrIdxInfo {

		static inline constexpr Sz kSz = 4;
		static inline constexpr Sz kSum = 1;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // idx
		};

		IU32 $idx;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $idx,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct StrIdxInfoSlot {

		StrIdxInfo $idx;

		auto & read(ByteStream & t) {
			$StaticBlock::read($idx, t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			$StaticBlock::write($idx, t);
			return thiz;
		}

	};

	/*

	namespace $ResList_Res_AppendMember {
		constexpr Sz kBlockSize(12);
		constexpr Sz kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kKey_Ofs(0x0, 4);
			constexpr OSD kUnKnown(0x4, 4);
			constexpr OSD kVal_Ofs(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			IU32 _keyOfs;
			IU32 _unKnown;
			IU32 _valOfs;

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
		constexpr Sz kBlockSize(18);
		constexpr Sz kMemberSum(11);
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
			UI16 $x;
			UI16 $y;
			UI16 _w;
			UI16 _h;
			UI16 _unKnown_C;
			UI16 _unKnown_D;
			IU32 _parent_Ofs;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_unKnown_A,
						& _flag,
						& _paddingX,
						& _paddingY,
						& $x,
						& $y,
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
						& $x,
						& $y,
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
		constexpr Sz kBlockSize(28);
		constexpr Sz kMemberSum(8);
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
			IU32 _unKnown;
			UI16 $type;
			UI16 _appendBlock_RelaOfs;
			IU32 _appendMember_Ofs;
			IU32 _expandMember_Ofs;
			IU32 _id_Ofs;
			IU32 _path_Ofs;
			IU32 _appendMember_Sum;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_unKnown,
						& $type,
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
						& $type,
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
			return static_cast<Sz>(ternary_oper(_info._expandMember_Ofs == kThNone, _info.getSz() + _appendMember.getSz(), _info.getSz() + _expandMember.getSz() + _appendMember.getSz()));
		}
		auto & read(Byte const * const & data, Sz const & absPos) {
			$DataBlockUtil::read(data + kThFirst, _info);
			if (_info._expandMember_Ofs != kThNone)
				$DataBlockUtil::read(data + (_info._expandMember_Ofs - absPos), _expandMember);
			_appendMember.allocThenNew(_info._appendMember_Sum);
			$DataBlockUtil::readArr(data + (_info._appendMember_Ofs - absPos), _appendMember);
			return thiz;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			$DataBlockUtil::writeArr(data + _info.getSz(), _appendMember);
			return thiz;
		}
	};
	namespace $ResList_ResSlot {
		constexpr Sz kBlockSize(4);
		constexpr Sz kMemberSum(1);
		namespace $MemberOSD {
			constexpr OSD kInfo_Ofs(0x0, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			IU32 _info_Ofs;

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
		constexpr Sz kBlockSize(16);
		constexpr Sz kMemberSum(4);
		namespace $MemberOSD {
			constexpr OSD kID_Ofs(0x8, 4);
			constexpr OSD kRes_Sum(0xC, 4);
			constexpr OSD kResID(0x0, 4);
			constexpr OSD kLocID(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			IU32 _id_Ofs;
			IU32 _res_Sum;
			IU32 _resID;
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

		ListOfBlock<$ResList_ResSlot::Member> $res;

		auto getSz() const { return static_cast<Sz>(_info.getSz() + $res.getSz()); }

		auto & read(Byte const * const & data) {
			$DataBlockUtil::read(data + kThFirst, _info);
			$res.allocThenNew(_info._res_Sum);
			$DataBlockUtil::readArr(data + _info.getSz(), $res);
			return thiz;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			$DataBlockUtil::writeArr(data + _info.getSz(), $res);
			return thiz;
		}

	};
	namespace $ResList_Group_Base {
		constexpr Sz kBlockSize(12);
		constexpr Sz kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kID_Ofs(0x0, 4);
			constexpr OSD kSubGroup_Sum(0x4, 4);
			constexpr OSD kUnKnown(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			IU32 _id_Ofs;
			IU32 _subGroup_Sum;
			IU32 _unKnown;

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

		ListOfBlock<ResList_SubGroup> $subGroup;

		auto getSz() const { return static_cast<Sz>(_info.getSz() + $subGroup.getSz()); }

		auto & read(Byte const * const & data) {
			$DataBlockUtil::read(data + kThFirst, _info);
			$subGroup.read(data + _info.getSz(), _info._subGroup_Sum);
			return thiz;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			$subGroup.write(data + _info.getSz());
			return thiz;
		}

	};
*/

}
