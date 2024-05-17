# pragma once

# include "../Type.h"
# include "../Const.h"

# include "./Iter.h"

# include "../UtilKit/PtrUtil.h"
# include "../UtilKit/MemoryUtil.h"
# include "./Interval.h"
# include "./ListB.h"

uint32_t f_strlen(char const * const & s);
int f_strcmp(char const * const & s1, char const * const & s2);
int f_stricmp(char const * const & s1, char const * const & s2);

namespace $TJ {

	constexpr Interval<Ch> kCharIntervalOfNumber('0', '9', kFalse, kFalse);
	constexpr Interval<Ch> kCharIntervalOfOctNumber('0', '7', kFalse, kFalse);
	constexpr Interval<Ch> kCharIntervalOfLowerCase('a', 'z', kFalse, kFalse);
	constexpr Interval<Ch> kCharIntervalOfUpperCase('A', 'Z', kFalse, kFalse);
	constexpr Interval<Ch> kCharIntervalOfNonCtrlChar(' ', '~', kFalse, kFalse);
	constexpr UI8 kDiffOfCharCase('a' - 'A');

	SzI calcEscCharSize(Ch const & src);
	SzI readEscChar(Ch const * const & src, Ch & dst);
	SzI writeEscChar(Ch const & src, Ch * const & dst);

	struct Str : ListB<Ch> {
		Ch *& _str;
		SzI & _len;

		auto getLen() const { return _len; }
		auto getLenRaw() const { return static_cast<SzI>(_len + kSzOne); }

		auto & setNull() { getListB().setNull(); return *this; }

		auto & cpy(Ch const * const & src, SzI const & sz = kSzNull) { getListB().cpy(src, (sz == kSzNull) ? f_strlen(src) : sz); return *this; }
		auto & cpy(ListB const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) { getListB().cpy(src, beg, sz); return *this; }

		auto & insSz(SzI const & beg, SzI const & sz) { getListB().insSz(beg, sz); return *this; }
		auto & ins(Ch const & src, SzI const & beg, SzI const & sz = kSzOne) { getListB().ins(src, beg, sz); return *this; }
		auto & ins(Ch const * const & src, SzI const & beg, SzI const & sz = kSzNull) { getListB().ins(src, beg, (sz == kSzNull) ? f_strlen(src) : sz); return *this; }
		auto & ins(ListB const & src, SzI const & beg, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { getListB().ins(src, beg, srcBeg, sz); return *this; }

		auto & prependSz(SzI const & sz = kSzOne) { getListB().prependSz(sz); return *this; }
		auto & prepend(Ch const & src, SzI const & sz = kSzOne) { getListB().prepend(src, sz); return *this; }
		auto & prepend(Ch const * const & src, SzI const & sz = kSzNull) { getListB().prepend(src, (sz == kSzNull) ? f_strlen(src) : sz); return *this; }
		auto & prepend(ListB const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { getListB().prepend(src, srcBeg, sz); return *this; }

		auto & appendSz(SzI const & sz = kSzOne) { getListB().appendSz(sz); return *this; }
		auto & append(Ch const & src, SzI const & sz = kSzOne) { getListB().append(src, sz); return *this; }
		auto & append(Ch const * const & src, SzI const & sz = kSzNull) { getListB().append(src, (sz == kSzNull) ? f_strlen(src) : sz); return *this; }
		auto & append(ListB const & src, SzI const & srcBeg = kThFirst, SzI const & sz = kSzNull) { getListB().append(src, srcBeg, sz); return *this; }

		auto & toLowerCase(SzI const & beg = kThFirst, SzI sz = kSzNull) {
			precond(!isNull());
			precond(beg < _sz);
			precond(beg + sz <= _sz);
			if (sz == kSzNull) sz = _sz - beg;
			for_cnt(i, kThFirst, sz)
				if (kCharIntervalOfUpperCase.btw(get(beg + i)))
					get(beg + i) += kDiffOfCharCase;
			return *this;
		}
		auto & toUpperCase(SzI const & beg = kThFirst, SzI sz = kSzNull) {
			precond(!isNull());
			precond(beg < _sz);
			precond(beg + sz <= _sz);
			if (sz == kSzNull) sz = _sz - beg;
			for_cnt(i, kThFirst, sz)
				if (kCharIntervalOfLowerCase.btw(get(beg + i)))
					get(beg + i) -= kDiffOfCharCase;
			myAssert(isNull());
			return *this;
		}

		auto cmp(ListB const & src) const { return getListB().cmp(src); }
		auto cmp(Ch const * const & src, SzI const & sz = kSzNull) const { return getListB().cmp(src, (sz == kSzNull) ? f_strlen(src) : sz); }

		static auto cmp(Ch const * const & s1, Ch const * const & s2) { return ListB::cmp(s1, f_strlen(s1), s2, f_strlen(s2)); }

		auto getEscLen() const {
			precond(!isNull());
			auto len = kSzNone;
			for_cnt(i, kThFirst, _len) {
				++len;
				if (!kCharIntervalOfNonCtrlChar.btw(get(i)))
					len += calcEscCharSize(get(i));
			}
			return len;
		}

		auto & fromRaw(Ch const * const & src, Ch const & delimiter = $Char::kNull) {
			for (auto i = kThFirst; ; ++i)
				if (src[i] == delimiter) {
					alloc(i).append(src, i);
					break;
				}
			return *this;
		}
		auto & toRaw(Ch * const & dst, Ch const & delimiter = $Char::kNull) const {
			precond(!isNull());
			memCpy(dst, _str, _len);
			dst[_len] = delimiter;
			return *this;
		}
		auto & fromRawEsc(Ch const * const & src, SzI const & sz) {
			alloc(sz);
			for_cnt(i, kThFirst, sz) {
				get(_len) = src[i++];
				if (get(_len) == '\\')
					i += readEscChar(src + i, get(_len));
			}
			return *this;
		}
		auto toRawEsc(Ch * const & dst) const {
			auto sz = kSzNone;
			for_riter(c, *this) {
				if (kCharIntervalOfNonCtrlChar.btw(c) && c != '\\' && c != '\'' && c != '\"') {
					dst[sz++] = c;
				} else {
					dst[sz++] = '\\';
					sz += writeEscChar(c, dst + sz);
				}
			}
			return sz;
		}

		auto & fromRaw(Byte const * const & src, Byte const & delimiter = kNullByte) { return fromRaw(reinterpret_cast<Ch const * const>(src), delimiter); }
		auto & toRaw(Byte * const & dst, Byte const & delimiter = kNullByte) const { return toRaw(reinterpret_cast<Ch * const>(dst), delimiter); }

		Str & printFmt(Ch const * const fmt, ...);
		Str & printFmtByAppend(Ch const * const fmt, ...);
		Str const & scanFmt(Ch const * const fmt, ...) const;
		Str const & scanFmt(SzI const & beg, Ch const * const fmt, ...) const;

		auto operator==(ListB const & t) const { return getListB().operator==(t); }
		auto operator!=(ListB const & t) const { return getListB().operator!=(t); }
		auto & operator=(ListB const & t) { getListB().operator=(t); return *this; }
		auto & operator+=(ListB const & t) { getListB().operator+=(t); return *this; }

		auto operator+(ListB const & t) const {
			Str r; r.alloc(_sz + t._sz).append(*this).append(t);
			return r;
		}

		auto operator>(ListB const & t) const { return f_strcmp(_str, t._item) > 0; }
		auto operator<(ListB const & t) const { return f_strcmp(_str, t._item) < 0; }

		auto operator==(Ch const * const & t) const { return cmp(t); }
		auto operator!=(Ch const * const & t) const { return !cmp(t); }
		auto operator>(Ch const * const & t) const { return f_strcmp(_str, t) > 0; }
		auto operator<(Ch const * const & t) const { return f_strcmp(_str, t) < 0; }
		auto & operator=(Ch const * const & t) { return cpy(t); }

		Str() : ListB(), _str(_item), _len(_sz) {}
		Str(Str const & t) : Str() { cpy(t); }
		explicit Str(SzI const & cap, Bool const & useCap = kFalse) : Str() { alloc(cap, useCap); }
		explicit Str(Ch const * const & src, SzI const & sz = kSzNull) : Str() { cpy(src, (sz == kSzNull) ? f_strlen(src) : sz); }
		explicit Str(ListB const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) : Str() { cpy(src, beg, sz); }

		auto & gtStr() { return *this; }
		auto & gtStr() const { return *this; }
	};

	extern Str const kNullStr;
	extern Str const kEmptyStr;

	struct StrRef : Str {

		auto & set(Ch const * const & str, SzI const & len = kSzNull) {
			_str = const_cast<Ch *>(reinterpret_cast<Ch const *>(str));
			_len = ternary_oper(len == kSzNull, f_strlen(str), len);
			_cap = kSzNull;
			return *this;
		}
		auto & set(Str const & str, SzI const & begin = kThFirst, SzI const & len = kSzNull) {
			_str = str._str + begin;
			_len = ternary_oper(len == kSzNull, str._len - begin, len);
			_cap = kSzNull;
			return *this;
		}
		auto & clear() {
			_str = kNullPtr;
			_len = kSzNull;
			_cap = kSzNull;
			return *this;
		}

		~StrRef() {
			_str = kNullPtr;
			return;
		}
		StrRef() { return; }
		explicit StrRef(Ch const * const & str, SzI const & len = kSzNull) {
			set(str, len);
			return;
		}
		explicit StrRef(Str const & str, SzI const & begin = kThFirst, SzI const & len = kSzNull) {
			set(str, begin, len);
			return;
		}

		auto & operator=(Str const & tgt) { return set(tgt); }
		auto & operator+=(SzI const & pos) {
			_str += pos;
			_len -= pos;
			return *this;
		}
		auto & operator-=(SzI const & pos) {
			_str -= pos;
			_len += pos;
			return *this;
		}
		auto & operator++() { return (*this) += kThSecond; }
		auto & operator--() { return (*this) -= kThSecond; }
	};
	tmpl_st(T) struct StrBlock {
		Str _str;

		auto & tfree() {
			_str.tfree();
			return *this;
		}
		auto & set(Str const & src) {
			_str = src;
			return *this;
		}
		auto getSz() const {
			return static_cast<SzI>(szOf<T>() + _str._len);
		}
		auto & read(Byte const * const data) {
			_str.cpy(reinterpret_cast<Ch const *>(data + szOf<T>()), *reinterpret_cast<T const *>(data));
			return *this;
		}
		auto & write(Byte * const data) const {
			memCpy(data + kThFirst, reinterpret_cast<T const *>(&_str._len), szOf<T>());
			memCpy(data + szOf<T>(), _str._str, static_cast<SzI>(_str._len));
			return *this;
		}
	};
	using StrBlock8 = StrBlock<UI8>;
	using StrBlock16 = StrBlock<UI16>;
	using StrBlock32 = StrBlock<UI32>;
	using StrBlock64 = StrBlock<UI64>;

	tmpl_st(T) constexpr auto isTStr() { return cmpT<T, Str>() || cmpT<T, StrRef>(); }

	SzI cnvByteSizeStr(Str const & str);

	inline auto calcSzFromHexStr(SzI const & len, Bool const & insSpace = kTrue) {
		return static_cast<SzI>(insSpace ? ((len + 1) / 3) : (len / 2));
	}
	Str & writeHexStr(Byte const * const & src, SzI const & sz, Str & dst, Bool const & insSpace = kTrue);
	Void readHexStr(Str const & src, Byte * const & dst, SzI & sz, Bool const & insSpace = kTrue);

	namespace $StrUtil {

		extern SzI const kUTF8ChLenTable[];

		inline auto isASCII(Ch const & ch) { return (ch & 0x80u) == 0; }

		inline auto getUTF8ChLen(Ch const & ch) { return kUTF8ChLenTable[static_cast<UCh>(ch)]; }

		inline auto readUTF8Ch(Ch const * const & src, SzI & pos) {
			auto ch = src[pos];
			auto len = getUTF8ChLen(ch);
			pos += len;
			return (len == kSzOne) ? ch : static_cast<Ch>(0xFF);
		}

		Void skipCmt(Ch const * const & src, SzI & pos);

		Str & readUTF8Str(Ch const * const & src, SzI & pos, Str & dst);
		Void writeUTF8Str(Str const & src, Ch * const & dst, SzI & pos);

		SzI putChN(Ch const & ch, SzI const & cnt, Ch * const & dst);

		enum class NumType : UI8 { kUInt, kSInt, kFloat };

		SzI numToStr(UI64 src, Ch * const & dst, SzI & pos);
		SzI numToStr(SI64 src, Ch * const & dst, SzI & pos, Bool const & outSgnAlways = kFalse);
		SzI numToStr(F64 src, Ch * const & dst, SzI & pos, Bool const & outSgnAlways = kFalse);
		NumType strToNum(Ch const * const & src, SzI & pos, Void * const & dst);
	}
}
