# pragma once

# include "../Type.h"
# include "./Str.h"
# include "./ListP.h"

# include "../Const.h"

namespace $TJ {
	struct StrList : ListP<Str> {

		auto & appendSz(SzI const & sz = kSzOne) { getListP().appendSz(sz); return *this; }
		auto & append(Str const & src, SzI const & sz = kSzOne) { getListP().append(src, sz); return *this; }
		auto & append(ListP const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) { getListP().append(src, beg, sz); return *this; }

		auto find(Str const & s) const { return getListP().find(s); }
		auto find(Str const & s, SzI & i) const { return getListP().find(s, i); }
		auto exist(Str const & s) const { return getListP().exist(s); }

		auto & append(Ch const * const & str, SzI const & len = kSzNull) { return append(StrRef(str, len)); }
		auto find(Ch const * const & s) const { return find(StrRef(s)); }
		auto find(Ch const * const & s, SzI & i) const { return find(StrRef(s), i); }
		auto exist(Ch const * const & s) const { return exist(StrRef(s)); }

		auto & toLowerCase(SzI const & beg = kThFirst, SzI sz = kSzNull) {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			for_cnt(i, kThFirst, sz)
				get(beg + i).toLowerCase();
			return *this;
		}
		auto & toUpperCase(SzI const & beg = kThFirst, SzI sz = kSzNull) {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			for_cnt(i, kThFirst, sz)
				get(beg + i).toUpperCase();
			return *this;
		}

		auto getStrLen(SzI const & beg = kThFirst, SzI sz = kSzNull) const {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			auto len = kSzNone;
			for_cnt(i, kThFirst, sz)
				len += get(beg + i).getLen();
			return len;
		}
		auto getStrLenRaw(SzI const & beg = kThFirst, SzI sz = kSzNull) const {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			auto len = kSzNone;
			for_cnt(i, kThFirst, sz)
				len += get(beg + i).getLenRaw();
			return len;
		}

		auto & fromRaw(Ch const * const & src, SzI sz = kSzNull, Ch const & delimiter = $Char::kNull) {
			precond(!isNull());
			if (sz == kSzNull) sz = _cap - _sz;
			else processOverflow(sz);
			auto pos = kThFirst;
			for_cnt(i, kThFirst, sz)
				pos += appendSz().last().fromRaw(src + pos, delimiter).getLenRaw();
			return *this;
		}
		auto & toRaw(Ch * const & dst, SzI const & beg = kThFirst, SzI sz = kSzNull, Ch const & delimiter = $Char::kNull) const {
			precond(!isNull());
			if (sz == kSzNull) sz = _sz - beg;
			auto pos = kThFirst;
			for_cnt(i, kThFirst, sz)
				pos += get(beg + i).toRaw(dst + pos, delimiter).getLenRaw();
			return *this;
		}

		auto & fromRaw(Str const & src, SzI sz = kSzNull, Byte const & delimiter = kNullByte) { return fromRaw(src._str, sz, delimiter); }
		auto & toRaw(Str & dst, SzI const & beg = kThFirst, SzI sz = kSzNull, Byte const & delimiter = kNullByte) const {
			dst._len = dst.alloc(getStrLenRaw())._cap;
			return toRaw(dst._str, beg, sz, delimiter);
		}

		auto & fromRaw(Byte const * const & src, SzI sz = kSzNull, Byte const & delimiter = kNullByte) { return fromRaw(reinterpret_cast<Ch const *>(src), sz, delimiter); }
		auto & toRaw(Byte * const & dst, SzI const & beg = kThFirst, SzI sz = kSzNull, Byte const & delimiter = kNullByte) const { return toRaw(reinterpret_cast<Ch *>(dst), beg, sz, delimiter); }

		StrList() : ListP() {}
		StrList(StrList const & t) : StrList() { cpy(t); }
		explicit StrList(SzI const & cap, Bool const & useCap = kFalse) : StrList() { alloc(cap, useCap); }
		explicit StrList(Str const * const & src, SzI const & sz = kSzOne) : StrList() { cpy(src, sz); }
		explicit StrList(ListP const & src, SzI const & beg = kThFirst, SzI sz = kSzNull) : StrList() { cpy(src, beg, sz); }

		auto & getStrList() { return *this; }
		auto & getStrList() const { return *this; }
	};
}
