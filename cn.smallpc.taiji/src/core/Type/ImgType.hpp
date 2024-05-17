#pragma once

#include "../Type.hpp"
#include "../Const.hpp"
#include "../TypeUtil.hpp"
#include "./JSON.hpp"

namespace $TJ {

	template <typename T>
	struct TypeNullTemp {};

	template <>
	struct TypeNullTemp<Sz> {

		static inline constexpr Sz kNull = kNoX;

	};

	template <>
	struct TypeNullTemp<SSz> {

		static inline constexpr SSz kNull = kSNoX;

	};

	template <typename T, Bool $enableNull = kBoF>
	struct XYPair : TypePair<T> {

		T & $x;
		T & $y;

		auto isNull() const {
			if constexpr ($enableNull) {
				return $x == TypeNullTemp<T>::kNull && $y == TypeNullTemp<T>::kNull;
			} else {
				static_assert_failed("!enable-null");
			}
		}

		auto & setNull() {
			if constexpr ($enableNull) {
				TypePair<T>::set(TypeNullTemp<T>::kNull, TypeNullTemp<T>::kNull);
			} else {
				static_assert_failed("!enable-null");
			}
			return thiz;
		}
		auto & setZero() {
			TypePair<T>::set(T(0), T(0));
			return thiz;
		}

		auto & read($JSON::Arr const & t) {
			if constexpr ($enableNull) {
				t[kNo1].get($x, TypeNullTemp<T>::kNull);
				t[kNo2].get($y, TypeNullTemp<T>::kNull);
			} else {
				t[kNo1].get($x);
				t[kNo2].get($y);
			}
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2);
			if constexpr ($enableNull) {
				t.add($x, $x == TypeNullTemp<T>::kNull);
				t.add($y, $y == TypeNullTemp<T>::kNull);
			} else {
				t.add($x);
				t.add($y);
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto & operator=(XYPair const & t) {
			TypePair<T>::operator=(t);
			return thiz;
		}

		implicit XYPair() :
			TypePair<T>(), $x(TypePair<T>::$a), $y(TypePair<T>::$b) {
			if constexpr ($enableNull)
				TypePair<T>::set(TypeNullTemp<T>::kNull, TypeNullTemp<T>::kNull);
		}
		implicit XYPair(XYPair const & t) :
			TypePair<T>(t), $x(TypePair<T>::$a), $y(TypePair<T>::$b) {
		}
		explicit XYPair(Sz const & x, Sz const & y) :
			TypePair<T>(x, y), $x(TypePair<T>::$a), $y(TypePair<T>::$b) {
		}

	};

	using ImgPos = XYPair<Sz, kBoT>;
	using ImgSgnPos = XYPair<SSz, kBoT>;

	struct ImgSz : ImgPos {

		Sz & $w;
		Sz & $h;

		auto calcSz() const {
			return static_cast<Sz>($w * $h);
		}
		auto calcByteSz(Sz const & bpp) const {
			return static_cast<Sz>(calcSz() * bpp / 8);
		}

		auto & operator=(ImgSz const & t) {
			ImgPos::operator=(t);
			return thiz;
		}

		implicit ImgSz() :
			ImgPos(), $w($x), $h($y) {
		}
		implicit ImgSz(ImgSz const & t) :
			ImgPos(t), $w($x), $h($y) {
		}
		explicit ImgSz(Sz const & w, Sz const & h) :
			ImgPos(w, h), $w($x), $h($y) {
		}

	};

	struct ImgArea {

		ImgPos $pos;
		ImgSz $sz;

		auto & set(Sz const & x, Sz const & y, Sz const & w, Sz const & h) {
			$pos.set(x, y);
			$sz.set(w, h);
			return thiz;
		}

		auto & read($JSON::Arr const & t) {
			t[kNo1].get($pos.$x, kSzX);
			t[kNo2].get($pos.$y, kSzX);
			t[kNo3].get($sz.$w, kSzX);
			t[kNo4].get($sz.$h, kSzX);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(4);
			t.add($pos.$x, $pos.$x == kSzX);
			t.add($pos.$y, $pos.$y == kSzX);
			t.add($sz.$w, $sz.$w == kSzX);
			t.add($sz.$h, $sz.$h == kSzX);
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

	};

}
