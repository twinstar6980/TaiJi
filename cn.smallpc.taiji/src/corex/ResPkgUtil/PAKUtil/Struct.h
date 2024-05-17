# pragma once

# include "../../Type.h"
# include "../../MyType/OSD.h"
# include "../../MyType/Str.h"
# include "../../UtilKit/DataBlockUtil.h"

namespace $TJ::$ResPkgUtil::$PAKUtil::$Struct {
	namespace $Header {
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kMagic(0x0, 4);
			constexpr OSD kVer(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _magic;
			UI64 _ver;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_magic,
						& _ver,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_magic,
						& _ver,
				};
			}
		};
	}
	namespace $EndFlag {
		constexpr Byte kNext(0x00);
		constexpr Byte kStop(0x80);
	}
	namespace $ResInfo_Base {
		constexpr SzI kBlockSize(12);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kSize(0x0, 4);
			constexpr OSD kTime(0x4, 8);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI32 _sz;
			UI64 _time;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_sz,
						& _time,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_sz,
						& _time,
				};
			}
		};
	}
	struct ResInfo {
		StrBlock8 _path;
		$ResInfo_Base::Member _info;

		SzI getSz() const {
			return  _path.getSz() + _info.getSz();
		}
		auto & read(Byte const * const data) {
			_path.read(data + kThFirst);
			$DataBlockUtil::read(data + _path.getSz(), _info);
			return *this;
		}
		auto & write(Byte * const data) const {
			_path.write(data + kThFirst);
			$DataBlockUtil::write(data + _path.getSz(), _info);
			return *this;
		}
	};
}
