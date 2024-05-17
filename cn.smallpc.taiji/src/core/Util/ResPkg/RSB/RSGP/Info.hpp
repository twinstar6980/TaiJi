#pragma once

#include "./Struct.hpp"
#include "../Common.hpp"

#include "../../../../Type.hpp"
#include "../../../../Type/InfoType.hpp"
#include "../../../../Type/ImgType.hpp"
#include "../../../../Type/JSON.hpp"
#include "../../../Image/Type.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$RSGP::$Info {

	struct AtlasInfo {

		Sz $idx;
		ImgSz $sz;

		auto & read($JSON::Obj const & t) {
			t["idx"_r].get($idx);
			$sz.read(t["sz"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			t("idx"_r).set($idx);
			$sz.write(t("sz"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto operator==(AtlasInfo const & t) const {
			return $idx == t.$idx && $sz == t.$sz;
		}
		auto operator!=(AtlasInfo const & t) const { return !operator==(t); }

	};

	struct Res {

		Path $path;
		ResType $type;
		Void * $expand;

		auto & freeExpand() {
			if ($type == ResType::kNormal)
				$expand = kNullPtr;
			else if ($type == ResType::kAtlas)
				pvFreeS<AtlasInfo>($expand);
			$type = ResType::kNormal;
			return thiz;
		}
		auto & free() {
			$path.free();
			freeExpand();
			return thiz;
		}

		auto isType(ResType const & t) const {
			return $type == t;
		}
		auto isNormalType() const {
			return isType(ResType::kNormal);
		}
		auto isAtlasType() const {
			return isType(ResType::kAtlas);
		}

		auto & getAtlasInfo() {
			return *static_cast<AtlasInfo *>($expand);
		}
		auto & getAtlasInfo() const {
			return *static_cast<AtlasInfo const *>($expand);
		}

		auto & setType(ResType const & t) {
			$type = t;
			if (t == ResType::kNormal)
				$expand = kNullPtr;
			else if (t == ResType::kAtlas)
				$expand = new AtlasInfo();
			return thiz;
		}
		auto & setTypeNormal() {
			return setType(ResType::kNormal);
		}
		auto & setTypeAtlas() {
			return setType(ResType::kAtlas);
		}

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			if (!t.exist("atlas"_r))
				setTypeNormal();
			else
				setTypeAtlas().getAtlasInfo().read(t["atlas"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			cnvPathToJSONArr($path, t("path"_r));
			if ($type == ResType::kAtlas)
				getAtlasInfo().write(t("atlas"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & operator=(Res const & t) {
			$path = t.$path;
			$type = t.$type;
			if (t.$type == ResType::kAtlas) {
				$expand = new AtlasInfo();
				getAtlasInfo() = t.getAtlasInfo();
			}
			return thiz;
		}

		auto operator==(Res const & t) const {
			if (!$Path::equal($path, t.$path) || $type != t.$type)
				return kBoF;
			if (isAtlasType())
				return getAtlasInfo() == t.getAtlasInfo();
			return kBoT;
		}
		auto operator!=(Res const & t) const { return !operator==(t); }

		~Res() {
			freeExpand();
		}

	};

	struct ResList : InfoList<Res> {

		auto calcResSum(ResType const & ty) const {
			return range().eachSum(
				[&ty](auto & v) {
					return v.$type == ty ? kSz1 : kSzN;
				}, kSzN
			);
		}

		auto calcNormalResSum() const {
			return calcResSum(ResType::kNormal);
		}
		auto calcAtlasResSum() const {
			return calcResSum(ResType::kAtlas);
		}

	};

	struct Info {

		Sz $headerType;
		ResStoreMethod $resStoreMethod;
		ResList $res;

		auto & init() {
			$headerType = 0;
			$resStoreMethod.set(kBoF, kBoF);
			$res.setNull();
			return thiz;
		}
		auto & free() {
			return init();
		}

		auto & read($JSON::Obj const & t) {
			t["headerType"_r].get($headerType);
			$resStoreMethod.read(t["resStoreMethod"_r]);
			$res.read(t["res"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(3);
			t("headerType"_r).set($headerType);
			$resStoreMethod.write(t("resStoreMethod"_r));
			$res.write(t("res"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

}
