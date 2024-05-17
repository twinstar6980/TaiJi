# pragma once

# include "../Type.h"

namespace $TJ {
	tmpl_st(T) struct PairVal {
		T _left, _right;

		auto & set(T const & left, T const & right) {
			_left = left;
			_right = right;
			return *this;
		}
		auto & cpy(PairVal const & src) {
			_left = src._left;
			_right = src._right;
			return *this;
		}

		PairVal() : _left(), _right() {}
		constexpr explicit PairVal(T const & left, T const & right) : _left(left), _right(right) {}

	};
}
