#pragma once

#include "./Type.hpp"
#include "./TypeUtil.hpp"
#include "./Const.hpp"

namespace $TJ::$Iterator {

	template <typename T>
	inline auto itDiff(T const & beg, T const & end) {
		return static_cast<Sz>(end - beg);
	}

	template <typename T>
	struct Range : TypePair<T const> {

		// range iterator : [ x, y )

	private:

		using TT = Range;
		using TS = TypePair<T const>;
		using TE = TRC<TRR<decltype(*T())>>;

	public:

		auto begin() const {
			return TS::$a;
		}
		auto end() const {
			return TS::$b;
		}

		auto range() const {
			return thiz;
		}

		auto sz() const {
			return itDiff(TS::$a, TS::$b);
		}

		auto sub(Sz const & beg = kNo1, Sz sz = kNoX) {
			if (sz == kNoX)
				sz = TT::sz() - beg;
			return Range(TS::$a + beg, TS::$a + beg + sz);
		}

		template <typename TF>
		auto & each(TF const & f) const {
			for (auto cur = TS::$a; cur != TS::$b; ++cur)
				f(*cur);
			return thiz;
		}
		template <typename TF>
		auto & eachIter(TF const & f) const {
			for (auto cur = TS::$a; cur != TS::$b; ++cur)
				f(cur);
			return thiz;
		}
		template <typename TF>
		auto eachUntil(TF const & f) const {
			auto i = kNo1;
			for (auto cur = TS::$a; cur != TS::$b; ++cur, ++i)
				if (f(*cur))
					return i;
			return kNoX;
		}
		template <typename TF>
		auto eachSum(TF const & f, decltype(std::declval<TF>()(crv(std::declval<TE>()))) const & init) const {
			auto r = init;
			for (auto cur = TS::$a; cur != TS::$b; ++cur)
				r += f(*cur);
			return r;
		}

		template <typename TF>
		auto & eachRev(TF const & f) const {
			for (auto cur = TS::$b; cur != TS::$a; --cur) {
				f(*(cur - kNo2));
			}
			return thiz;
		}
		template <typename TF>
		auto & eachIterRev(TF const & f) const {
			for (auto cur = TS::$b; cur != TS::$a; --cur)
				f(cur - kNo2);
			return thiz;
		}
		template <typename TF>
		auto eachUntilRev(TF const & f) const {
			auto i = sz() - kNo2;
			for (auto cur = TS::$b; cur != TS::$a; --cur, --i)
				if (f(*(cur - kNo2)))
					return i;
			return kNoX;
		}

		auto cnt(TE const & t) const {
			return eachSum(
				[&t](auto const & v) {
					return v == t ? kSz1 : kSzN;
				}, kSzN
			);
		}
		template <typename TOtherRange>
		auto cntN(TOtherRange const & t) const {
			auto tRange = t.range();
			return eachSum(
				[&tRange](auto const & v) {
					return tRange.exist(v) ? kSz1 : kSzN;
				}, kSzN
			);
		}
		auto & replace(TE const & before, TE const & after) const {
			each(
				[&before, &after](auto & v) {
					if (v == before)
						v = after;
					return;
				}
			);
			return thiz;
		}

		auto & set(TE const & t = TE()) const {
			each(
				[&t](auto & v) {
					v = t;
					return;
				}
			);
			return thiz;
		}

		template <typename TOtherIter = T>
		Range const & cpy(TOtherIter tbeg) const {
			each(
				[&tbeg](auto & v) {
					v = *tbeg;
					++tbeg;
					return;
				}
			);
			return thiz;
		}

		auto & swp(Sz const & i, Sz const & j) {
			vSwp(*(TS::$a + i), *(TS::$a + j));
			return thiz;
		}

		template <typename TOtherIter>
		auto cmp(TOtherIter tbeg) const {
			return eachUntil(
				[&tbeg](auto const & v) {
					auto & tv = *tbeg;
					++tbeg;
					return v != tv;
				}
			) == kNoX;
		}

		auto find(TE const & t) const {
			return eachUntil(
				[&t](auto & v) {
					return v == t;
				}
			);
		}
		auto find(TE const & t, Sz & i) const {
			return (i = find(t)) != kNoX;
		}
		auto exist(TE const & t) const {
			return find(t) != kNoX;
		}

		template <typename TF>
		auto findBy(TF const & f) const {
			return eachUntil(
				[&f](auto & v) {
					return f(v);
				}
			);
		}
		template <typename TF>
		auto findBy(TF const & f, Sz & i) const {
			return (i = findBy(f)) != kNoX;
		}
		template <typename TF>
		auto existBy(TF const & f) const {
			return findBy(f) != kNoX;
		}

		auto findSub(TT const & t) const {
			auto i = kNo1;
			for (auto cur = TS::$a; cur != TS::$b && t.sz() <= itDiff(cur, TS::$b); ++cur) {
				if (Range(cur, cur + t.sz()).cmp(t.begin()))
					return i;
				++i;
			}
			return kNoX;
		}
		auto findSub(TT const & t, Sz & i) const {
			return (i = findSub(t)) != kNoX;
		}
		auto existSub(TT const & t) const {
			return findSub(t) != kNoX;
		}

		auto findRev(TE const & t) const {
			return eachUntilRev(
				[&t](auto & v) {
					return v == t;
				}
			);
		}
		auto findRev(TE const & t, Sz & i) const {
			return (i = findRev(t)) != kNoX;
		}
		auto existRev(TE const & t) const {
			return findRev(t) != kNoX;
		}

		template <typename TOtherRange>
		auto findDiff(TOtherRange const & t) const {
			auto n = vMin(sz(), t.sz());
			auto cur = begin();
			auto tcur = t.begin();
			for (auto i = kNo1; i < n; ++i) {
				if (*cur != *tcur)
					return i;
				++cur; ++tcur;
			}
			return sz() == t.sz() ? kNoX : n;
		}
		template <typename TOtherRange>
		auto findDiff(TOtherRange const & t, Sz & i) const {
			return (i = findDiff(t)) != kNoX;
		}

		template <typename TF>
		auto & sort(TF const & f) const {
			for (auto i = TS::$a; i != TS::$b; ++i)
				for (auto j = i + kNo2; j != TS::$b; ++j)
					if (f(*i, *j))
						vSwp(*i, *j);
			return thiz;
		}
		auto & sortUp() const {
			for (auto i = TS::$a; i != TS::$b; ++i)
				for (auto j = i + kNo2; j != TS::$b; ++j)
					if (*i > * j)
						vSwp(*i, *j);
			return thiz;
		}
		auto & sortDown() const {
			for (auto i = TS::$a; i != TS::$b; ++i)
				for (auto j = i + kNo2; j != TS::$b; ++j)
					if (*i < *j)
						vSwp(*i, *j);
			return thiz;
		}

	public:

		implicit Range() :
			TS() {
		}
		implicit Range(TT const & t) :
			TS(t) {
		}
		explicit Range(T const & x, T const & y) :
			TS(x, y) {
		}
		explicit Range(T const & x, Sz const & sz) :
			TS(x, x + sz) {
		}

	};

	template <typename T, Bool $const = kBoF>
	struct SingleIter {

	private:

		using TT = SingleIter;

	public:

		T * $v;
		Sz $i;

	public:

		auto & operator*() {
			if constexpr ($const)
				return static_cast<T const &>(*$v);
			else
				return static_cast<T &>(*$v);
		}
		auto & operator*() const {
			return static_cast<T const &>(*$v);
		}

		auto operator==(TT const & t) const {
			return $v == t.$v && $i == t.$i;
		}
		auto operator!=(TT const & t) const { return !operator==(t); }

		auto & operator+=(Sz const & n) {
			$i += n;
			return thiz;
		}
		auto & operator-=(Sz const & n) {
			$i -= n;
			return thiz;
		}

		auto & operator++() {
			++$i;
			return thiz;
		}
		auto & operator--() {
			--$i;
			return thiz;
		}

		auto operator+(Sz const & n) const {
			return TT(thiz) += n;
		}
		auto operator-(Sz const & n) const {
			return TT(thiz) -= n;
		}

		auto operator-(TT const & t) const {
			return static_cast<Sz>($i - t.$i);
		}

	public:

		implicit SingleIter() :
			$v(kNullPtr), $i(kNoX) {
		}
		implicit SingleIter(TT const & t) = default;
		explicit SingleIter(T const & v, Sz const & i) :
			$v(&const_cast<T &>(v)), $i(i) {
		}

	};

	template <typename T>
	using ConstSingleIter = SingleIter<T, kBoT>;

	template <typename T, Bool $const = kBoF>
	struct SeqIter { // Sequential

	private:

		using TT = SeqIter;

	public:

		T * $v;
		Sz $i;

	public:

		auto & operator*() {
			if constexpr ($const)
				return static_cast<TRC<TRR<decltype(T()[kNoX])>> const &>((*$v)[$i]);
			else
				return static_cast<TRC<TRR<decltype(T()[kNoX])>> &>((*$v)[$i]);
		}
		auto & operator*() const {
			return static_cast<TRC<TRR<decltype(T()[kNoX])>> const &>((*$v)[$i]);
		}

		auto operator==(TT const & t) const {
			return $v == t.$v && $i == t.$i;
		}
		auto operator!=(TT const & t) const { return !operator==(t); }

		auto & operator+=(Sz const & n) {
			$i += n;
			return thiz;
		}
		auto & operator-=(Sz const & n) {
			$i -= n;
			return thiz;
		}

		auto & operator++() {
			++$i;
			return thiz;
		}
		auto & operator--() {
			--$i;
			return thiz;
		}

		auto operator+(Sz const & n) const {
			return TT(thiz) += n;
		}
		auto operator-(Sz const & n) const {
			return TT(thiz) -= n;
		}

		auto operator-(TT const & t) const {
			return static_cast<Sz>($i - t.$i);
		}

	public:

		implicit SeqIter() :
			$v(kNullPtr), $i(kNoX) {
		}
		implicit SeqIter(TT const & t) = default;
		explicit SeqIter(T const & v, Sz const & i) :
			$v(&const_cast<T &>(v)), $i(i) {
		}

	};

	template <typename T>
	using ConstSeqIter = SeqIter<T, kBoT>;

	template <typename T, Bool $const = kBoF>
	struct SeqRevIter { // Sequential Reverse

	private:

		using TT = SeqRevIter;

	public:

		T * $v;
		Sz $i;

	public:

		auto & operator*() {
			if constexpr ($const)
				return static_cast<TRC<TRR<decltype(T()[kNoX])>> const &>((*$v)[$i]);
			else
				return static_cast<TRC<TRR<decltype(T()[kNoX])>> &>((*$v)[$i]);
		}
		auto & operator*() const {
			return static_cast<TRC<TRR<decltype(T()[kNoX])>> const &>((*$v)[$i]);
		}

		auto operator==(TT const & t) const {
			return $v == t.$v && $i == t.$i;
		}
		auto operator!=(TT const & t) const { return !operator==(t); }

		auto & operator+=(Sz const & n) {
			$i -= n;
			return thiz;
		}
		auto & operator-=(Sz const & n) {
			$i += n;
			return thiz;
		}

		auto & operator++() {
			--$i;
			return thiz;
		}
		auto & operator--() {
			++$i;
			return thiz;
		}

		auto operator+(Sz const & n) const {
			return TT(thiz) += n;
		}
		auto operator-(Sz const & n) const {
			return TT(thiz) -= n;
		}

		auto operator-(TT const & t) const { // todo?
			return static_cast<Sz>($i - t.$i);
		}

	public:

		implicit SeqRevIter() :
			$v(kNullPtr), $i(kNoX) {
		}
		implicit SeqRevIter(TT const & t) = default;
		explicit SeqRevIter(T const & v, Sz const & i) :
			$v(&const_cast<T &>(v)), $i(i) {
		}

	};

	template <typename T>
	using ConstSeqRevIter = SeqRevIter<T, kBoT>;

}

namespace $TJ {
	using namespace $Iterator;
}
