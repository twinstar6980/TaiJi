# pragma once

# include "../Type.h"
# include "./Iter.h"

# include "../Const.h"
# include "../UtilKit/PtrUtil.h"

# include "./ListB.h"

namespace $TJ {
	tmpl_st(T) struct ListP : ListB<T *> {

		auto getIter(SzI const & i) {
			precond(!(*this).isNull());
			precond(i <= (*this)._sz);
			return Iter<ListP, T>(*this, i);
		}
		auto getIter(SzI const & i) const {
			precond(!(*this).isNull());
			precond(i <= (*this)._sz);
			return Iter<ListP, T>(*this, i);
		}

		auto begin() { return getIter(kThFirst); }
		auto end() { return getIter((*this)._sz); }

		auto begin() const { return getIter(kThFirst); }
		auto end() const { return getIter((*this)._sz); }

		auto & get(SzI const & i) {
			precond(!(*this).isNull());
			precond(i < (*this)._sz);
			return *_item[i];
		}
		auto const & get(SzI const & i) const {
			precond((*this).isNull());
			precond(i < (*this)._sz);
			return *_item[i];
		}
		auto & first() { return get(kThFirst); }
		auto & first() const { return get(kThFirst); }
		auto & last() { return get((*this).lastIdx()); }
		auto & last() const { return get((*this).lastIdx()); }

		auto & clear(SzI const & beg, SzI const & sz) {
			precond(!(*this).isNull());
			precond(beg < (*this)._sz);
			precond(beg + sz <= (*this)._sz);
			for_cnt(i, kThFirst, sz)
				delPtrSingle((*this)._item[beg + i]);
			return *this;
		}
		auto & clearOne(SzI const & i) { return clear(i, kSzOne); }
		auto & clearAfter(SzI const & beg = kThFirst) { return clear(beg, (*this)._sz - beg); }
		auto & clearFromEnd(SzI const & sz) { return clear((*this)._sz - sz, sz); }
		auto & clearLast() { return clearOne(lastIdx()); }

		auto & reset(SzI const & beg, SzI const & sz) {
			precond(!(*this).isNull());
			precond(beg < (*this)._sz);
			precond(beg + sz <= (*this)._sz);
			for_cnt(i, kThFirst, sz)
				newPtr(delPtrSingle((*this)._item[beg + i]));
			return *this;
		}
		auto & resetOne(SzI const & i) { return reset(i, kSzOne); }
		auto & resetAfter(SzI const & beg = kThFirst) { return reset(beg, (*this)._sz - beg); }
		auto & resetFromEnd(SzI const & sz) { return reset((*this)._sz - sz, sz); }
		auto & resetLast() { return resetOne(lastIdx()); }

		auto & tfree() {
			if (!(*this).isNoCap()) {
				while ((*this)._sz > kSzNone)
					delPtrSingle((*this)._item[--(*this)._sz]);
			}
			(*this).getListB().tfree();
			return *this;
		}
		auto & setNull() { return tfree(); }

		auto & alloc(SzI const & cap = kSzNone, Bool const & useCap = kFalse) {
			tfree().getListB().alloc(cap);
			if (useCap) {
				while ((*this)._sz < cap)
					newPtr((*this)._item[(*this)._sz++]);
			}
			return *this;
		}
		auto & allocThenNew(SzI const & cap = kSzNone) { return alloc(cap, kTrue); }

		auto & cpy(T const * const & src, SzI const & sz = kSzOne) {
			precond(src != kNullPtr);
			allocThenNew(sz);
			for_cnt(i, kThFirst, sz)
				get(i) = src[i];
			return *this;
		}
		auto & cpy(ListP const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) {
			return src.isNull() ? setNull() : cpy(src._item + beg, (sz == kSzNull) ? (src._sz - beg) : sz);
		}

		auto & insSz(SzI const & beg, SzI const & sz) {
			(*this).insProto(beg, sz, kFalse);
			for_cnt(i, kThFirst, sz)
				newPtr((*this)._item[beg + i]);
			return *this;
		}
		auto & ins(T const & src, SzI const & beg, SzI const & sz = kSzOne) {
			(*this).insProto(beg, sz, kFalse);
			for_cnt(i, kThFirst, sz)
				(*newPtr((*this)._item[beg + i])) = src;
			return *this;
		}
		auto & ins(T const * const & src, SzI const & beg, SzI const & sz) {
			precond(src != kNullPtr);
			(*this).insProto(beg, sz, kFalse);
			for_cnt(i, kThFirst, sz)
				(*newPtr((*this)._item[beg + i])) = src[i];
			return *this;
		}
		auto & ins(ListP const & src, SzI const & beg, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) {
			precond(!src.isNull());
			precond(srcBeg < src._sz);
			precond(srcBeg + sz <= src._sz);
			return ins(src._item + srcBeg, beg, (sz == kSzNull) ? (src._sz - srcBeg) : sz);
		}

		auto & prependSz(SzI const & sz = kSzOne) { return insSz(kThFirst, sz); }
		auto & prepend(T const & src, SzI const & sz = kSzOne) { return ins(src, kThFirst, sz); }
		auto & prepend(T const * const & src, SzI const & sz) { return ins(src, kThFirst, sz); }
		auto & prepend(ListP const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { return ins(src, kThFirst, srcBeg, sz); }

		auto & appendSz(SzI const & sz = kSzOne) { return insSz((*this)._sz, sz); }
		auto & append(T const & src, SzI const & sz = kSzOne) { return ins(src, (*this)._sz, sz); }
		auto & append(T const * const & src, SzI const & sz) { return ins(src, (*this)._sz, sz); }
		auto & append(ListP const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { return ins(src, (*this)._sz, srcBeg, sz); }

		//auto & appendSzFull() { return appendSz((*this).getDiffOfFull()); }

		auto & erase(SzI const & beg, SzI const & sz) {
			precond(!isNoCap());
			precond(beg < (*this)._sz);
			precond(beg + sz <= (*this)._sz);
			for_cnt(i, static_cast<SzI>(beg + sz), sz)
				get(i - sz) = get(i);
			clearAfter((*this)._sz - sz);
			_sz -= sz;
			return *this;
		}
		auto & eraseOne(SzI const & i) { return erase(i, kSzOne); }
		auto & eraseAfter(SzI const & beg = kThFirst) { return erase(beg, (*this)._sz - beg); }
		auto & eraseFromEnd(SzI const & sz) { return erase((*this)._sz - sz, sz); }
		auto & eraseLast() { return eraseOne((*this).lastIdx()); }

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

		auto findSub(ListP const & v) const {
			precond(!isNull());
			precond(!v.isNull());
			SzI i;
			for (i = kThFirst; i + v._sz <= _sz; ++i)
				if (cmp(_item + i, v._sz, v._item, v._sz))
					break;
			return (i + v._sz <= _sz) ? i : kThNull;
		}
		auto findSub(ListP const & v, SzI & i) const { return (i = findSub(v)) != kThNull; }
		auto existSub(ListP const & v) const { return findSub(v) != kThNull; }

		auto & eraseSub(ListP const & v) {
			precond(!isNull());
			precond(!v.isNull());
			SzI beg;
			if (findSub(v, beg))
				erase(beg, v._sz);
			return *this;
		}

		auto findDiff(ListP const & v) const {
			precond(!isNull());
			precond(!v.isNull());
			SzI i;
			for (i = kThFirst; i < _sz && i < v._sz; ++i)
				if (get(i) != v[i])
					break;
			return (i == _sz && i == v._sz) ? kThNull : i;
		}
		auto findDiff(ListP const & v, SzI & i) const { return (i = findDiff(v)) != kThNull; }

		static auto cmp(T const * const & v1, SzI const & sz1, T const * const & v2, SzI const & sz2) {
			if (sz1 != sz2) return kFalse;
			SzI i;
			for_cnt_ev(i, kThFirst, sz1) {
				if (v1[i] != v2[i])
					break;
			}
			return i < sz1;
		}
		auto cmp(T const * const & t, SzI const & sz) const {
			return cmp(_item, _sz, t, sz);
		};
		auto cmp(ListP const & t) const {
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

		auto operator==(ListP const & t) const { return cmp(t); }
		auto operator!=(ListP const & t) const { return !cmp(t); }

		auto & operator=(ListP const & t) { return cpy(t); }

		auto & operator+=(ListP const & t) { return append(t); }

		auto operator+(ListP const & t) const {
			ListP r; r.alloc(_sz + t._sz).append(*this).append(t);
			return r;
		}

		~ListP() { tfree(); }
		ListP() : ListB<T *>() {}
		ListP(ListP const & t) : ListP() { cpy(t); }
		explicit ListP(SzI const & cap, Bool const & useCap = kFalse) : ListP() { alloc(cap, useCap); }
		explicit ListP(T const * const & src, SzI const & sz = kSzOne) : ListP() { cpy(src, sz); }
		explicit ListP(ListP const & src, SzI const & beg, SzI sz = kSzNull) : ListP() { cpy(src, beg, sz); }

		auto & getListP() { return *this; }
		auto & getListP() const { return *this; }
	};
}
