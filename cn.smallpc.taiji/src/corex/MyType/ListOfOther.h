# pragma once

# include "../Type.h"
# include "../Const.h"

# include "./ListP.h"
# include "./JSON.h"

namespace $TJ {
		/*
		auto & setNull() { (*this).getListP().setNull(); return *this; }
		auto & alloc(SzI const & cap = kSzNone, SzI const & useCap = kFalse) { (*this).getListP().alloc(cap, useCap); return *this; }
		auto & allocThenNew(SzI const & cap = kSzNone) { (*this).getListP().allocThenNew(cap); return *this; }
		auto & appendSz(SzI const & sz = kSzOne) { (*this).getListP().appendSz(sz); return *this; }
		auto & append(T const & src, SzI const & sz = kSzOne) { (*this).getListP().append(src, sz); return *this; }
		auto & append(T const * const * const & src, SzI const & sz = kSzOne) { (*this).getListP().append(src, sz); return *this; }
		auto & append(ListP const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { (*this).getListP().append(src, srcBeg, sz); return *this; }
*/
	tmpl_st(T) struct ListOfInfo : ListP<T> {

		auto & deserialize($JSON::Arr const & t) {
			precond(!t.isNull());
			alloc(t._sz);
			for_criter(e, t)
				add().last().deserialize(e);
			return *this;
		}
		auto & serialize($JSON::Arr & t) const {
			precond(!(*this).isNull());
			t.alloc((*this)._sz);
			for_criter(e, *this)
				e.serialize(t.addObj().last());
			return t;
		}
		auto & deserialize($JSON::Obj const & t) {
			precond(!t.isNull());
			alloc(t._sz);
			for_criter(e, t)
				add().last().deserialize(e);
			return *this;
		}
		auto & serialize($JSON::Obj & t) const {
			precond(!(*this).isNull());
			t.alloc((*this)._sz);
			for_criter(e, *this)
				e.serialize(t.addObj().last());
			return t;
		}

		tmpl_st(TBlock) auto & deserialize(ListP<TBlock> const & t) {
			precond(!t.isNull());
			alloc(t._sz);
			for_criter(e, t)
				add().last().deserialize(e);
			return *this;
		}
		tmpl_st(TBlock) auto & serialize(ListP<TBlock> & t) const {
			precond(!(*this).isNull());
			t.alloc((*this)._sz);
			for_criter(e, *this)
				e.serialize(t.add().last());
			return *this;
		}
	};
	tmpl_st(T) struct ListOfBlock : ListP<T> {

		auto getBlockSz(SzI const & beg = kThFirst, SzI sz = kSzNull) const {
			precond(!(*this).isNull());
			if (sz == kSzNull) sz = (*this)._sz - beg;
			auto result = kSzNone;
			for_cnt(i, kThFirst, sz)
				result += (*this)[beg + i].getBlockSz();
			return result;
		}

		auto & read(Byte const * const & src, SzI const & sz) {
			(*this).allocThenNew(sz);
			auto pos = kThFirst;
			for_riter(e, *this)
				pos += e.read(src + pos).getSz();
			return *this;
		}
		auto & write(Byte * const & dst) const {
			precond(!(*this).isNull());
			auto pos = kThFirst;
			for_criter(e, *this)
				pos += e.write(dst + pos).getSz();
			return *this;
		}
	};
}
