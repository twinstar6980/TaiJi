#pragma once

#include "../../Type.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/InfoType.hpp"
#include "../../Type/Map.hpp"
#include "../../Type/JSON.hpp"
#include "../../UserLiteral.hpp"

namespace $TJ::$Util::$Expand::$JSONPatch {

	enum class SimpleValType : IU8 { kNull, kBool, kNum, kStr, kArr, kObj };

	List<$JSON::ValPath> & clearRepeat(List<$JSON::ValPath> & t);

	inline auto getSimpleType($JSON::ValType const & t) {
		SimpleValType r;
		switch (t) {
			case $JSON::ValType::kNull:
				r = SimpleValType::kNull;
				break;
			case $JSON::ValType::kBool:
				r = SimpleValType::kBool;
				break;
			case $JSON::ValType::kIntU:
			case $JSON::ValType::kIntS:
			case $JSON::ValType::kFloat:
				r = SimpleValType::kNum;
				break;
			case $JSON::ValType::kStr:
				r = SimpleValType::kStr;
				break;
			case $JSON::ValType::kArr:
				r = SimpleValType::kArr;
				break;
			case $JSON::ValType::kObj:
				r = SimpleValType::kObj;
				break;
		}
		return r;
	}
	inline auto getSimpleType($JSON::Val const & t) {
		return getSimpleType(t.$$ty());
	}
	inline auto getSimpleType(Str const & t) {
		SimpleValType r;
		switch (t.hashByPtr(operator""_sh)) {
			default:
				r = SimpleValType::kNull;
				break;
			case "null"_sh:
				r = SimpleValType::kNull;
				break;
			case "bool"_sh:
				r = SimpleValType::kBool;
				break;
			case "num"_sh:
				r = SimpleValType::kNum;
				break;
			case "str"_sh:
				r = SimpleValType::kStr;
				break;
			case "arr"_sh:
				r = SimpleValType::kArr;
				break;
			case "obj"_sh:
				r = SimpleValType::kObj;
				break;
		}
		return r;
	}

	inline auto getNum($JSON::Val const & t) {
		F64 r;
		switch (t.$$ty()) {
			default:
				throw VAMGException("JSONPatch::getNum()", "value-type not a number");
				break;
			case $JSON::ValType::kIntU: r = t.getIntU(); break;
			case $JSON::ValType::kIntS: r = t.getIntS(); break;
			case $JSON::ValType::kFloat: r = t.getFloat(); break;
		}
		return r;
	}

	struct ValSelector {

		Str $oper;
		$JSON::Arr $data;

		auto select($JSON::Val const & v) const {
			auto r = kBoF;
			auto needInverse = $oper.first() == '!';
			Str commonOper;
			commonOper.ref($oper, needInverse ? kNo2 : kNo1);
			switch (commonOper.hashByPtr(operator""_sh)) {
				default:
					throw VAMGException("JSONPatch::ValSelector::select", "unknown operator");
					break;
				case "all"_sh:
					r = kBoT;
					break;
				case "ty"_sh:
					r = getSimpleType(v) == getSimpleType($data.first());
					break;
				case "eq"_sh:
					if (auto const & val = $data.first(); getSimpleType(v) == getSimpleType(val)) {
						switch (getSimpleType(v)) {
							default:
								r = kBoF;
								break;
							case SimpleValType::kNum:
								r = getNum(v) == getNum(val);
								break;
							case SimpleValType::kNull:
							case SimpleValType::kBool:
							case SimpleValType::kStr:
							case SimpleValType::kArr:
							case SimpleValType::kObj:
								r = v == val;
								break;
						}
					}
					break;
				case "lt"_sh:
					if (auto const & val = $data.first(); getSimpleType(v) == getSimpleType(val)) {
						switch (getSimpleType(v)) {
							default:
								r = kBoF;
								break;
							case SimpleValType::kNum:
								r = getNum(v) < getNum(val);
								break;
							case SimpleValType::kStr:
								r = v.getStr() < val.getStr();
								break;
						}
					}
					break;
				case "gt"_sh:
					if (auto const & val = $data.first(); getSimpleType(v) == getSimpleType(val)) {
						switch (getSimpleType(v)) {
							default:
								r = kBoF;
								break;
							case SimpleValType::kNum:
								r = getNum(v) > getNum(val);
								break;
							case SimpleValType::kStr:
								r = v.getStr() > val.getStr();
								break;
						}
					}
					break;
				case "ek"_sh: {
					auto const & val = $data.first();
					if (v.isObj()) {
						r = v.getObj().range().existBy([&val](auto & v) { return v.$k == val; });
					} else if (v.isArr()) {
						for (auto i = kNo1; i < v.getArr().$sz; ++i) {
							if ($JSON::Val().set(i) == val) {
								r = kBoT;
								break;
							}
						}
					}
					break;
				}
				case "ev"_sh: {
					auto const & val = $data.first();
					if (v.isObj()) {
						r = v.getObj().range().existBy([&val](auto & v) { return v.$v == val; });
					} else if (v.isArr()) {
						r = v.getArr().range().existBy([&val](auto & v) { return v == val; });
					}
					break;
				}
				case "regex"_sh:
					r = $Str::regexMatch($data.first().getStr(), v.getStr());
					break;
			}
			return needInverse ? !r : r;
		}

	};

	struct ElementSelector {

		Str $tgt;
		ValSelector $selector;

		auto & read($JSON::Arr const & t) {
			$tgt = t[kNo1].getStr();
			$selector.$oper = t[kNo2].getStr();
			$selector.$data.cpy($JSON::Arr().ref(t, kNo3));
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2 + $selector.$data.$sz);
			t.add($tgt).add($selector.$oper);
			for (auto const & e : $selector.$data)
				t.add(e);
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto & select($JSON::Val const & doc, List<$JSON::ValPath> const & src, List<$JSON::ValPath> & dst) const {
			dst.alloc(src.$sz);
			for (auto const & e : src) {
				auto vp = $JSON::getValByPath(e, const_cast<$JSON::Val &>(doc));
				if (vp == kNullPtr)
					continue;
				switch ($tgt.hashByPtr(operator""_sh)) {
					default:
						throw VAMGException("JSONPatch::ElementSelector::select", "unknown target");
						break;
					case ".k"_sh:
						if ($selector.select(e.$path.last().toJSONVal()))
							dst.append(e);
						break;
					case ".v"_sh:
						if ($selector.select(*vp))
							dst.append(e);
						break;
					case "k"_sh:
						if ((*vp).isArr()) {
							for (auto i = kNo1; i < (*vp).getArr().$sz; ++i) {
								if ($selector.select($JSON::Val().set(i))) {
									dst.append(e).last().assign(i);
								}
							}
						} else if ((*vp).isObj()) {
							for (auto const & m : (*vp).getObj()) {
								if ($selector.select(m.$k)) {
									dst.append(e).last().assign(m.$k.getStr());
								}
							}
						}
						break;
					case "v"_sh:
						if ((*vp).isArr()) {
							for (auto i = kNo1; i < (*vp).getArr().$sz; ++i) {
								if ($selector.select((*vp)[i])) {
									dst.append(e).last().assign(i);
								}
							}
						} else if ((*vp).isObj()) {
							for (auto const & m : (*vp).getObj()) {
								if ($selector.select(m.$v)) {
									dst.append(e).last().assign(m.$k.getStr());
								}
							}
						}
						break;
					case "..k"_sh:
						if ($selector.select(e.$path[e.$path.lastIdx() - kNo2].toJSONVal()))
							dst.append(e).last().eraseLast();
						break;
					case "..v"_sh:
						if ($selector.select(*$JSON::getValByPath(e, const_cast<$JSON::Val &>(doc), e.$path.$sz - kSz1)))
							dst.append(e).last().eraseLast();
						break;
				}
			}
			clearRepeat(dst);
			return dst;
		}

	};

	struct Selector {

		InfoList<ElementSelector> $tier;

		auto & read($JSON::Arr const & t) {
			$tier.read(t);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			$tier.write(t);
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto & select($JSON::Val const & doc, List<$JSON::ValPath> const & src, List<$JSON::ValPath> & dst) const {
			dst = src;
			for (auto const & e : $tier) {
				List<$JSON::ValPath> newPath;
				e.select(doc, dst, newPath);
				dst.free().bind(newPath);
				newPath.unbind();
			}
			return dst;
		}

	};

	struct Modifyer {

		Str $type;
		$JSON::Val $data;

		auto & read($JSON::Arr const & t) {
			t.first().get($type);
			if (t.$sz > kSz1) {
				$data.cpy(t[kNo2]);
			} else {
				$data.setUndef();
			}
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2);
			t.add($type);
			if (!$data.isUndef()) {
				t.add($data);
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto & apply($JSON::Val & doc, $JSON::ValPath const & path) const {
			if (auto vp = $JSON::getValByPath(path, doc); vp != kNullPtr) {
				switch ($type.hashByPtr(operator""_sh)) {
					default:
						throw VAMGException("JSONPatch::Modifyer::apply", "unknown type");
						break;
					case "remove"_sh:
						(*vp).setUndef();
						break;
					case "replace"_sh:
						*vp = $data;
						break;
					case "append-member"_sh:
						if ((*vp).isObj()) {
							(*vp).getObj().append($data.getObj());
						}
						break;
					case "append-element"_sh:
						if ((*vp).isArr()) {
							(*vp).getArr().append($data.getArr());
						}
						break;
				}
			}
			return thiz;
		}
		auto & apply($JSON::Val & doc, List<$JSON::ValPath> const & path) const {
			for (auto const & e : path)
				apply(doc, e);
			return thiz;
		}

	};

	struct Patch {

		ExistShell<InfoList<$JSON::ValPath>> $path;
		ExistShell<Selector> $selector;
		ExistShell<Modifyer> $modifyer;
		InfoList<Patch> $inherit;

		auto & read($JSON::Obj const & t) {
			if ($path.$state = t.exist("path"_r))
				$path.$data.read(t["path"_r]);
			if ($selector.$state = t.exist("selector"_r))
				$selector.$data.read(t["selector"_r]);
			if ($modifyer.$state = t.exist("modifyer"_r))
				$modifyer.$data.read(t["modifyer"_r]);
			if (t.exist("inherit"_r))
				$inherit.read(t["inherit"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			if ($path.$state)
				$path.$data.write(t("path"_r));
			if ($selector.$state)
				$selector.$data.write(t("selector"_r));
			if ($modifyer.$state)
				$modifyer.$data.write(t("modifyer"_r));
			if (!$inherit.isNull())
				$inherit.write(t("inherit"_r));
			return thiz;
		}

		Patch & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		Patch const & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	namespace $Apply {

		extern $JSON::Val & apply(Patch const & patch, $JSON::ValPath const & initPath, $JSON::Val & data);
		extern $JSON::Val & apply(Patch const & patch, List<$JSON::ValPath> const & initPath, $JSON::Val & data);
		extern $JSON::Val & apply(List<Patch> const & patch, List<$JSON::ValPath> const & initPath, $JSON::Val & data);
		extern $JSON::Val & apply(List<Patch> const & patch, $JSON::ValPath const & initPath, $JSON::Val & data);
		extern $JSON::Val & apply(List<Patch> const & patch, $JSON::Val & data);

		extern Void applyExport(Path const & patchFile, Path const & srcFile, Path const & dstFile);

	}

	namespace $Diff {

		extern Void diffExport(Path const & srcFile, Path const & dstFile, Path const & patchFile);

	}

}
