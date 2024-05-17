# pragma once

# include "../../Type.h"
# include "../../MyType/OSD.h"
# include "../../UtilKit/DataBlockUtil.h"
# include "../../MyType/Str.h"
# include "../../DataUtil/Hash/Hash.h"

namespace $TJ::$AudioUtil::$SoundBankUtil::$Struct {
	namespace $BlockSign {
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 4);
			constexpr OSD kSize(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			FourCC _id;
			UI32 _sz;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _sz,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _sz,
				};
			}
		};
	}
	namespace $BlockID {
		constexpr FourCC kBKHD(makeFourCC('B', 'K', 'H', 'D'));
		constexpr FourCC kDIDX(makeFourCC('D', 'I', 'D', 'X'));
		constexpr FourCC kDATA(makeFourCC('D', 'A', 'T', 'A'));
		constexpr FourCC kSTID(makeFourCC('S', 'T', 'I', 'D'));
		constexpr FourCC kSTMG(makeFourCC('S', 'T', 'M', 'G'));
		constexpr FourCC kHIRC(makeFourCC('H', 'I', 'R', 'C'));
	}
	namespace $BlockIDStr {
		const Str kBKHD("BKHD");
		const Str kDIDX("DIDX");
		const Str kDATA("DATA");
		const Str kSTID("STID");
		const Str kSTMG("STMG");
		const Str kHIRC("HIRC");
	}
	namespace $BKHD_Begin {
		constexpr SzI kBlockSize(16);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kVer(0x0, 4);
			constexpr OSD kID(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _ver;
			UI32 _id;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_ver,
						& _id,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_ver,
						& _id,
				};
			}
		};
	}
	namespace $DIDX_Sub {
		constexpr SzI kBlockSize(12);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 4);
			constexpr OSD kOfs(0x4, 4);
			constexpr OSD kSize(0x8, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _id;
			UI32 _ofs;
			UI32 _sz;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _ofs,
						& _sz,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _ofs,
						& _sz,
				};
			}
		};
	}
	namespace $STID_Begin {
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(2);
		namespace $MemberConst {
			constexpr UI32 kUnKnown_1(0x1);
		}
		namespace $MemberOSD {
			constexpr OSD kUnKnown_1(0x0, 4);
			constexpr OSD kItemSum(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _unKnown_1; // always 1
			UI32 _itemSum;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_unKnown_1,
						& _itemSum,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_unKnown_1,
						& _itemSum,
				};
			}
		};
	}
	struct STID_Sub {
		UI32 _id;
		StrBlock8 _name;

		auto getSz() const { return  static_cast<SzI>(szOf<UI32>() + _name.getSz()); }

		auto & read(Byte const * const data) {
			memCpy(&_id, data + kThFirst, szOf<UI32>());
			_name.read(data + szOf<UI32>());
			return *this;
		}
		auto & write(Byte * const data) const {
			memCpy(data + kThFirst, &_id, szOf<UI32>());
			_name.write(data + szOf<UI32>());
			return *this;
		}
	};
	namespace $HIRC_Begin {
		constexpr SzI kBlockSize(4);
		constexpr SzI kMemberSum(1);
		namespace $MemberOSD {
			constexpr OSD kSum(0x0, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _sz;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_sz,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_sz,
				};
			}
		};
	}
	namespace $HIRC_Sub_Begin {
		constexpr SzI kBlockSize(9);
		constexpr SzI kMemberSum(3);
		namespace $MemberOSD {
			constexpr OSD kIdent(0x0, 1);
			constexpr OSD kSize(0x1, 4);
			constexpr OSD kID(0x5, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI8 _type;
			UI32 _sz;
			UI32 _id;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_type,
						& _sz,
						& _id,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_type,
						& _sz,
						& _id,
				};
			}
		};
	}
}
