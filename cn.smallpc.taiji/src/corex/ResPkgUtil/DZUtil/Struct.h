# pragma once

# include "../../Type.h"
# include "../../MyType/OSD.h"

namespace $TJ::$ResPkgUtil::$DZUtil::$Struct {
	namespace $Header {
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 4);
			constexpr OSD kDirSum(0x6, 2);
			constexpr OSD kNameSum(0x4, 2);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr FourCC kID(makeFourCC('D', 'T', 'R', 'Z'));
		}
		struct Member {
			FourCC _id;
			UI16 _dirSum;
			UI16 _nameSum;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _dirSum,
						& _nameSum,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _dirSum,
						& _nameSum,
				};
			}
		};
	}
	namespace $ResPathInfo {
		constexpr SzI kBlockSize(6);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kDirIdx(0x0, 2);
			constexpr OSD kNameIdx(0x2, 2);
			constexpr OSD kEnd(0x4, 2);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr UI16 kEnd(0xFFFF);
		}
		struct Member {
			UI16 _dirIdx;
			UI16 _nameIdx;
			UI16 _end;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_dirIdx,
						& _nameIdx,
						& _end,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_dirIdx,
						& _nameIdx,
						& _end,
				};
			}
		};
	}
	namespace $ResDataInfo {
		constexpr SzI kBlockSize(16);
		constexpr SzI kMemberSum(4);
		namespace $MemberOSD {
			constexpr OSD kOfs(0x0, 4);
			constexpr OSD kSize(0x4, 4);
			constexpr OSD kSize_2(0x8, 4);
			constexpr OSD kFlag(0xC, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _ofs;
			UI32 _sz;
			UI32 _sz2;
			UI32 _flag;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_ofs,
						& _sz,
						& _sz2,
						& _flag,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_ofs,
						& _sz,
						& _sz2,
						& _flag,
				};
			}
		};
	}
	// TODO
	namespace $UnKnownBlock {
		constexpr SzI kBlockSize(4);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kUnKnown_1(0x0, 2);
			constexpr OSD kUnKnown_e(0x2, 2);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		namespace $MemberConst {
			constexpr UI16 kUnKnown_1(1);
		}
		struct Member {
			UI16 _unKnown_1; // always 1
			UI16 _unKnown_e; // sum of resource

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_unKnown_1,
						& _unKnown_e,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_unKnown_1,
						& _unKnown_e,
				};
			}
		};
	}

	using LZMAProps = Byte[5];
	namespace $CompressHeader_512 {
		constexpr SzI kBlockSize(13);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kProps(0x0, 5);
			constexpr OSD kSize(0x5, 4);
			constexpr OSD kNullBlock(0x9, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			LZMAProps _props;
			UI32 _sz;
			UI32 _nullBlock; // always = { 0x0, 0x0, 0x0, 0x0 }

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_props,
						& _sz,
						& _nullBlock,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_props,
						& _sz,
						& _nullBlock,
				};
			}
		};
	}
	namespace $CompressHeader_8 {
		// unknown DataBlock
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(2);

		namespace $MemberOSD {
			constexpr OSD kSize(0x0, 4);
			constexpr OSD kNullBlock(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _sz;
			UI32 _nullBlock; // always = { 0x0, 0x0, 0x0, 0x0 }

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_sz,
						& _nullBlock,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_sz,
						& _nullBlock,
				};
			}
		};
	}
}
