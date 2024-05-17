# pragma once

# include "../Type.h"
# include "./Str.h"
# include "./ListP.h"

namespace $TJ::$JSON {

	struct Val;
	struct Key;
	struct Member;
	struct Obj;
	struct Arr;

	enum class ValType : UI8 { kUndef, kNull, kObj, kArr, kBool, kUInt, kSInt, kFloat, kStr };

	tmpl_st(T) constexpr auto isTVal() { return cmpT<T, Val>(); }
	tmpl_st(T) constexpr auto isTObj() { return cmpT<T, Obj>(); }
	tmpl_st(T) constexpr auto isTArr() { return cmpT<T, Arr>(); }

	tmpl_st(T) constexpr auto typeIs() {
		goif(isTObj<T>()) return ValType::kObj;
		elif(isTArr<T>()) return ValType::kArr;
		elif(isTBool<T>()) return ValType::kBool;
		elif(isTUInt<T>()) return ValType::kUInt;
		elif(isTSInt<T>()) return ValType::kSInt;
		elif(isTFolat<T>()) return ValType::kFloat;
		elif(isTStr<T>()) return ValType::kStr;
			else return ValType::kUndef;
	}

	tmpl_st(T) constexpr auto isTErr() { return typeIs<T>() == ValType::kUndef; }

	struct Val {
		ValType _type;
		Void * _val;

		Val & set(ValType const & type = ValType::kUndef);

		auto & setUndef() { return set(ValType::kUndef); }
		auto & setNull() { return set(ValType::kNull); }
		auto & setObj() { return set(ValType::kObj); }
		auto & setArr() { return set(ValType::kArr); }
		auto & setBool() { return set(ValType::kBool); }
		auto & setUInt() { return set(ValType::kUInt); }
		auto & setSInt() { return set(ValType::kSInt); }
		auto & setFloat() { return set(ValType::kFloat); }
		auto & setStr() { return set(ValType::kStr); }

		tmpl_st(T) auto & set(T const & val, Bool const & nullOrNot = kFalse) {
			if (nullOrNot) return setNull();
			ifcexp(isTVal<T>()) return cpy(val);
			clear();
			goifcexp(isTObj<T>()) {
			# if !defined PLATFORM_ANDROID
				_type = ValType::kObj; _val = new Obj(val);
			# endif
			}
			elifcexp(isTArr<T>()) {
			# if !defined PLATFORM_ANDROID
				_type = ValType::kArr; _val = new Arr(val);
			# endif
			}
			elifcexp(isTBool<T>()) { _type = ValType::kBool; _val = new Bool(val); }
			elifcexp(isTUInt<T>()) { _type = ValType::kUInt; _val = new UI64(static_cast<UI64>(val)); }
			elifcexp(isTSInt<T>()) { _type = ValType::kSInt; _val = new SI64(static_cast<SI64>(val)); }
			elifcexp(isTFolat<T>()) { _type = ValType::kFloat; _val = new F64(static_cast<F64>(val)); }
			elifcexp(isTStr<T>()) { _type = ValType::kStr; _val = new Str(val); }
			elifcexp(isTErr<T>()) { _type = ValType::kUndef; _val = kNullPtr; } else wErr("UnKnown ValType");
			return *this;
		}
	# if defined PLATFORM_ANDROID
		Val & set(Obj const & val, Bool const & nullOrNot = kFalse);
		Val & set(Arr const & val, Bool const & nullOrNot = kFalse);
	# endif
		Val & clear() {
			switch (_type) {
				default: wErr("UnKnown ValType"); break;
				case ValType::kUndef: break;
				case ValType::kNull: break;
				case ValType::kObj: delVoidPtrSingle<Obj>(_val); break;
				case ValType::kArr: delVoidPtrSingle<Arr>(_val); break;
				case ValType::kBool: delVoidPtrSingle<Bool>(_val); break;
				case ValType::kUInt: delVoidPtrSingle<UI64>(_val); break;
				case ValType::kSInt: delVoidPtrSingle<SI64>(_val); break;
				case ValType::kFloat: delVoidPtrSingle<F64>(_val); break;
				case ValType::kStr: delVoidPtrSingle<Str>(_val); break;
			}
			return *this;
		}

		Val & cpy(Val const & t);

		tmpl_st(T) auto isType() const { return _type == typeIs<T>(); }

		auto isTypeUndef() const { return _type == ValType::kUndef; }
		auto isTypeNull() const { return _type == ValType::kNull; }
		auto isTypeObj() const { return _type == ValType::kObj; }
		auto isTypeArr() const { return _type == ValType::kArr; }
		auto isTypeBool() const { return _type == ValType::kBool; }
		auto isTypeUInt() const { return _type == ValType::kUInt; }
		auto isTypeSInt() const { return _type == ValType::kSInt; }
		auto isTypeInt() const { return _type == ValType::kUInt || _type == ValType::kSInt; }
		auto isTypeFloat() const { return _type == ValType::kFloat; }
		auto isTypeStr() const { return _type == ValType::kStr; }

		tmpl_st(T) auto & get() {
			goifcexp(isTObj<T>()) return *static_cast<Obj *>(_val);
			elifcexp(isTArr<T>()) return *static_cast<Arr *>(_val);
			elifcexp(isTBool<T>()) return *static_cast<Bool *>(_val);
			elifcexp(isTUInt<T>()) return *static_cast<UI64 *>(_val);
			elifcexp(isTSInt<T>()) return *static_cast<SI64 *>(_val);
			elifcexp(isTFolat<T>()) return *static_cast<F64 *>(_val);
			elifcexp(isTStr<T>()) return *static_cast<Str *>(_val);
			else wErr("UnKnown ValType At GetVal");
		}
		tmpl_st(T) auto const & get() const {
			goifcexp(isTObj<T>()) return *static_cast<Obj const *>(_val);
			elifcexp(isTArr<T>()) return *static_cast<Arr const *>(_val);
			elifcexp(isTBool<T>()) return *static_cast<Bool const *>(_val);
			elifcexp(isTUInt<T>()) return *static_cast<UI64 const *>(_val);
			elifcexp(isTSInt<T>()) return *static_cast<SI64 const *>(_val);
			elifcexp(isTFolat<T>()) return *static_cast<F64 const *>(_val);
			elifcexp(isTStr<T>()) return *static_cast<Str const *>(_val);
			else wErr("UnKnown ValType At GetVal");
		}
		tmpl_st(T) T & geaxt(T & dflt) {
			ifcexp(typeIs<T>() == ValType::kSInt) return (_type == ValType::kSInt || _type == ValType::kUInt) ? get<T>() : dflt;
				else return (typeIs<T>() == _type) ? get<T>() : dflt;
		}
		tmpl_st(T) T const & geaxt(T const & dflt) const {
			ifcexp(typeIs<T>() == ValType::kSInt) return (_type == ValType::kSInt || _type == ValType::kUInt) ? get<T>() : dflt;
				else return (typeIs<T>() == _type) ? get<T>() : dflt;
		}

		tmpl_st(T) auto & getX(T & dst) const {
			return dst = static_cast<T>(get<T>());
		}
		tmpl_st(T) auto const & getX(T & dst, T const & dflt) const {
			ifcexp(typeIs<T>() == ValType::kSInt) return dst = (_type == ValType::kSInt || _type == ValType::kUInt) ? static_cast<T const>(get<T>()) : dflt;
			return dst = (typeIs<T>() == _type) ? static_cast<T const>(get<T>()) : dflt;
		}

		auto & getObj() { return get<Obj>(); }
		auto & getArr() { return get<Arr>(); }
		auto & getBool() { return get<Bool>(); }
		auto & getUInt() { return get<UI64>(); }
		auto & getSInt() { return get<SI64>(); }
		auto & getFloat() { return get<F64>(); }
		auto & getStr() { return get<Str>(); }

		auto & getObj() const { return get<Obj>(); }
		auto & getArr() const { return get<Arr>(); }
		auto & getBool() const { return get<Bool>(); }
		auto & getUInt() const { return get<UI64>(); }
		auto & getSInt() const { return get<SI64>(); }
		auto & getFloat() const { return get<F64>(); }
		auto & getStr() const { return get<Str>(); }

		Bool cmp(Val const & t) const;

		Val & read(Ch * const & src, SzI & pos);
		Void write(Ch * const & dst, SzI & pos, SzI tabLv = kSzZero) const;

		~Val() { clear(); }
		Val() : _type(ValType::kUndef), _val(kNullPtr) {}
		explicit Val(ValType const & type) : Val() { set(type); }
		tmpl_st(T) explicit Val(T const & val) : Val() { set(val); }

		inline Val & operator[](SzI const & idx);
		inline Val & operator[](Str const & key);

		inline Val const & operator[](SzI const & idx) const;
		inline Val const & operator[](Str const & key) const;

		auto & operator[](Ch const * const & key) { return (*this)[StrRef(key)]; }
		auto & operator[](Ch const * const & key) const { return (*this)[StrRef(key)]; }

		auto operator==(Val const & t) { return cmp(t); }
		auto operator!=(Val const & t) { return !cmp(t); }

		auto & operator=(Val const & t) { return cpy(t); }

		auto & gtVal() { return *this; }
		auto & gtVal() const { return *this; }
	};

	struct Key : Val {

		auto isOK() const { return _type == ValType::kStr; }

		auto & get() { return getStr(); }
		auto & get() const { return getStr(); }

		auto & review() {
			if (!isOK()) setUndef();
			return *this;
		}

		Key & read(Ch * const & src, SzI & pos);
		Void write(Ch * const & dst, SzI & pos) const;

		Key() {}
		Key(Key const & t) : Val(t) {}
		explicit Key(Str const & key) : Val(key) {}
		explicit Key(Ch const * const & key) : Val(StrRef(key)) {}
	};

	struct Member {
		Key _key;
		Val _val;

		auto & set(Key const & key, Val const & val) {
			_key = key;
			_val = val;
			return *this;
		}
		auto & set(Key const & key, ValType const & valType = ValType::kUndef) {
			_key = key;
			_val.set(valType);
			return *this;
		}
		auto & set(Str const & key, Val const & val) {
			_key.set(key);
			_val = val;
			return *this;
		}
		auto & set(Str const & key, ValType const & valType = ValType::kUndef) {
			_key.set(key);
			_val.set(valType);
			return *this;
		}

		auto & cpy(Member const & src) {
			_key = src._key;
			_val = src._val;
			return *this;
		}

		auto & clear() {
			_key.clear();
			_val.clear();
			return *this;
		}

		auto cmp(Member const & t) const { return _key.cmp(t._key) && _val.cmp(t._val); }

		Member() {}
		Member(Member const & src) { cpy(src); }
		explicit Member(ValType const & valType) : _val(valType) {}
		explicit Member(Key const & key, Val const & val) : _key(key), _val(val) {}
		explicit Member(Str const & key, Val const & val) : _key(key), _val(val) {}
		explicit Member(Ch const * const & key, Val const & val) : _key(key), _val(val) {}

		auto & operator[](SzI const & idx) { return _val[idx]; }
		auto & operator[](Str const & key) { return _val[key]; }

		auto & operator[](SzI const & idx) const { return _val[idx]; }
		auto & operator[](Str const & key) const { return _val[key]; }

		auto & operator[](Ch const * const & key) { return _val[key]; }
		auto & operator[](Ch const * const & key) const { return _val[key]; }

		auto operator==(Member const & t) { return cmp(t); }
		auto operator!=(Member const & t) { return !cmp(t); }

		auto & operator=(Member const & t) { return cpy(t); }

	};

	struct Arr : ListP<Val> {

		tmpl_st(T) auto & add(T const & val, Bool const & nullOrNot = kFalse) {
			getListP().appendSz().last().set(val, nullOrNot);
			return *this;
		}
		auto & add(ValType const & type = ValType::kUndef) {
			getListP().appendSz().last().set(type);
			return *this;
		}
		auto & addUndef() { return add(ValType::kUndef); }
		auto & addNull() { return add(ValType::kNull); }
		auto & addObj() { return add(ValType::kObj); }
		auto & addArr() { return add(ValType::kArr); }

		Arr & read(Ch * const & src, SzI & pos);
		Void write(Ch * const & dst, SzI & pos, SzI tabLv = kSzZero) const;

		auto & gtArr() { return *this; }
		auto & gtArr() const { return *this; }
	};

	struct Obj : ListP<Member> {

		auto & appendSz(SzI const & sz) { getListP().appendSz(sz); return *this; }
		auto & append(Member const & src) { getListP().append(src); return *this; }

		auto find(Str const & key) const {
			precond(!isNull());
			SzI i;
			for_cnt_ev(i, kThFirst, _sz)
				if (get(i)._key.isOK() && get(i)._key.get() == key)
					break;
			return (i == _sz) ? kThNull : i;
		}
		auto find(Str const & key, SzI & i) const { return (i = find(key)) != kThNull; }
		auto exist(Str const & key) const { return find(key) != kThNull; }

		auto find(Ch const * const & str) const { return find(StrRef(str)); }
		auto find(Ch const * const & str, SzI & i) const { return find(StrRef(str), i); }
		auto exist(Ch const * const & str) const { return exist(StrRef(str)); }

		tmpl_st(T) Obj & add(Str const & key, T const & val, Bool const & nullOrNot = kFalse) {
			getListP().appendSz().last().set(key);
			last()._val.set(val, nullOrNot);
			return *this;
		}
		auto & add(Str const & key, ValType const & valType = ValType::kUndef) {
			getListP().appendSz().last().set(key, valType);
			return *this;
		}
		auto & addUndef(Str const & key) { return add(key, ValType::kUndef); }
		auto & addNull(Str const & key) { return add(key, ValType::kNull); }
		auto & addObj(Str const & key) { return add(key, ValType::kObj); }
		auto & addArr(Str const & key) { return add(key, ValType::kArr); }

		tmpl_st(T) auto & add(Ch const * const & key, T const & val, Bool const & nullOrNot = kFalse) { return add(StrRef(key), val, nullOrNot); }
		auto & add(Ch const * const & key, ValType const & valType = ValType::kUndef) { return add(StrRef(key), valType); }
		auto & addUndef(Ch const * const & key) { return add(StrRef(key), ValType::kUndef); }
		auto & addNull(Ch const * const & key) { return add(StrRef(key), ValType::kNull); }
		auto & addObj(Ch const * const & key) { return add(StrRef(key), ValType::kObj); }
		auto & addArr(Ch const * const & key) { return add(StrRef(key), ValType::kArr); }

		Obj & read(Ch * const & src, SzI & pos);
		Void write(Ch * const & dst, SzI & pos, SzI tabLv = kSzZero) const;

		auto & operator[](SzI const & idx) { return get(idx); }
		auto & operator[](Str const & key) { return get(find(key))._val; }

		auto & operator[](SzI const & idx) const { return get(idx); }
		auto & operator[](Str const & key) const { return get(find(key))._val; }

		auto & operator[](Ch const * const & key) { return (*this)[StrRef(key)]; }
		auto & operator[](Ch const * const & key) const { return (*this)[StrRef(key)]; }

		auto & gtObj() { return *this; }
		auto & gtObj() const { return *this; }
	};

	inline Val & Val::operator[](SzI const & idx) { return getArr()[idx]; }
	inline Val & Val::operator[](Str const & key) { return getObj()[key]; }

	inline Val const & Val::operator[](SzI const & idx) const { return getArr()[idx]; }
	inline Val const & Val::operator[](Str const & key) const { return getObj()[key]; }

	extern Val const kDfltErrVal;
	extern Member const kDfltErrMember;

	namespace $StyleCtrl {
		Void setStyle(Bool const & outNewLineIfElement);
	}

	$JSON::Val & eraseUndef($JSON::Val & t);
}
