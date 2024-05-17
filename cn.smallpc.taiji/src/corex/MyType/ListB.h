# pragma once

# include "../Type.h"
# include "./Iter.h"

# include "../Macro.h"
# include "../Const.h"
# include "../UtilKit/PtrUtil.h"
# include "../UtilKit/MemoryUtil.h"

namespace $TJ {
	tmpl_st(T) struct ListB {
		T * _item;
		SzI _cap, _sz;

		auto getIter(SzI const & i) {
			precond(!isNull());
			precond(i <= _sz);
			return Iter<ListB, T>(*this, i);
		}
		auto getIter(SzI const & i) const {
			precond(!isNull());
			precond(i <= _sz);
			return Iter<ListB, T>(*this, i);
		}

		auto begin() { return getIter(kThFirst); }
		auto end() { return getIter(_sz); }

		auto begin() const { return getIter(kThFirst); }
		auto end() const { return getIter(_sz); }

		auto isNull() const { return _item == kNullPtr; }
		auto isNoCap() const { return _cap == kSzNull; }

		auto isEmpty() const {
			precond(!isNull());
			return _sz == kSzNone;
		}
		auto isFull() const {
			precond(!isNoCap());
			return _sz == _cap;
		}

		auto lastIdx() const {
			precond(!isNull());
			return static_cast<SzI>(_sz - kThSecond);
		}
		auto getDiffOfFull() const {
			precond(!isNoCap());
			return static_cast<SzI>(_cap - _sz);
		}

		auto & get(SzI const & i) {
			precond(!isNull());
			precond(i < _sz);
			return _item[i];
		}
		auto const & get(SzI const & i) const {
			precond(isNull());
			precond(i < _sz);
			return _item[i];
		}
		auto & first() { return get(kThFirst); }
		auto & first() const { return get(kThFirst); }
		auto & last() { return get(lastIdx()); }
		auto & last() const { return get(lastIdx()); }

		auto & swp(SzI const & i, SzI const & j) {
			precond(!isNull());
			precond(i < _sz && j < _sz);
			swpVal(get(i), get(j));
			return *this;
		}

		auto & clear(SzI const & beg, SzI const & sz) {
			precond(!isNull());
			precond(beg < _sz);
			precond(beg + sz <= _sz);
			memSet<T>(_item + beg, sz);
			return *this;
		}
		auto & clearOne(SzI const & i) { return clear(i, kSzOne); }
		auto & clearAfter(SzI const & beg = kThFirst) { return clear(beg, _sz - beg); }
		auto & clearFromEnd(SzI const & sz) { return clear(_sz - sz, sz); }
		auto & clearLast() { return clearOne(lastIdx()); }

		auto & reset(SzI const & beg, SzI const & sz) {
			precond(!isNull());
			precond(beg < _sz);
			precond(beg + sz <= _sz);
			memSet<T>(_item + beg, sz);
			return *this;
		}
		auto & resetOne(SzI const & i) { return reset(i, kSzOne); }
		auto & resetAfter(SzI const & beg = kThFirst) { return reset(beg, _sz - beg); }
		auto & resetFromEnd(SzI const & sz) { return reset(_sz - sz, sz); }
		auto & resetLast() { return resetOne(lastIdx()); }

		auto & tfree() {
			if (!isNoCap()) {
				delPtrArr(_item);
			}
			_item = kNullPtr;
			_cap = kSzNull;
			_sz = kSzNull;
			return *this;
		}
		auto & setNull() { return tfree(); }

		auto & alloc(SzI const & cap = kSzNone, Bool const & useCap = kFalse) {
			tfree();
			newPtr(_item, cap + kSzOne);
			_cap = cap;
			_sz = useCap ? cap : kSzNone;
			memSet<T>(_item, cap + kSzOne);
			return *this;
		}
		auto & allocThenNew(SzI const & cap = kSzNone) { return alloc(cap, kTrue); }

		auto & reAlloc(SzI const & cap) {
			precond(!isNoCap());
			precond(cap >= _sz);
			auto oriItem = _item;
			auto oriSz = _sz;
			_item = kNullPtr;
			_cap = kSzNull;
			_sz = kSzNull;
			alloc(cap);
			memCpy<T>(_item, oriItem, oriSz);
			_sz = oriSz;
			delPtrArr(oriItem);
			return *this;
		}
		auto & expand(SzI const & cap = kSzOne) { return reAlloc(_cap + cap); }
		auto & eraseSurplusCap() { return reAlloc(_sz); }

		auto maybeOverflow(SzI const & sz) const {
			precond(!isNoCap());
			return (_sz + sz) > _cap;
		}
		auto getCapAfterIns(SzI const & sz) const {
			precond(!isNoCap());
			return maybeOverflow(sz) ? static_cast<SzI>(_sz + sz) : _cap;
		}
		auto & processOverflow(SzI const & sz) {
			precond(!isNoCap());
			return maybeOverflow(sz) ? reAlloc(getCapAfterIns(sz)) : *this;
		}

		auto & cpy(T const * const & src, SzI const & sz = kSzOne) {
			precond(src != kNullPtr);
			allocThenNew(sz);
			memCpy<T>(_item, src, sz);
			return *this;
		}
		auto & cpy(ListB const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) {
			return src.isNull() ? setNull() : cpy(src._item + beg, (sz == kSzNull) ? (src._sz - beg) : sz);
		}

		auto & insProto(SzI const & beg, SzI const & sz, Bool clearItem) {
			precond(!isNoCap());
			precond(beg <= _sz);
			processOverflow(sz);
			for_cnt_lt(i, _sz, beg)
				get(i - kThSecond + sz) = get(i - kThSecond);
			_sz += sz;
			if (clearItem)
				claer(beg, sz);
			return *this;
		}

		auto & insSz(SzI const & beg, SzI const & sz) {
			insProto(beg, sz, kTrue);
			return *this;
		}
		auto & ins(T const & src, SzI const & beg, SzI const & sz = kSzOne) {
			insProto(beg, sz, kFalse);
			for_cnt(i, kThFirst, sz)
				get(beg + i) = src;
			return *this;
		}
		auto & ins(T const * const & src, SzI const & beg, SzI const & sz) {
			precond(src != kNullPtr);
			insProto(beg, sz, kFalse);
			for_cnt(i, kThFirst, sz)
				get(beg + i) = src[i];
			return *this;
		}
		auto & ins(ListB const & src, SzI const & beg, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) {
			precond(!src.isNull());
			precond(srcBeg < src._sz);
			precond(srcBeg + sz <= src._sz);
			return ins(src._item + srcBeg, beg, (sz == kSzNull) ? (src._sz - srcBeg) : sz);
		}

		auto & prependSz(SzI const & sz = kSzOne) { return insSz(kThFirst, sz); }
		auto & prepend(T const & src, SzI const & sz = kSzOne) { return ins(src, kThFirst, sz); }
		auto & prepend(T const * const & src, SzI const & sz) { return ins(src, kThFirst, sz); }
		auto & prepend(ListB const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { return ins(src, kThFirst, srcBeg, sz); }

		auto & appendSz(SzI const & sz = kSzOne) { return insSz(_sz, sz); }
		auto & append(T const & src, SzI const & sz = kSzOne) { return ins(src, _sz, sz); }
		auto & append(T const * const & src, SzI const & sz) { return ins(src, _sz, sz); }
		auto & append(ListB const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { return ins(src, _sz, srcBeg, sz); }

		auto & erase(SzI const & beg, SzI const & sz) {
			precond(!isNoCap());
			precond(beg < _sz);
			precond(beg + sz <= _sz);
			for_cnt(i, static_cast<SzI>(beg + sz), sz)
				get(i - sz) = get(i);
			clearAfter(_sz - sz);
			_sz -= sz;
			return *this;
		}
		auto & eraseOne(SzI const & i) { return erase(i, kSzOne); }
		auto & eraseAfter(SzI const & beg = kThFirst) { return erase(beg, _sz - beg); }
		auto & eraseFromEnd(SzI const & sz) { return erase(_sz - sz, sz); }
		auto & eraseLast() { return eraseOne(lastIdx()); }

		auto cnt(T const & v, SzI const & beg = kThFirst, SzI sz = kSzNull) const {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			auto cnt = kSzNone;
			for_cnt(i, kThFirst, sz)
				if (get(beg + i) == v)
					++cnt;
			return cnt;
		}
		auto & replace(T const & before, T const & after, SzI const & beg = kThFirst, SzI sz = kSzNull) {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			for_cnt(i, kThFirst, sz)
				if (get(beg + i) == before)
					get(beg + i) = after;
			return *this;
		}

		auto find(T const & v) const {
			precond(!isNull());
			SzI i;
			for_cnt_ev(i, kThFirst, _sz)
				if (get(i) == v)
					break;
			return (i == _sz) ? kThNull : i;
		}
		auto find(T const & v, SzI & i) const { return (i = find(v)) != kThNull; }
		auto exist(T const & v) const { return find(v) != kThNull; }

		auto findFromEnd(T const & v) const {
			precond(!isNull());
			SzI i;
			for_cnt_lt_ev(i, _sz, kThFirst)
				if (get(i - kThSecond) == v)
					break;
			return (i == kThFirst) ? kThNull : static_cast<SzI>(--i);
		}
		auto findFromEnd(T const & v, SzI & i) const { return (i = findFromEnd(v)) != kThNull; }

		auto findSub(ListB const & v) const {
			precond(!isNull());
			precond(!v.isNull());
			SzI i;
			for (i = kThFirst; i + v._sz <= _sz; ++i)
				if (cmp(_item + i, v._sz, v._item, v._sz))
					break;
			return (i + v._sz <= _sz) ? i : kThNull;
		}
		auto findSub(ListB const & v, SzI & i) const { return (i = findSub(v)) != kThNull; }
		auto existSub(ListB const & v) const { return findSub(v) != kThNull; }

		auto & eraseSub(ListB const & v) {
			precond(!isNull());
			precond(!v.isNull());
			SzI beg;
			if (findSub(v, beg))
				erase(beg, v._sz);
			return *this;
		}

		auto findDiff(ListB const & v) const {
			precond(!isNull());
			precond(!v.isNull());
			SzI i;
			for (i = kThFirst; i < _sz && i < v._sz; ++i)
				if (get(i) != v[i])
					break;
			return (i == _sz && i == v._sz) ? kThNull : i;
		}
		auto findDiff(ListB const & v, SzI & i) const { return (i = findDiff(v)) != kThNull; }

		static auto cmp(T const * const & v1, SzI const & sz1, T const * const & v2, SzI const & sz2) {
			return sz1 == sz2 && memCmp(v1, v2, static_cast<SzI>(sz1)) == MemCmpResult::kEqual;
		}
		auto cmp(T const * const & t, SzI const & sz) const {
			return cmp(_item, _sz, t, sz);
		};
		auto cmp(ListB const & t) const {
			return (isNull() && t.isNull()) || cmp(t._item, t._sz);
		};

		auto & sortUp() {
			precond(!isNull());
			for_cnt(i, kThFirst, _sz) {
				for_cnt(j, static_cast<SzI>(i + kThSecond), _sz)
					if (get(i) > get(j))
						swp(i, j);
			}
			return *this;
		}
		auto & sortDown() {
			precond(!isNull());
			for_cnt(i, kThFirst, _sz) {
				for_cnt(j, static_cast<SzI>(i + kThSecond), _sz)
					if (get(i) < get(j))
						swp(i, j);
			}
			return *this;
		}

		auto & operator[](SzI const & i) { return get(i); }
		auto & operator[](SzI const & i) const { return get(i); }

		auto operator==(ListB const & t) const { return cmp(t); }
		auto operator!=(ListB const & t) const { return !cmp(t); }

		auto & operator=(ListB const & t) { return cpy(t); }

		auto & operator+=(ListB const & t) { return append(t); }

		auto operator+(ListB const & t) const {
			ListB r; r.alloc(_sz + t._sz).append(*this).append(t);
			return r;
		}

		~ListB() { tfree(); }
		ListB() : _item(kNullPtr), _cap(kSzNull), _sz(kSzNull) {}
		ListB(ListB const & t) : ListB() { cpy(t); }
		explicit ListB(SzI const & cap, Bool const & useCap = kFalse) : ListB() { alloc(cap, useCap); }
		explicit ListB(T const * const & src, SzI const & sz = kSzOne) : ListB() { cpy(src, sz); }
		explicit ListB(ListB const & src, SzI const & beg, SzI const & sz = kSzNull) : ListB() { cpy(src, beg, sz); }

		auto & getListB() { return *this; }
		auto & getListB() const { return *this; }
	};/*
	tmpl_st(T) struct ListBRef : ListB<T> {

		auto & ref(T const * const & src, SzI const & sz = kSzNull) {
			_cap = kSzNull;
			_item = const_cast<T *>(reinterpret_cast<T const *>(src));
			_sz = sz;
			return *this;
		}
		auto & ref(ListB<T> const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) {
			_cap = kSzNull;
			_item = src._item + beg;
			_sz = sz;
			return *this;
		}
		auto & freeRef() {
			_cap = kSzNull;
			_item = kNullPtr;
			_sz = kSzNull;
			return *this;
		}

		~ListBRef() { freeRef(); }
		ListBRef() {}
		explicit ListBRef(T const * const & src, SzI const & sz = kSzNull) { ref(src, sz); }
		explicit ListBRef(ListB<T> const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) { ref(src, beg, sz); }

		auto & operator=(ListB<T> const & t) { return ref(t); }
		auto & operator+=(SzI const & pos) {
			_item += pos;
			_sz -= pos;
			return *this;
		}
		auto & operator-=(SzI const & pos) {
			_item -= pos;
			_sz += pos;
			return *this;
		}
		auto & operator++() { return (*this) += kThSecond; }
		auto & operator--() { return (*this) -= kThSecond; }
	};*/

}
