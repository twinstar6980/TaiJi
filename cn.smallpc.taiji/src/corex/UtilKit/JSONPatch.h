#pragma once

# include "../Type.h"
# include "../MyType/Path.h"
# include "../MyType/ListOfOther.h"
# include "../MyType/Map.h"
# include "../MyType/JSON.h"

namespace $TJ::$JSON::$Patch {

	struct JSONPath {
		Arr _path;

		auto cmp(JSONPath const & t) const { return _path == t._path; }
		auto & cpy(JSONPath const & t) {
			_path = t._path;
			return *this;
		}

		auto getVal(Val & src, SzI lv = kSzNull) const {
			if (lv == kSzNull)
				lv = _path._sz;
			auto cur = &src;
			for_cnt(i, kThFirst, lv) {
				if (_path[i].isTypeUInt() && (*cur).isTypeArr()) {
					cur = &(*cur)[_path[i].getUInt()];
				} elif(_path[i].isTypeStr() && (*cur).isTypeObj()) {
					cur = &(*cur)[_path[i].getStr()];
				} else {
					cur = kNullPtr;
					break;
				}
			}
			return cur;
		}
		auto & assign(SzI const & v) {
			_path.add().last().set(v);
			return *this;
		}
		auto & assign(Str const & v) {
			_path.add().last().set(v);
			return *this;
		}
		auto & eraseLast() {
			_path.eraseLast();
			return *this;
		}

	};

	enum class SimpleValType : UI8 { kNull, kBool, kNum, kStr, kArr, kObj };

	ListP<JSONPath> & clearRepeat(ListP<JSONPath> & t);

	inline auto getSimpleType(ValType const & type) {
		SimpleValType result;
		switch (type) {
			case ValType::kNull:
				result = SimpleValType::kNull;
				break;
			case ValType::kBool:
				result = SimpleValType::kBool;
				break;
			case ValType::kUInt:
			case ValType::kSInt:
			case ValType::kFloat:
				result = SimpleValType::kNum;
				break;
			case ValType::kStr:
				result = SimpleValType::kStr;
				break;
			case ValType::kArr:
				result = SimpleValType::kArr;
				break;
			case ValType::kObj:
				result = SimpleValType::kObj;
				break;
		}
		return result;
	}
	inline auto getSimpleType(Val const & val) { return getSimpleType(val._type); }
	inline auto getSimpleType(Str const & type) {
		SimpleValType result;
		if (type == "null") {
			result = SimpleValType::kNull;
		} elif(type == "bool") {
			result = SimpleValType::kBool;
		} elif(type == "num") {
			result = SimpleValType::kNum;
		} elif(type == "str") {
			result = SimpleValType::kStr;
		} elif(type == "arr") {
			result = SimpleValType::kArr;
		} elif(type == "obj") {
			result = SimpleValType::kObj;
		}
		return result;
	}

	inline auto getNum(Val const & v) {
		F64 result;
		switch (v._type) {
			default: break;
			case ValType::kUInt: result = v.getUInt(); break;
			case ValType::kSInt: result = v.getSInt(); break;
			case ValType::kFloat: result = v.getFloat(); break;
		}
		return result;
	}

	struct ValSelector {
		Str _oper;
		Arr _data;

		auto test(Val const & v) const {
			auto result = kFalse;
			auto needInverse = _oper[kThFirst] == '!';
			Str commonOper(_oper, needInverse ? kThSecond : kThFirst);
			if (commonOper == "all") {
				result = kTrue;
			} elif(commonOper == "ty") {
				result = getSimpleType(v) == getSimpleType(_data[kThFirst]);
			} elif(commonOper == "eq") { // eq, neq, lt, bt, nlt, nbt
				auto const & val = _data[kThFirst];
				if (getSimpleType(v) == getSimpleType(val)) {
					switch (getSimpleType(v)) {
						default: result = kFalse; break;
						case SimpleValType::kNum:
							result = getNum(v) == getNum(val);
							break;
						case SimpleValType::kNull:
						case SimpleValType::kBool:
						case SimpleValType::kStr:
						case SimpleValType::kArr:
						case SimpleValType::kObj:
							result = v == val;
							break;
					}
				}
			} elif(commonOper == "lt") {
				auto const & val = _data[kThFirst];
				if (getSimpleType(v) == getSimpleType(val)) {
					switch (getSimpleType(v)) {
						default: result = kFalse; break;
						case SimpleValType::kNum:
							result = getNum(v) < getNum(val);
							break;
						case SimpleValType::kStr:
							result = v.getStr() < val.getStr();
							break;
					}
				}
			} elif(commonOper == "bt") {
				auto const & val = _data[kThFirst];
				if (getSimpleType(v) == getSimpleType(val)) {
					switch (getSimpleType(v)) {
						default: result = kFalse; break;
						case SimpleValType::kNum:
							result = getNum(v) > getNum(val);
							break;
						case SimpleValType::kStr:
							result = v.getStr() > val.getStr();
							break;
					}
				}
			} elif(commonOper == "ek") {
				auto const & val = _data[kThFirst];
				if (v.isTypeObj()) {
					for_criter(e, v.getObj()) {
						if (e._key == val) {
							result = kTrue;
							break;
						}
					}
				} elif(v.isTypeArr()) {
					for_cnt(i, kThFirst, v.getArr()._sz) {
						if (Val().set(i) == val) {
							result = kTrue;
							break;
						}
					}
				}
			} elif(commonOper == "ev") {
				auto const & val = _data[kThFirst];
				if (v.isTypeObj()) {
					for_criter(e, v.getObj()) {
						if (e._val == val) {
							result = kTrue;
							break;
						}
					}
				} elif(v.isTypeArr()) {
					for_criter(e, v.getArr()) {
						if (e == val) {
							result = kTrue;
							break;
						}
					}
				}
			}
			return needInverse ? !result : result;
		}
	};

	struct ElementSelector {
		Str _tgt;
		ValSelector _selector;

		auto & read(Arr const & src) {
			_tgt = src[kThFirst].getStr();
			_selector._oper = src[kThSecond].getStr();
			_selector._data.setNull();
			for_cnt(i, kThThird, src._sz) {
				_selector._data.add(src[i]);
			}
			return *this;
		}

		auto & select(Val & doc, ListP<JSONPath> const & src, ListP<JSONPath> & dst) const {
			dst.setNull();
			for_criter(e, src) {
				auto vp = e.getVal(doc);
				if (vp == kNullPtr)
					continue;
				if (_tgt == "this-key") {
					if (_selector.test(e._path.last()))
						dst.add(e);
				} elif(_tgt == "this-val") {
					if (_selector.test(*vp))
						dst.add(e);
				} elif(_tgt == "child-key") {
					if ((*vp).isTypeArr()) {
						for_cnt(i, kThFirst, (*vp).getArr()._sz) {
							if (_selector.test(Val().set(i))) {
								dst.add(e).last().assign(i);
							}
						}
					} elif((*vp).isTypeObj()) {
						//wLogLn("at");
						for_criter(m, (*vp).getObj()) {
							//wLogLn("%s", m._key.get()._str);
							if (_selector.test(Val().set(m._key.get()))) {
								dst.add(e).last().assign(m._key.getStr());
							}
						}
					}
				} elif(_tgt == "child-val") {
					if ((*vp).isTypeArr()) {
						for_cnt(i, kThFirst, (*vp).getArr()._sz) {
							if (_selector.test(Val().set((*vp)[i]))) {
								dst.add(e).last().assign(i);
							}
						}
					} elif((*vp).isTypeObj()) {
						for_criter(m, (*vp).getObj()) {
							if (_selector.test(Val().set(m._val))) {
								dst.add(e).last().assign(m._key.getStr());
							}
						}
					}
				} elif(_tgt == "parent-key") {
					if (_selector.test(e._path.get(e._path.lastIdx() - 1)))
						dst.add(e).last().eraseLast();
				} elif(_tgt == "parent-val") {
					if (_selector.test(*e.getVal(doc, e._path._sz - 1)))
						dst.add(e).last().eraseLast();
				}
			}
			clearRepeat(dst);
			return dst;
		}
	};
	using TierSelector = ElementSelector;
	/*
	struct TierSelector {
		ListP<ListP<ElementSelector>> _selector;
	};*/
	struct JSONSelector {
		ListOfInfo<TierSelector> _tier;

		auto & read(Arr const & src) {
			_tier.readByArrStyle(src);
			return *this;
		}

		auto & select(Val & doc, ListP<JSONPath> const & src, ListP<JSONPath> & dst) const {
			dst = src;
			for_criter(e, _tier) {
				auto newSelector = new ListP<JSONPath>;
				e.select(doc, dst, *newSelector);
				dst = *newSelector;
				(*newSelector).tfree();
			}
			return dst;
		}
	};

	struct Modifyer {
		Str _type;
		Val _data;

		auto & read(Obj const & src) {
			src["Type"].getX(_type);
			_data = src["Data"];
			return *this;
		}
		auto & apply(Val & doc, JSONPath const & path) const {
			auto vp = path.getVal(doc);
			if (vp != kNullPtr) {
				if (_type == "remove") {
					(*vp).setUndef();
				} elif(_type == "replace") {
					*vp = _data;
				} elif(_type == "append-member") {
					if ((*vp).isTypeObj()) {
						for_criter(e, _data.getArr()) {
							(*vp).getObj().add(e[kThFirst].getStr(), e[kThSecond]);
						}
					}
				} elif(_type == "append-element") {
					if ((*vp).isTypeArr()) {
						for_criter(e, _data.getArr()) {
							(*vp).getArr().add(e);
						}
					}
				}
			}
			return *this;
		}
		auto & apply(Val & doc, ListP<JSONPath> const & path) const {
			for_criter(e, path) {
				apply(doc, e);
			}
			return *this;
		}
	};

	struct Patch {
		JSONSelector _selector;
		Modifyer _modifyer;

		auto & read(Obj const & src) {
			_selector.read(src["Selector"].getArr());
			_modifyer.read(src["Modifyer"].getObj());
			return *this;
		}

	};

	Val & apply(Patch const & patch, Val & data);
	Val & apply(ListP<Patch> const & patch, Val & data);
	Void apply(Path const & patchFile, Path const & srcFile, Path const & dstFile);

}
