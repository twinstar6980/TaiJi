#pragma once

#include "./List.hpp"
#include "./Map.hpp"
#include "./JSON.hpp"

namespace $TJ {

	class InfoTypeBase {

		virtual InfoTypeBase & read($JSON::Val const &) = 0;
		virtual InfoTypeBase & write($JSON::Val &) = 0;

	};

#if defined ENABLE_CONCEPTS

	template <typename T>
	concept CTInfo = requires(T t, T const ct) {
		true;
		// todo
		//t.read(static_cast<$JSON::Val const &>($JSON::Val()));
		//ct.write(crv($JSON::Val()));
	};

#else

#define CTInfo typename

#endif

	template <CTInfo T>
	struct InfoList : List<T> {

	private:

		using TS = List<T>;

	public:

		auto & read($JSON::Arr const & t) {
			assert(!t.isNull());
			TS::alloc(t.$sz);
			for (auto const & e : t)
				TS::appendX().last().read(e);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			assert(!TS::isNull());
			t.alloc(TS::$sz);
			for (auto const & e : thiz)
				e.write(t.appendX().last());
			return t;
		}

		auto & read($JSON::Val const & t) {
			if (t.isNull())
				TS::setNull();
			else
				read(t.getArr());
			return thiz;
		}
		auto & write($JSON::Val & t) const {
			if (TS::isNull())
				t.setNull();
			else
				write(t.setArr().getArr());
			return t;
		}

		template <typename TBlock>
		auto & parse(List<TBlock> const & t) {
			assert(!t.isNull());
			TS::alloc(t.$sz);
			for (auto const & e : t)
				TS::appendX().last().parse(e);
			return thiz;
		}
		template <typename TBlock>
		auto & parseTo(List<TBlock> & t) const {
			assert(!TS::isNull());
			t.alloc(TS::$sz);
			for (auto const & e : thiz)
				e.parseTo(t.appendX().last());
			return thiz;
		}

	};


	template <CTInfo T>
	using InfoMapE = MapE<Str, T>;

	template <CTInfo T>
	struct InfoMap : MapProto<InfoMapE<T>> {

	private:

		using TS = MapProto<InfoMapE<T>>;

	public:

		auto & read($JSON::Obj const & t) {
			assert(!t.isNull());
			TS::alloc(t.$sz);
			for (auto const & e : t)
				TS::appendX().last().setKey(e.$k.getStr()).$v.read(e.$v);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			assert(!TS::isNull());
			t.alloc(TS::$sz);
			for (auto const & e : thiz)
				e.$v.write(t.appendX().last().set(e.$k).$v);
			return t;
		}

		auto & read($JSON::Val const & t) {
			if (t.isNull())
				TS::setNull();
			else
				read(t.getObj());
			return thiz;
		}
		auto & write($JSON::Val & t) const {
			if (TS::isNull())
				t.setNull();
			else
				write(t.setObj().getObj());
			return t;
		}

	};

}
