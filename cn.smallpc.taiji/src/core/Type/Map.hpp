#pragma once

#include "../Type.hpp"
#include "../TypeUtil.hpp"
#include "./List.hpp"

namespace $TJ {

	template <typename TKey, typename TVal>
	struct MapE : Pair<TKey, TVal> {

	private:

		using TT = MapE;
		using TS = Pair<TKey, TVal>;

	public:

		TKey & $k;
		TVal & $v;

	public:

		auto & set(TKey const & k, TVal const & v) {
			TS::set(k, v);
			return thiz;
		}
		auto & setKey(TKey const & k) {
			TS::setA(k);
			return thiz;
		}
		auto & setVal(TVal const & v) {
			TS::setB(v);
			return thiz;
		}

	public:

		auto operator==(TT const & t) const {
			return TS::operator==(t);
		}
		auto operator!=(TT const & t) const {
			return TS::operator!=(t);
		}

		auto & operator=(TT const & t) {
			TS::operator=(t);
			return thiz;
		}

	public:

		implicit MapE() :
			TS(), $k(TS::$a), $v(TS::$b) {
		}
		implicit MapE(TT const & t) :
			TS(t), $k(TS::$a), $v(TS::$b) {
		}
		explicit MapE(TKey const & k, TVal const & v) :
			TS(k, v), $k(TS::$a), $v(TS::$b) {
		}

	};

	template <typename TMapE>
	struct MapProto : List<TMapE> {

	private:

		using TS = List<TMapE>;

	private:

		using TKey = TRR<decltype(TMapE::$k)>;
		using TVal = TRR<decltype(TMapE::$v)>;

	public:

		auto & appendByKV(TKey const & k, TVal const & v) {
			TS::appendX().last().set(k, v);
			return thiz;
		}

		auto findKey(TKey const & t) const {
			return TS::range().findBy([&t](auto & v) { return v.$k == t; });
		}
		auto findKey(TKey const & t, Sz & i) const {
			return (i = findKey(t)) != kNoX;
		}
		auto existKey(TKey const & t) const {
			return findKey(t) != kNoX;
		}

		auto findVal(TVal const & t) const {
			return TS::range().findBy([&t](auto & v) { return v.$v == t; });
		}
		auto findVal(TVal const & t, Sz & i) const {
			return (i = findVal(t)) != kNoX;
		}
		auto existVal(TVal const & t) const {
			return findVal(t) != kNoX;
		}

		auto & getByKey(TKey const & t) {
			Sz i;
			if (!findKey(t, i))
				throw Exception("Map", "can not found member by key");
			return TS::get(i);
		}
		auto & getByKey(TKey const & t) const {
			Sz i;
			if (!findKey(t, i))
				throw Exception("Map", "can not found member by key");
			return TS::get(i);
		}

		auto & eraseByKey(TKey const & t) {
			TS::eraseOne(findKey(t));
			return thiz;
		}

		auto & sortUpByKey() {
			TS::range().sort([](auto & a, auto & b) { return a.$k > b.$k; });
			return thiz;
		}

		auto & operator[](TKey const & k) {
			return TS::get(findKey(k)).$v;
		}
		auto & operator[](TKey const & k) const {
			return TS::get(findKey(k)).$v;
		}

		auto & operator()(TKey const & k) {
			if (Sz i; findKey(k, i)) {
				return TS::get(i).$v;
			} else {
				TS::appendX().last().setKey(k);
				return TS::last().$v;
			}
		}

	};

	template <typename TKey, typename TVal>
	using Map = MapProto<MapE<TKey, TVal>>;

}
