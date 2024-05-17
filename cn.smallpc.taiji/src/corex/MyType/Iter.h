# pragma once

# include "../Type.h"

# include "../Const.h"
# include "../UtilKit/PtrUtil.h"

namespace $TJ {
	tmpl_dt(TList, TSub) struct Iter {
		TList & _list;
		SzI _idx;

		Iter() : _list(kNullPtr), _idx(kThNull) {}
		explicit Iter(TList & list, SzI const & i) : _list(list), _idx(i) {}
		explicit Iter(TList const & list, SzI const & i) : _list(const_cast<TList &>(list)), _idx(i) {}

		auto operator!=(Iter const & t) const { return _idx != t._idx; }
		auto & operator++() {
			++_idx;
			return *this;
		}
		auto & operator*() { return _list[_idx]; }
		auto const & operator*() const { return _list[_idx]; }

	};
}
