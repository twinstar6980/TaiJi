#pragma once

#include "../Type.hpp"
#include "../TypeUtil.hpp"
#include "./List.hpp"
#include "./Map.hpp"
#include "./Str.hpp"
#include "./Path.hpp"

namespace $TJ::$JSON {

	struct Val;
	struct Member;
	struct Arr;
	struct Obj;

	enum class ValType : IU8 { kUndef, kNull, kObj, kArr, kBool, kIntU, kIntS, kFloat, kStr };

	template <typename T>
	inline constexpr Bool kTIsJSONVal = kTSame<T, Val>;
	template <typename T>
	inline constexpr Bool kTIsJSONArr = kTSame<T, Arr>;
	template <typename T>
	inline constexpr Bool kTIsJSONObj = kTSame<T, Obj>;

	template <typename T>
	inline constexpr auto getType() {
		if constexpr (kTIsBool<T>)
			return ValType::kBool;
		else if constexpr (kTIsIntU<T>)
			return ValType::kIntU;
		else if constexpr (kTIsIntS<T>)
			return ValType::kIntS;
		else if constexpr (kTIsFloat<T>)
			return ValType::kFloat;
		else if constexpr (kTIsStr<T>)
			return ValType::kStr;
		else if constexpr (kTIsJSONArr<T>)
			return ValType::kArr;
		else if constexpr (kTIsJSONObj<T>)
			return ValType::kObj;
		else
			return ValType::kUndef;
	}

	template <typename T>
	inline constexpr auto tIsJSONUndef() {
		return getType<T>() == ValType::kUndef;
	}

#if defined ENABLE_CONCEPTS

	template<typename T>
	concept CTValidJSONVal = requires() {
		requires(!tIsJSONUndef<T>());
	};

#else

#define CTValidJSONVal typename

#endif

	struct Val {

	private:

		using TT = Val;

	private:

		ValType $ty;
		Void * $v;

	public:

		fun_getter(ty);

		auto isType(ValType const & t) const { return $ty == t; }

		template <typename T>
		auto isType() const { return isType(getType<T>()); }

		auto isUndef() const { return isType(ValType::kUndef); }
		auto isNull() const { return isType(ValType::kNull); }
		auto isBool() const { return isType(ValType::kBool); }
		auto isIntU() const { return isType(ValType::kIntU); }
		auto isIntS() const { return isType(ValType::kIntS); }
		auto isInt() const { return isType(ValType::kIntU) || isType(ValType::kIntS); }
		auto isFloat() const { return isType(ValType::kFloat); }
		auto isStr() const { return isType(ValType::kStr); }
		auto isArr() const { return isType(ValType::kArr); }
		auto isObj() const { return isType(ValType::kObj); }

		Val & set(ValType const & type);

		auto & setUndef() { return set(ValType::kUndef); }
		auto & setNull() { return set(ValType::kNull); }
		auto & setBool() { return set(ValType::kBool); }
		auto & setIntU() { return set(ValType::kIntU); }
		auto & setIntS() { return set(ValType::kIntS); }
		auto & setFloat() { return set(ValType::kFloat); }
		auto & setStr() { return set(ValType::kStr); }
		auto & setArr() { return set(ValType::kArr); }
		auto & setObj() { return set(ValType::kObj); }

		template <typename T>
		auto & bind(T const & t) {
			clear();
			if constexpr (kTIsJSONVal<T>) {
				$ty = t.$ty;
				$v = t.$v;
			} else if constexpr (tIsJSONUndef<T>()) {
				$ty = ValType::kUndef;
				$v = kNullPtr;
			} else if constexpr (kTIsBool<T>) {
				$ty = ValType::kBool;
				$v = &const_cast<T &>(t);
			} else if constexpr (kTIsIntU<T>) {
				$ty = ValType::kIntU;
				$v = &const_cast<T &>(t);
			} else if constexpr (kTIsIntS<T>) {
				$ty = ValType::kIntS;
				$v = &const_cast<T &>(t);
			} else if constexpr (kTIsFloat<T>) {
				$ty = ValType::kFloat;
				$v = &const_cast<T &>(t);
			} else if constexpr (kTIsStr<T>) {
				$ty = ValType::kStr;
				$v = &const_cast<T &>(t);
			} else if constexpr (kTIsJSONArr<T>) {
				$ty = ValType::kArr;
				$v = &const_cast<T &>(t);
			} else if constexpr (kTIsJSONObj<T>) {
				$ty = ValType::kObj;
				$v = &const_cast<T &>(t);
			} else {
				static_assert_failed("UnKnown ValType");
			}
			return thiz;
		}
		auto & unbind() {
			$ty = ValType::kUndef;
			$v = kNullPtr;
			return thiz;
		}

		auto & moveFrom(TT & t) {
			bind(t);
			t.unbind();
			return thiz;
		}

		template <typename T>
		auto & set(T const & t, Bool const & forceNull = kBoF) {
			if (forceNull)
				return setNull();
			if constexpr (kTIsJSONVal<T>)
				return cpy(t);
			clear();
			if constexpr (tIsJSONUndef<T>()) {
				$ty = ValType::kUndef;
				$v = kNullPtr;
			} else if constexpr (kTIsBool<T>) {
				$ty = ValType::kBool;
				$v = new Bool(t);
			} else if constexpr (kTIsIntU<T>) {
				$ty = ValType::kIntU;
				$v = new IU64(static_cast<IU64>(t));
			} else if constexpr (kTIsIntS<T>) {
				$ty = ValType::kIntS;
				$v = new IS64(static_cast<IS64>(t));
			} else if constexpr (kTIsFloat<T>) {
				$ty = ValType::kFloat;
				$v = new F64(static_cast<F64>(t));
			} else if constexpr (kTIsStr<T>) {
				$ty = ValType::kStr;
				$v = new Str(t);
			} else if constexpr (kTIsJSONArr<T>) {
			#if !defined PLATFORM_ANDROID
				$ty = ValType::kArr;
				$v = new Arr(t);
			#endif
			} else if constexpr (kTIsJSONObj<T>) {
			#if !defined PLATFORM_ANDROID
				$ty = ValType::kObj;
				$v = new Obj(t);
			#endif
			} else {
				static_assert_failed("UnKnown ValType");
			}
			return thiz;
		}

	#if defined PLATFORM_ANDROID

		Val & set(Obj const & t, Bool const & forceNull = kBoF);
		Val & set(Arr const & t, Bool const & forceNull = kBoF);

	#endif

		Val & clear();

		Val & cpy(Val const & t);

		template <typename T>
		auto & get() {
			if constexpr (kTIsBool<T>)
				return *static_cast<Bool *>($v);
			else if constexpr (kTIsIntU<T>)
				return *static_cast<IU64 *>($v);
			else if constexpr (kTIsIntS<T>)
				return *static_cast<IS64 *>($v);
			else if constexpr (kTIsFloat<T>)
				return *static_cast<F64 *>($v);
			else if constexpr (kTIsStr<T>)
				return *static_cast<Str *>($v);
			else if constexpr (kTIsJSONArr<T>)
				return *static_cast<Arr *>($v);
			else if constexpr (kTIsJSONObj<T>)
				return *static_cast<Obj *>($v);
			else
				static_assert_failed("UnKnown ValType At GetVal");
		}
		template <typename T>
		auto & get() const {
			if constexpr (kTIsBool<T>)
				return *static_cast<Bool const *>($v);
			else if constexpr (kTIsIntU<T>)
				return *static_cast<IU64 const *>($v);
			else if constexpr (kTIsIntS<T>)
				return *static_cast<IS64 const *>($v);
			else if constexpr (kTIsFloat<T>)
				return *static_cast<F64 const *>($v);
			else if constexpr (kTIsStr<T>)
				return *static_cast<Str const *>($v);
			else if constexpr (kTIsJSONArr<T>)
				return *static_cast<Arr const *>($v);
			else if constexpr (kTIsJSONObj<T>)
				return *static_cast<Obj const *>($v);
			else
				static_assert_failed("UnKnown ValType At GetVal");
		}

		template <typename T>
		auto & get(T & dst) const {
			return dst = get<T>();
		}
		template <typename T>
		auto & get(T & dst, T const & dflt) const {
			if constexpr (getType<T>() == ValType::kIntS) {
				dst = isInt() ? static_cast<T const>(get<T>()) : dflt;
			} else {
				if (isType<T>()) {
					get(dst);
				} else {
					dst = dflt;
				}
			}
			return dst;
		}

		auto & getBool() {
			return get<Bool>();
		}
		auto & getIntU() {
			return get<IU64>();
		}
		auto & getIntS() {
			return get<IS64>();
		}
		auto & getFloat() {
			return get<F64>();
		}
		auto & getStr() {
			return get<Str>();
		}
		auto & getArr() {
			return get<Arr>();
		}
		auto & getObj() {
			return get<Obj>();
		}

		auto & getBool() const {
			return get<Bool>();
		}
		auto & getIntU() const {
			return get<IU64>();
		}
		auto & getIntS() const {
			return get<IS64>();
		}
		auto & getFloat() const {
			return get<F64>();
		}
		auto & getStr() const {
			return get<Str>();
		}
		auto & getArr() const {
			return get<Arr>();
		}
		auto & getObj() const {
			return get<Obj>();
		}

		Bool cmp(Val const & t) const;

		Void read(ConstCStr const & src, Sz & pos);
		Void write(CStr const & dst, Sz & pos, Sz indent = kSz0) const;

	public:

		inline Val & operator[](Sz const & i);
		inline Val & operator[](Str const & k);

		inline Val const & operator[](Sz const & i) const;
		inline Val const & operator[](Str const & k) const;

		inline Val & operator()(Str const & k);
		inline Val & operator()(Str const & k) const;

		auto operator==(Val const & t) const {
			return cmp(t);
		}
		auto operator!=(Val const & t) const { return !operator==(t); }

		auto & operator=(Val const & t) {
			return cpy(t);
		}

	public:

		~Val() {
			clear();
		}

		implicit Val() :
			$ty(ValType::kUndef), $v(kNullPtr) {
		}
		implicit Val(Val && t) :
			Val() {
			$ty = t.$ty;
			$v = t.$v;
			t.unbind();
		}
		implicit Val(Val const & t) :
			Val() {
			cpy(t);
		}
		explicit Val(ValType const & type) :
			Val() {
			set(type);
		}

	};

	struct Member : MapE<Val, Val> {

		auto & set(Val const & k, Val const & v) {
			$k = k;
			$v = v;
			return thiz;
		}
		template <typename T>
		auto & set(Val const & k, T const & v) {
			$k = k;
			$v.set(v);
			return thiz;
		}
		auto & set(Val const & k, ValType const & vt = ValType::kUndef) {
			$k = k;
			$v.set(vt);
			return thiz;
		}
		auto & set(Str const & k, Val const & v) {
			$k.set(k);
			$v = v;
			return thiz;
		}
		auto & set(Str const & k, ValType const & vt = ValType::kUndef) {
			$k.set(k);
			$v.set(vt);
			return thiz;
		}
		template <typename T>
		auto & set(Str const & k, T const & v) {
			$k.set(k);
			$v.set(v);
			return thiz;
		}

		auto & clear() {
			$k.clear();
			$v.clear();
			return thiz;
		}

		auto & operator[](Sz const & i) {
			return $v[i];
		}
		auto & operator[](Str const & k) {
			return $v[k];
		}

		auto const & operator[](Sz const & i) const {
			return $v[i];
		}
		auto const & operator[](Str const & k) const {
			return $v[k];
		}

	};

	struct Arr : List<Val> {

	private:

		using TS = List;

	public:

		auto & alloc(Sz const & cap = kSzN) {
			TS::alloc(cap);
			return thiz;
		}

		template <typename T>
		auto & add(T const & v, Bool const & forceNull = kBoF) {
			TS::appendX().last().set(v, forceNull);
			return thiz;
		}
		auto & add(ValType const & ty) {
			TS::appendX().last().set(ty);
			return thiz;
		}
		auto & addUndef() {
			return add(ValType::kUndef);
		}
		auto & addNull() {
			return add(ValType::kNull);
		}
		auto & addArr() {
			return add(ValType::kArr);
		}
		auto & addObj() {
			return add(ValType::kObj);
		}

		Void read(ConstCStr const & src, Sz & pos);
		Void write(CStr const & dst, Sz & pos, Sz indent = kSz0) const;

	};

	struct Obj : MapProto<Member> {

	private:

		using TS = MapProto;

	public:

		auto & alloc(Sz const & cap = kSzN) {
			TS::alloc(cap);
			return thiz;
		}

		auto find(Str const & k) const {
			Val tmp;
			tmp.bind(k);
			auto r = TS::findKey(tmp);
			tmp.unbind();
			return r;
		}
		auto find(Str const & k, Sz & i) const {
			return (i = find(k)) != kNoX;
		}
		auto exist(Str const & k) const {
			return find(k) != kNoX;
		}

		template <typename T>
		auto & add(Str const & k, T const & v, Bool const & forceNull = kBoF) {
			TS::appendX().last().set(k).$v.set(v, forceNull);
			return thiz;
		}

		auto & add(Str const & k, ValType const & valType) {
			TS::appendX().last().set(k, valType);
			return thiz;
		}
		auto & addUndef(Str const & k) {
			return add(k, ValType::kUndef);
		}
		auto & addNull(Str const & k) {
			return add(k, ValType::kNull);
		}
		auto & addObj(Str const & k) {
			return add(k, ValType::kObj);
		}
		auto & addArr(Str const & k) {
			return add(k, ValType::kArr);
		}

		Void read(ConstCStr const & src, Sz & pos);
		Void write(CStr const & dst, Sz & pos, Sz indent = kSz0) const;

	public:

		auto & operator[](Str const & k) {
			Sz i;
			if (!find(k, i))
				throw VAMGException("JSON", "can not found member by key : %s", k.$data);
			return get(i).$v;
		}
		auto const & operator[](Str const & k) const {
			Sz i;
			if (!find(k, i))
				throw VAMGException("JSON", "can not found member by key : %s", k.$data);
			return get(i).$v;
		}

		auto & operator()(Str const & k) {
			if (Sz i; find(k, i)) {
				return TS::get(i).$v;
			} else {
				addUndef(k);
				return last().$v;
			}
		}

	};

	inline Val & Val::operator[](Sz const & i) {
		return getArr()[i];
	}
	inline Val & Val::operator[](Str const & k) {
		return getObj()[k];
	}

	inline Val const & Val::operator[](Sz const & i) const {
		return getArr()[i];
	}
	inline Val const & Val::operator[](Str const & k) const {
		return getObj()[k];
	}

	inline Val & Val::operator()(Str const & k) {
		return getObj()(k);
	}

	struct ValPath {

		struct Item {

			enum class Type : IU8 { kNull, kNum, kStr };

			Type $ty;
			Sz $vNum;
			Str $vStr;

			auto isNull() const {
				return $ty == Type::kNull;
			}
			auto isNum() const {
				return $ty == Type::kNum;
			}
			auto isStr() const {
				return $ty == Type::kStr;
			}

			auto & getNum() const {
				return $vNum;
			}
			auto & getStr() const {
				return $vStr;
			}

			auto & fromJSONVal($JSON::Val const & t) {
				if (t.isIntU()) {
					$ty = Type::kNum;
					$vNum = t.getIntU();
				} else if (t.isStr()) {
					$ty = Type::kStr;
					$vStr = t.getStr();
				} else {
					$ty = Type::kNull;
				}
				return thiz;
			}
			auto toJSONVal() const {
				$JSON::Val r;
				if (isNum())
					r.set(getNum());
				else if (isStr())
					r.set(getStr());
				else
					r.setNull();
				return r;
			}

			auto operator==(Item const & t) const {
				if ($ty != t.$ty)
					return kBoF;
				if (isNull())
					return kBoT;
				if (isNum())
					return getNum() == t.getNum();
				if (isStr())
					return getStr() == t.getStr();
				return kBoF;
			}
			auto operator!=(Item const & t) const { return !operator==(t); }

			implicit Item() :
				$ty(Type::kNull), $vNum(), $vStr() {
			}
			implicit Item(Item const & t) = default;
			explicit Item(Sz const & num) :
				$ty(Type::kNum), $vNum(num), $vStr() {
			}
			explicit Item(Str const & str) :
				$ty(Type::kStr), $vNum(), $vStr(str) {
			}

		};

		List<Item> $path;

		auto & read($JSON::Arr const & t) {
			$path.alloc(t.$sz);
			for (auto const & e : t) {
				$path.appendX().last().fromJSONVal(e);
			}
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(t.$sz);
			for (auto const & e : $path) {
				t.append(e.toJSONVal());
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto & assign(Sz const & t) {
			$path.append(Item(t));
			return thiz;
		}
		auto & assign(Str const & t) {
			$path.append(Item(t));
			return thiz;
		}
		auto & eraseLast() {
			$path.eraseFromEnd(kSz1);
			return thiz;
		}

		auto operator==(ValPath const & t) const {
			return $path == t.$path;
		}
		auto operator!=(ValPath const & t) const { return !operator==(t); }

	};

	inline auto getValByPath(ValPath const & path, $JSON::Val & t, Sz lv = kNoX) {
		if (lv == kNoX)
			lv = path.$path.$sz;
		auto cur = &t;
		for (auto i = kNo1; i < lv; ++i) {
			if (path.$path[i].isNum() && (*cur).isArr()) {
				cur = &(*cur)[path.$path[i].getNum()];
			} else if (path.$path[i].isStr() && (*cur).isObj()) {
				cur = &(*cur)[path.$path[i].getStr()];
			} else {
				cur = kNullPtr;
				break;
			}
		}
		return cur;
	}

	namespace $Style {

		Void set(Bool const & noLineFeedWhenElement);

	}

	namespace $Tool {

		Val & eraseUndef(Val const & src, Val & dst);

		template <typename T>
		Sz cntStrSum(T const & src) {
			auto cnt = kSzN;
			if constexpr (kTIsJSONVal<T>) {
				switch (src.$$ty()) {
					default:
						break;
					case ValType::kStr:
						++cnt;
						break;
					case ValType::kArr:
						cnt += cntStrSum(src.getArr());
						break;
					case ValType::kObj:
						cnt += cntStrSum(src.getObj());
						break;
				}
			} else if constexpr (kTIsJSONArr<T>) {
				for (auto const & e : src) {
					cnt += cntStrSum(e);
				}
			} else if constexpr (kTIsJSONObj<T>) {
				for (auto const & e : src) {
					++cnt;
					cnt += cntStrSum(e.$v);
				}
			}
			return cnt;
		}

		template <typename T>
		Void getAllStr(T const & src, List<Str> & dst) {
			if constexpr (kTIsJSONVal<T>) {
				switch (src.$ty) {
					default:
						break;
					case ValType::kStr:
						if (!dst.range().exist(src.getStr()))
							dst.append(src.getStr());
						break;
					case ValType::kArr:
						getAllStr(src.getArr(), dst);
						break;
					case ValType::kObj:
						getAllStr(src.getObj(), dst);
						break;
				}
			} else if constexpr (kTIsJSONArr<T>) {
				for (auto const & e : src) {
					getAllStr(e.$v, dst);
				}
			} else if constexpr (kTIsJSONObj<T>) {
				for (auto const & e : src) {
					if (!dst.range().exist(e.$k.getStr()))
						dst.append(e.$k.getStr());
					getAllStr(e.$v, dst);
				}
			}
			return;
		}

		template <typename T>
		inline auto & getListFromArr(Arr const & src, List<T> & dst) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				e.get(dst.appendX().last());
			return dst;
		}
		template <typename T>
		inline auto & setListToArr(List<T> const & src, Arr & dst) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				dst.add(e);
			return dst;
		}

		template <typename T>
		inline auto & getListFromArr(Val const & src, List<T> & dst) {
			if (src.isArr())
				getListFromArr(src.getArr(), dst);
			else
				dst.setNull();
			return dst;
		}
		template <typename T>
		inline auto & setListToArr(List<T> const & src, Val & dst) {
			if (src.isNull())
				dst.setNull();
			else
				setListToArr(src, dst.setArr().getArr());
			return dst;
		}

		template <typename T>
		inline auto & getMapFromArr(Obj const & src, Map<Str, T> & dst) {
			dst.alloc(src.$sz);
			for (auto const & e : src) {
				auto & t = dst.appendX().last();
				e.$k.get(t.$k);
				e.$v.get(t.$v);
			}
			return dst;
		}
		template <typename T>
		inline auto & setMapToArr(Map<Str, T> const & src, Obj & dst) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				dst.add(e.$k, e.$v);
			return dst;
		}

		template <typename T>
		inline auto & getMapFromArr(Val const & src, Map<Str, T> & dst) {
			return getMapFromArr(src.getObj(), dst);
		}
		template <typename T>
		inline auto & setMapToArr(Map<Str, T> const & src, Val & dst) {
			return setMapToArr(src, dst.setObj().getObj());
		}

		namespace $IOMem {

			inline constexpr Sz kDfltBufSz(kSzBM * 48);

			Void free();
			Void realloc(Sz const & sz);

		}

		Val & read(Str const & src, Val & dst);
		Obj & read(Str const & src, Obj & dst);
		Arr & read(Str const & src, Arr & dst);

		Str & write(Str & dst, Val const & src);
		Str & write(Str & dst, Obj const & src);
		Str & write(Str & dst, Arr const & src);

		Val & read(Path const & src, Val & dst);
		Obj & read(Path const & src, Obj & dst);
		Arr & read(Path const & src, Arr & dst);

		Void write(Path const & dst, Val const & src);
		Void write(Path const & dst, Obj const & src);
		Void write(Path const & dst, Arr const & src);

		template <typename T>
		auto write(T const & src) { // show on cmd
			log.txtln("%s", write(Str().alloc(), src).$data);
			return;
		}

		inline auto fmtFile(Path const & src, Path const & dst) {
			return write(dst, read(src, crv(Val())));
		}

	}

	using namespace $Tool;

	inline auto & cnvJSONArrToPath($JSON::Val const & src, Path & dst) {
		return getListFromArr(src, dst);
	}
	inline auto & cnvPathToJSONArr(Path const & src, $JSON::Val & dst) {
		return setListToArr(src, dst);
	}

}
