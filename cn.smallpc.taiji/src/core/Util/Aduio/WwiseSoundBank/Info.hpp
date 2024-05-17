#pragma once

#include "./Struct.hpp"

#include "../../../Type.hpp"
#include "../../../Type/InfoType.hpp"
#include "../../../Type/Str.hpp"
#include "../../../Type/Map.hpp"
#include "../../../Type/JSON.hpp"
#include "../../Data/Hasher/FNV.hpp"

namespace $TJ::$Util::$Audio::$WwiseSoundBank {

	using IDType = IU32;

	inline Map<IDType, Str> gNameMapper;

	inline auto calcID32(Str const & name) {
		Str s(name);
		$Str::toLower(s);
		return $Data::$Hasher::$FNV::$FNV1::hash32(ByteSection().ref(reinterpret_cast<Byte const *>(s.$data), s.$sz));
	}

	inline auto & setIDMaybeHasName(IDType const & id, $JSON::Val & dst) {
		gNameMapper.alloc();
		if (IDType i; gNameMapper.findKey(id, i)) {
			log.txtln("::%s", gNameMapper[i].$data);
			dst.set(gNameMapper[i]);
		} else {
			dst.set(id);
		}
		return dst;
	}

}

namespace $TJ::$Util::$Audio::$WwiseSoundBank::$Info {

	struct BKHD {

		Sz $id;
		Sz $ver;

		auto & read($JSON::Obj const & t) {
			t["id"_r].get($id);
			t["ver"_r].get($ver);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			setIDMaybeHasName($id, t("id"_r));
			t("ver"_r).set($ver);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::BKHD_Begin const & dst) {
			$id = dst.$id;
			$ver = dst.$ver;
			return thiz;
		}

	};

	struct DIDX {

		IU32 $id;

		auto & read($JSON::Val const & t) {
			t.get($id);
			return thiz;
		}
		auto & write($JSON::Val & t) const {
			setIDMaybeHasName($id, t);
			return t;
		}

		auto & parse($Struct::DIDX_Sub const & src) {
			$id = src.$id;
			return thiz;
		}

	};

	struct STID {

		Str $name;

		auto & read($JSON::Val const & t) {
			t.get($name);
			return thiz;
		}
		auto & write($JSON::Val & t) const {
			t.set($name);
			return t;
		}

		auto & parse($Struct::STID_Sub const & t) {
			if (calcID32(t.$name.$str) != t.$id)
				throw VAMGException("WwiseSoundBank-Info", "calcID(%s) = %u, should %u", t.$name.$str.$data, calcID32(t.$name.$str), t.$id);
			$name = t.$name.$str;
			return thiz;
		}

	};

	struct HIRC {

		IU32 $id;
		IU8 $type;
		Section<Byte> $data;
		$JSON::Obj $plain;

		auto & read($JSON::Obj const & t) {
			t["id"_r].get($id);
			t["type"_r].get($type);
			$Str::readHexStr(t["data"_r].getStr(), $data);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			setIDMaybeHasName($id, t("id"_r));
			t("type"_r).set($type);
			$Str::writeHexStr($data, t("data"_r).setStr().getStr());
			if (!$plain.isNull())
				t("plain"_r).set($plain);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::HIRC_Sub_Begin const & begin, ByteSection const & data) {
			$id = begin.$id;
			$type = begin.$ty;
			$data = data;
			return thiz;
		}

	};

}
