#	pragma once

#	include "../Type.h"
#	include "./PairVal.h"

namespace $TJ {
	tmpl_st(T) struct Interval {
		PairVal<T> _limit;
		PairVal<Bool> _state;

		T & _min, & _max;
		Bool & _leftOpen, & _rightOpen;

		auto & setLimit(T const & min, T const & max) {
			_limit.set(min, max);
			return *this;
		}
		auto & setState(Bool const & leftOpen, Bool const & rightOpen) {
			_state.set(leftOpen, rightOpen);
			return *this;
		}

		auto getDiff() const { return static_cast<T>(_max - _min); }

		auto btw(T const & v) const {
			return ternary_oper(_leftOpen, v > _min, v >= _min)
				&& ternary_oper(_rightOpen, v < _max, v <= _max);
		}

		Interval() :
			_limit(), _state(),
			_min(_limit._left), _max(_limit._right),
			_leftOpen(_state._left), _rightOpen(_state._right) {
		}
		constexpr explicit Interval(T const & min, T const & max, Bool const & leftOpen, Bool const & rightOpen) :
			_limit(min, max), _state(leftOpen, rightOpen),
			_min(_limit._left), _max(_limit._right),
			_leftOpen(_state._left), _rightOpen(_state._right) {
		}

	};
}
