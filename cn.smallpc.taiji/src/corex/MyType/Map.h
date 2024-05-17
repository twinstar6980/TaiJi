#pragma once

# include "../Type.h"
# include "./Iter.h"

# include "../Const.h"
# include "../UtilKit/PtrUtil.h"
# include "./ListP.h"

# include "./JSON.h"

namespace $TJ {
	tmpl_dt(TKey, TVal) struct MapItem {
		TKey _key;
		TVal _val;

		auto & setKey(TKey const & key) {
			_key = key;
			return *this;
		}
		auto & setVal(TVal const & val) {
			_val = val;
			return *this;
		}
		auto & set(TKey const & key, TVal const & val) {
			_key = key;
			_val = val;
			return *this;
		}

		auto & cpy(MapItem const & src) {
			_key = src._key;
			_val = src._val;
			return *this;
		}

		auto cmp(MapItem const & t) { return _key == t._key && _val == t._val; }

		MapItem() : _key(), _val() {}
		explicit MapItem(TKey const & key, TVal const & val) : _key(key), _val(val) {}

		auto operator==(MapItem const & t) { return cmp(t); }
		auto operator!=(MapItem const & t) { return !cmp(t); }

		auto & operator=(MapItem const & t) { return cpy(t); }

	};
	tmpl_dt(TKey, TVal) struct Map : ListP<MapItem<TKey, TVal>> {

		auto & appendSz(SzI const & sz = kSzOne) { (*this).getListP().appendSz(sz); return *this; }
		auto & append(MapItem const & src, SzI const & sz = kSzOne) { (*this).getListP().append(src, sz); return *this; }
		auto & append(ListP const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { (*this).getListP().append(src, srcBeg, sz); return *this; }

		auto & getKey(SzI const & i) { return (*this).get(i)._key; }
		auto & getVal(SzI const & i) { return (*this).get(i)._val; }
		auto & getKey(SzI const & i) const { return (*this).get(i)._key; }
		auto & getVal(SzI const & i) const { return (*this).get(i)._val; }

		auto & lastKey() { return (*this).last()._key; }
		auto & lastVal() { return (*this).last()._val; }
		auto & lastKey() const { return (*this).last()._key; }
		auto & lastVal() const { return (*this).last()._val; }

		auto & appendByKey(TKey const & key) {
			appendSz().last().setKey(key);
			return *this;
		}
		auto & appendByVal(TVal const & val) {
			appendSz().last().setVal(val);
			return *this;
		}
		auto & appendByKV(TKey const & key, TVal const & val) {
			appendSz().last().set(key, val);
			return *this;
		}

		auto findKey(TKey const & key) const {
			precond(!(*this).isNull());
			SzI i;
			for_cnt_ev(i, kThFirst, (*this)._sz)
				if ((*this).get(i)._key == key)
					break;
			return (i == (*this)._sz) ? kThNull : i;
		}
		auto findKey(TKey const & key, SzI & i) const { return (i = findKey(key)) != kThNull; }
		auto existKey(TKey const & key) const { return findKey(key) != kThNull; }

		auto findVal(TVal const & val) const {
			precond(!(*this).isNull());
			SzI i;
			for_cnt_ev(i, kThFirst, (*this)._sz)
				if ((*this).get(i)._val == val)
					break;
			return (i == (*this)._sz) ? kThNull : i;
		}
		auto findVal(TVal const & val, SzI & i) const { return (i = findVal(val)) != kThNull; }
		auto existVal(TVal const & val) const { return findVal(val) != kThNull; }

		auto & getByKey(TKey const & key) { return get(findKey(key)); }
		auto & getByVal(TVal const & val) { return get(findVal(val)); }
		auto & getByKey(TKey const & key) const { return get(findKey(key)); }
		auto & getByVal(TVal const & val) const { return get(findVal(val)); }

		auto & getKeyByKey(TKey const & key) { return getKey(findKey(key)); }
		auto & getKeyByVal(TVal const & val) { return getKey(findVal(val)); }
		auto & getKeyByKey(TKey const & key) const { return getKey(findKey(key)); }
		auto & getKeyByVal(TVal const & val) const { return getKey(findVal(val)); }

		auto & getValByKey(TKey const & key) { return getVal(findKey(key)); }
		auto & getValByVal(TVal const & val) { return getVal(findVal(val)); }
		auto & getValByKey(TKey const & key) const { return getVal(findKey(key)); }
		auto & getValByVal(TVal const & val) const { return getVal(findVal(val)); }

		auto & eraseByKey(TKey const & key) {
			precond(!(*this).isNoCap());
			SzI i;
			if (findKey(key, i))
				(*this).erase(i);
			return *this;
		}

		auto & sortUpByKey() {
			precond(!(*this).isNull());
			for_cnt(i, kThFirst, (*this)._sz) {
				for_cnt(j, static_cast<SzI>(i + kThSecond), (*this)._sz)
					if (getKey(i) > getKey(j))
						(*this).swp(i, j);
			}
			return *this;
		}
		auto & sortUpByVal() {
			precond(!(*this).isNull());
			for_cnt(i, kThFirst, (*this)._sz) {
				for_cnt(j, static_cast<SzI>(i + kThSecond), (*this)._sz)
					if (getVal(i) > getVal(j))
						(*this).swp(i, j);
			}
			return *this;
		}
		auto & sortDownByKey() {
			precond(!(*this).isNull());
			for_cnt(i, kThFirst, (*this)._sz) {
				for_cnt(j, static_cast<SzI>(i + kThSecond), (*this)._sz)
					if (getKey(i) < getKey(j))
						(*this).swp(i, j);
			}
			return *this;
		}
		auto & sortDownByVal() {
			precond(!(*this).isNull());
			for_cnt(i, kThFirst, (*this)._sz) {
				for_cnt(j, static_cast<SzI>(i + kThSecond), (*this)._sz)
					if (getVal(i) < getVal(j))
						(*this).swp(i, j);
			}
			return *this;
		}

		auto & gtMap() { return *this; }
		auto & gtMap() const { return *this; }
	};
	tmpl_st(T) using MapItemOfInfo = MapItem<Str, T>;
	tmpl_st(T) struct MapOfInfo : Map<Str, T> {
		auto & deserialize($JSON::Obj const & src) {
			precond(!src.isNull());
			alloc(src._sz);
			for_criter(e, src)
				(*this).appendByKey(e._key.get()).lastVal().deserialize(e._val.getObj());
			return *this;
		}
		auto & serialize($JSON::Obj & dst) const {
			precond(!(*this).isNull());
			dst.alloc((*this)._sz);
			for_criter(e, *this)
				e._val.serialize(dst.addObj(e._key).last()._val.getObj());
			return dst;
		}
	};
}
