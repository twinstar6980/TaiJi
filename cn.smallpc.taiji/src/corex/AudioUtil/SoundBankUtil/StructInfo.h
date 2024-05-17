# pragma once

# include "../../Type.h"
# include "../../MyType/ListOfOther.h"

# include "./Struct.h"

# include "../../MyType/StrList.h"
# include "../../MyType/Map.h"
# include "../../MyType/JSON.h"

namespace $TJ::$AudioUtil::$SoundBankUtil {
	extern Map<UI32, Str> gNameMapper;

	inline auto calcID32(Str const & name) { return $DataUtil::$Hash::$FNV::make32(Str(name).lowerCase()); }

	inline auto & setIDMaybeHasName(UI32 const & id, $JSON::Val & dst) {
		UI32 idx;
		if (gNameMapper.findKey(id, idx)) {
			wLogLn("::%s", gNameMapper.getVal(idx)._str);
			dst.set(gNameMapper[idx]._val);
		} else {
			dst.set(id);
		}
		return dst;
	}
}
namespace $TJ::$AudioUtil::$SoundBankUtil::$StructInfo {

	struct BKHD {
		SzI _id;
		SzI _ver;

		auto & read($JSON::Obj const & src) {
			src["ID"].getX(_id);
			src["Ver"].getX(_ver);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			setIDMaybeHasName(_id, dst.add("ID").getLast()._val);
			dst.add("Ver", _ver);
			return dst;
		}
		auto & parse($Struct::$BKHD_Begin::Member const & dst) {
			_id = dst._id;
			_ver = dst._ver;
			return *this;
		}
	};
	struct DIDX {
		UI32 _id;

		auto & read($JSON::Obj const & src) {
			src["ID"].getX(_id);
			return *this;
		}
		auto & write($JSON::Val & dst) const {
			setIDMaybeHasName(_id, dst);
			return dst;
		}
		auto & parse($Struct::$DIDX_Sub::Member const & src) {
			_id = src._id;
			return *this;
		}
	};
	struct STID {
		UI32 _id;
		Str _name;

		auto & read($JSON::Obj const & src) {
			src["ID"].getX(_id);
			src["Name"].getX(_name);
			return *this;
		}
		auto & write($JSON::Val & dst) const {
			if (calcID32(_name) != _id)
				wErr("STID-Error : %s", _name._str);
			dst.set(_name);
			return dst;
		}
		auto & parse($Struct::STID_Sub const & src) {
			_id = src._id;
			_name = src._name._str;
			return *this;
		}
	};
	struct HIRC {
		UI32 _id;
		UI32 _type;
		ListB<Byte> _data;
		$JSON::Obj _plain;

		auto & read($JSON::Obj const & src) {
			src["ID"].getX(_id);
			src["Type"].getX(_type);
			StrRef data;
			data.set(src["Data"].getStr());
			_data.alloc(calcSzFromHexStr(data._len));
			readHexStr(data, _data._item, _data._sz);
			data.clear();
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			setIDMaybeHasName(_id, dst.add("ID").getLast()._val);
			dst.add("Type", _type);
			dst.add("Data", writeHexStr(_data._item, _data._cap, Str().setNull()));
			if (!_plain.isNull())
				dst.add("Plain", _plain);
			return dst;
		}
		auto & parse($Struct::$HIRC_Sub_Begin::Member const & begin, Byte const * const data, SzI const & dataSz) {
			_id = begin._id;
			_type = begin._type;
			_data.cpy(data, dataSz);
			return *this;
		}
	};
}
