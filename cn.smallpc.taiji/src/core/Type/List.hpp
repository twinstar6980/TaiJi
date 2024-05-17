#pragma once

#include "../Type.hpp"
#include "../Iter.hpp"
#include "../Const.hpp"
#include "../Ptr.hpp"
#include "../TypeUtil.hpp"

namespace $TJ {

	template <typename T, Sz $sz>
	struct ConstSzArray {

		T $data[$sz];

		auto & operator=(ConstSzArray const & t) {
			Range<T *>(&*$data, $sz).cpy(t.$data);
			return thiz;
		}

		implicit ConstSzArray() = default;
		implicit ConstSzArray(ConstSzArray const & t) {
			thiz = t;
		}

	};

	template <typename T>
	struct List {

	private:

		using TT = List;
		using TE = T;

		enum class State : IU8 { kNull, kSelf, kRef };

	public:

		State $state;
		T * $data;
		Sz $sz;
		Sz $cap;

	public:

		auto isNull() const {
			return $state == State::kNull;
		}
		auto isSelf() const {
			return $state == State::kSelf;
		}
		auto isRef() const {
			return $state == State::kRef;
		}

		auto isEmpty() const {
			assert(!isNull());
			return $sz == kSzN;
		}
		auto isFull() const {
			assert(!isNull());
			return $sz == $cap;
		}

		auto lastIdx() const {
			assert(!isNull());
			return static_cast<Sz>($sz - kNo2);
		}
		auto getDiffOfFull() const {
			assert(!isNull());
			return static_cast<Sz>($cap - $sz);
		}

		auto iter(Sz const & i) {
			assert(!isNull());
			assert(i <= $sz);
			return $data + i;
		}
		auto iter(Sz const & i) const {
			assert(!isNull());
			assert(i <= $sz);
			return $data + i;
		}
		auto citer(Sz const & i) const {
			assert(!isNull());
			assert(i <= $sz);
			return $data + i;
		}

		auto riter(Sz const & i) {
			assert(!isNull());
			assert(i < $sz || i == kNoX);
			return SeqRevIter($data, i);
		}
		auto riter(Sz const & i) const {
			assert(!isNull());
			assert(i < $sz || i == kNoX);
			return ConstSeqRevIter<T *>($data, i);
		}
		auto criter(Sz const & i) const {
			assert(!isNull());
			assert(i <= $sz);
			return ConstSeqRevIter<T *>($data, i);
		}

		auto begin() {
			return iter(kNo1);
		}
		auto begin() const {
			return iter(kNo1);
		}
		auto cbegin() const {
			return citer(kNo1);
		}

		auto end() {
			return iter($sz);
		}
		auto end() const {
			return iter($sz);
		}
		auto cend() const {
			return citer($sz);
		}

		auto rbegin() {
			return riter(lastIdx());
		}
		auto rbegin() const {
			return riter(lastIdx());
		}
		auto crbegin() const {
			return criter(lastIdx());
		}

		auto rend() {
			return riter(kNoX);
		}
		auto rend() const {
			return riter(kNoX);
		}
		auto crend() const {
			return criter(kNoX);
		}

		auto range(Sz const & beg = kNo1, Sz sz = kSzX) {
			assert(!isNull());
			assert(beg <= $sz);
			if (sz == kSzX)
				sz = $sz - beg;
			assert(beg + sz <= $sz);
			return Range(iter(beg), iter(beg + sz));
		}
		auto range(Sz const & beg = kNo1, Sz sz = kSzX) const {
			assert(!isNull());
			assert(beg <= $sz);
			if (sz == kSzX)
				sz = $sz - beg;
			assert(beg + sz <= $sz);
			return Range(iter(beg), iter(beg + sz));
		}
		auto crange(Sz const & beg = kNo1, Sz sz = kSzX) const {
			assert(!isNull());
			assert(beg <= $sz);
			if (sz == kSzX)
				sz = $sz - beg;
			assert(beg + sz <= $sz);
			return Range(citer(beg), citer(beg + sz));
		}

		auto rrange(Sz const & beg = kNo1, Sz sz = kSzX) const {
			assert(!isNull());
			assert(beg <= $sz);
			if (sz == kSzX)
				sz = $sz - beg;
			assert(beg + sz <= $sz);
			return Range(riter(beg + sz - kNo2), riter(beg - kNo2));
		}

		auto & get(Sz const & i) {
			assert(!isNull());
			assert(i < $sz);
			return $data[i];
		}
		auto const & get(Sz const & i) const {
			assert(!isNull());
			assert(i < $sz);
			return $data[i];
		}

		auto & first() {
			return get(kNo1);
		}
		auto & first() const {
			return get(kNo1);
		}
		auto & last() {
			return get(lastIdx());
		}
		auto & last() const {
			return get(lastIdx());
		}

		auto & free() {
			if (isSelf())
				pFreeN($data);
			$state = State::kNull;
			$data = kNullPtr;
			$sz = kSzX;
			$cap = kSzX;
			return thiz;
		}
		auto & setNull() {
			return free();
		}

		auto & alloc(Sz const & cap = kSzN) {
			free();
			$state = State::kSelf;
			pAlloc($data, cap + kSz1);
			$sz = kSzN;
			$cap = cap;
			return thiz;
		}
		auto & allocThenUse(Sz const & cap = kSzN, Sz const & sz = kSzX) {
			alloc(cap);
			$sz = (sz == kSzX || sz > cap) ? cap : sz;
			return thiz;
		}

		auto & unbind() {
			assert(isSelf());
			$state = State::kNull;
			$data = kNullPtr;
			$sz = kSzX;
			$cap = kSzX;
			return thiz;
		}
		auto & bind(TT const & t) {
			assert(t.isSelf());
			free();
			$state = State::kSelf;
			$data = const_cast<T *>(t.$data);
			$sz = t.$sz;
			$cap = t.$cap;
			return thiz;
		}
		auto & bind(T const * const & data, Sz const & sz, Sz const & cap = kSzX) {
			assert(sz <= cap);
			free();
			$state = State::kSelf;
			$data = const_cast<T *>(data);
			$sz = sz;
			$cap = cap == kSzX ? sz : cap;
			return thiz;
		}

		auto & moveFrom(TT & t) {
			bind(t);
			t.unbind();
			return thiz;
		}

		auto & unref() {
			assert(isRef());
			$state = State::kNull;
			$data = kNullPtr;
			$sz = kSzX;
			$cap = kSzX;
			return thiz;
		}
		auto & ref(TT const & t, Sz const & beg = kNo1, Sz sz = kSzX, Sz cap = kSzX) {
			assert(!t.isNull());
			assert(beg <= t.$sz);
			if (sz == kSzX)
				sz = t.$sz - beg;
			assert(beg + sz <= t.$sz);
			if (cap == kSzX)
				cap = sz;
			free();
			$state = State::kRef;
			$data = const_cast<T *>(t.$data + beg);
			$sz = sz;
			$cap = cap;
			return thiz;
		}
		auto & ref(T const * const & data, Sz const & sz, Sz cap = kSzX) {
			if (cap == kSzX)
				cap = sz;
			free();
			$state = State::kRef;
			$data = const_cast<T *>(data);
			$sz = sz;
			$cap = cap;
			return thiz;
		}

		auto & realloc(Sz const & cap) {
			assert(isSelf());
			assert(cap >= $sz);
			//cout << "rc " << $cap << " - " << cap << endl;
			auto oriData = $data;
			auto oriSz = $sz;
			unbind().allocThenUse(cap, oriSz);
			range().cpy(oriData);
			pFreeN(oriData);
			return thiz;
		}
		auto & expand(Sz const & cap = kSz1) {
			return realloc($cap + cap);
		}
		auto & lite() {
			return realloc(Sz($sz));
		}

		auto willOverflow(Sz const & sz) const {
			assert(isSelf());
			return $sz + sz > $cap;
		}
		auto getCapAfterIns(Sz const & sz) const {
			assert(isSelf());
			//return willOverflow(sz) ? static_cast<Sz>($sz + sz) : $cap;
			return willOverflow(sz) ? static_cast<Sz>(($sz + sz) * 2) : $cap;
		}
		auto & expandMin(Sz const & sz) {
			assert(isSelf());
			return willOverflow(sz) ? realloc(getCapAfterIns(sz)) : thiz;
		}

		auto & setSz(Sz const & sz) {
			assert(!isNull());
			assert(sz <= $cap);
			$sz = sz;
			return thiz;
		}
		auto & expandSz(Sz const & sz) {
			return setSz($sz + sz);
		}
		auto & fullSz() {
			return setSz($cap);
		}

		auto cmp(TT const & t) const {
			return (isNull() && t.isNull()) || ($sz == t.$sz && range().cmp(t.begin()));
		}

		auto & cpy(TT const & t) {
			if (t.isNull())
				return setNull();
			//cout << "copy : " << t.$sz << endl;
			allocThenUse(t.$sz);
			range().cpy(t.begin());
			return thiz;
		}
		auto & cpy(T const * const & data, Sz const & sz) {
			allocThenUse(sz);
			range().cpy(data);
			return thiz;
		}

		auto & insProto(Sz const & beg, Sz const & sz, Bool clearData) {
			assert(isSelf());
			assert(beg <= $sz);
			expandMin(sz);
			auto oriSz = $sz;
			$sz += sz;
			Range<TRR<decltype(rbegin())>>(rbegin(), riter(beg + sz - kNo2)).cpy(riter(oriSz - kNo2));
			if (clearData)
				range(beg, sz).set();
			return thiz;
		}

		auto & insX(Sz const & beg, Sz const & sz = kSz1) {
			return insProto(beg, sz, kBoT);
		}
		auto & ins(T const & t, Sz const & beg, Sz const & sz = kSz1) {
			insProto(beg, sz, kBoF);
			range(beg, sz).set(t);
			return thiz;
		}
		auto & ins(TT const & t, Sz const & beg) {
			assert(!t.isNull());
			insProto(beg, t.$sz, kBoF);
			range(beg, t.$sz).cpy(t.begin());
			return thiz;
		}

		auto & prependX(Sz const & sz = kSz1) {
			return insX(kNo1, sz);
		}
		auto & prepend(T const & t, Sz const & sz = kSz1) {
			return ins(t, kNo1, sz);
		}
		auto & prepend(TT const & t) {
			return ins(t, kNo1);
		}

		auto & appendX(Sz const & sz = kSz1) {
			return insX(Sz($sz), sz);
		}
		auto & append(T const & t, Sz const & sz = kSz1) {
			return ins(t, Sz($sz), sz);
		}
		auto & append(TT const & t) {
			return ins(t, Sz($sz));
		}

		auto & erase(Sz const & beg, Sz const & sz) {
			assert(isSelf());
			assert(beg <= $sz);
			assert(beg + sz <= $sz);
			range(beg, $sz - sz - beg).cpy(iter(beg + sz));
			range($sz - sz, sz).set();
			$sz -= sz;
			return thiz;
		}

		auto & eraseOne(Sz const & beg) {
			return erase(beg, kSz1);
		}
		auto & eraseAfter(Sz const & i) {
			return erase(i, $sz - i);
		}
		auto & eraseFromEnd(Sz const & sz = kSz1) {
			assert(sz <= $sz);
			return erase($sz - sz, sz);
		}
		auto & eraseSub(TT const & t) {
			if (Sz i; range().findSub(t.range(), i))
				erase(i, t.$sz);
			return thiz;
		}

		auto & setEmpty() {
			assert(!isNull());
			eraseAfter(kNo1);
			return thiz;
		}

		template <typename TF>
		auto hash(TF const & f) const {
			return f(thiz);
		}
		template <typename TF>
		auto hashByPtr(TF const & f) const {
			return f($data, $sz);
		}

		auto sumSz() const {
			return range().eachSum(
				[](auto & v) {
					return v.$sz;
				}, kSzN
			);
		}

		enum class ConcatOption : IU8 { kNone, kPrefix, kSuffix, kInside, kAround };

		template <ConcatOption option>
		auto concatSz() const {
			if (isEmpty())
				return kSzN;
			auto sz = sumSz();
			if constexpr (option == ConcatOption::kNone) {
			} else if constexpr (option == ConcatOption::kPrefix ||
								 option == ConcatOption::kSuffix) {
				sz += $sz;
			} else if constexpr (option == ConcatOption::kInside) {
				sz += $sz; --sz;
			} else if constexpr (option == ConcatOption::kAround) {
				sz += $sz; ++sz;
			} else {
				static_assert_failed("concat option error");
			}
			return sz;
		}

		template <ConcatOption option, typename TDelimiter>
		auto & concat(T & dst, TDelimiter const & delimiter) const {
			auto & r = dst;
			r.alloc(concatSz<option>());
			if constexpr (option == ConcatOption::kNone) {
				range().each([&r](auto & v) { r.append(v); });
			} else if constexpr (option == ConcatOption::kPrefix) {
				range().each([&r, &delimiter](auto & v) { r.append(delimiter).append(v); });
			} else if constexpr (option == ConcatOption::kSuffix) {
				range().each([&r, &delimiter](auto & v) { r.append(v).append(delimiter); });
			} else if constexpr (option == ConcatOption::kInside) {
				range().each([&r, &delimiter](auto & v) { r.append(v).append(delimiter); });
				if (!r.isEmpty())
					r.eraseFromEnd();
			} else if constexpr (option == ConcatOption::kAround) {
				r.append(delimiter);
				range().each([&r, &delimiter](auto & v) { r.append(v).append(delimiter); });
			} else {
				static_assert_failed("option error");
			}
			return dst;
		}

	public:

		auto & operator[](Sz const & i) {
			return get(i);
		}
		auto & operator[](Sz const & i) const {
			return get(i);
		}

		auto operator==(TT const & t) const {
			return cmp(t);
		}
		auto operator!=(TT const & t) const { return !operator==(t); }

		auto & operator=(TT const & t) {
			return cpy(t);
		}
		auto & operator=(TT && t) {
			free();
			if (!t.isNull()) {
				bind(t);
				t.unbind();
			}
			return thiz;
		}

		auto & operator+=(T const & t) {
			return append(t);
		}
		auto & operator+=(TT const & t) {
			assert(!t.isNull());
			return append(t);
		}

		auto operator+(T const & t) const {
			assert(!isNull());
			return TT(TT().alloc($sz + kSz1).append(thiz).append(t));
		}
		auto operator+(TT const & t) const {
			assert(!isNull());
			assert(!t.isNull());
			return TT(TT().alloc($sz + t.$sz).append(thiz).append(t));
		}

	public:

		~List() {
			free();
		}

		implicit List() :
			$state(State::kNull), $data(kNullPtr), $sz(kSzX), $cap(kSzX) {
		}
		implicit List(TT && t) :
			List() {
			if (t.isNull()) {
				setNull();
			} else if (t.isRef()) {
				ref(t);
				t.unref();
			} else if (t.isSelf()) {
				bind(t);
				t.unbind();
			}
		}
		implicit List(TT const & t) :
			List() {
			cpy(t);
		}
		explicit List(T const * const & data, Sz const & sz) :
			List() {
			cpy(List().ref(data, sz));
		}
		explicit List(Sz const & cap, Sz const & sz = kSzN) :
			List() {
			allocThenUse(cap, sz);
		}

	};

}
