# pragma once

# include "../Type.h"
# include "./Str.h"
# include "./StrList.h"

# include "../Const.h"
# include "./JSON.h"

namespace $TJ {

	inline auto getPathStr(Ch const * const & actualPathStr) {
	# if defined PLATFORM_WINDOWS
		return actualPathStr + $MaxStrLenDef::kDriveLetterStr;
	# elif defined PLATFORM_LINUX
		return actualPathStr;
	# else
		return kNullPtr;
	# endif
	}

	inline auto isPathDelimiter(Ch const & c) { return c == $PathDelimiter::kWindowsStyle || c == $PathDelimiter::kLinuxStyle; }

	extern Str const kEmptyPathStr;
	extern Str const kRootPathStr;
	/*
	auto & setNull() { getStrList().setNull(); return *this; }
	auto & alloc(SzI const & cap = kSzNone, SzI const & newEveryElement = kFalse) { getStrList().alloc(cap, newEveryElement); return *this; }
	auto & allocThenNew(SzI const & cap = kSzNone) { getStrList().allocThenNew(cap); return *this; }
	auto & tfree() { getStrList().tfree(); return *this; }
	auto & cpy(ListP const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) { getStrList().cpy(src, beg, sz); return *this; }
*/

	struct Path : StrList {

		auto & appendSz(SzI const & sz = kSzOne) { getListP().appendSz(sz); return *this; }
		auto & append(Str const & src, SzI const & sz = kSzOne) { getListP().append(src, sz); return *this; }
		auto & append(ListP const & src, SzI const & beg = kThFirst, SzI const & sz = kSzNull) { getListP().append(src, beg, sz); return *this; }
		auto & append(Ch const * const & src, SzI const & sz = kSzNull) { getStrList().append(StrRef(src, sz)); return *this; }

		/*
		"" -> {} null
		"a.txt" -> {"a.txt"} 1
		"dir/a.txt" -> {"dir", "a.txt"} 2
		"/" -> {""} under root 1
		"/a.txt" -> {"", "a.txt"} under root 2
		"/dir/a.txt" -> {"", "dir", "a.txt"} under root 3
		*/
		auto & parse(Ch const * const & str, SzI len = kSzNull) {
			if (len == kSzNull) len = f_strlen(str);
			tfree().alloc();
			if (len > kSzNone) {
				auto chPosBegin = kThFirst;
				if (isPathDelimiter(str[chPosBegin])) {
					appendSz().last().alloc();
					++chPosBegin;
				}
				auto subCnt = (chPosBegin == len) ? kSzNone : kSzOne;
				SzI chPos;
				for_cnt_ev(chPos, chPosBegin, len)
					if (isPathDelimiter(str[chPos]))
						++subCnt;
				expand(subCnt);
				auto chPosPre = chPosBegin;
				for (chPos = chPosBegin; chPos < len; ) {
					if (isPathDelimiter(str[chPos])) {
						append(str + chPosPre, chPos - chPosPre);
						chPosPre = chPos + kSzOne;
					}
					++chPos;
					if (chPos == len) {
						append(str + chPosPre, chPos - chPosPre);
						break;
					}
				}
			}
			return *this;
		}
		auto & parse(Str const & src) {
			precond(!src.isNull());
			return parse(src._str, src._len);
		}

		auto & deserialize($JSON::Arr const & src) {
			precond(!src.isNull());
			alloc(src._sz);
			for_criter(e, src)
				append(e.getStr());
			return *this;
		}
		auto & deserialize($JSON::Val const & src) { return src.isTypeArr() ? deserialize(src.getArr()) : (src.isTypeStr() ? parse(src.getStr()) : setNull()); }

		auto & serialize($JSON::Arr & dst) const {
			dst.alloc(_sz);
			for_criter(e, *this)
				dst.add(e);
			return dst;
		}

		auto & setPrefix(Str const & s) { last().prepend(s); return *this; }
		auto & setSuffix(Str const & s) { last().append($Char::kPeriod).append(s); return *this; }
		auto & setSuffixNoPeriod(Str const & s) { last().append(s); return *this; }

		auto & eraseSuffix() {
			SzI i;
			if (last().findFromEnd($Char::kPeriod, i))
				last().eraseAfter(i);
			return *this;
		}
		auto & getSuffix(Str & dst) const {
			SzI i;
			if (last().findFromEnd($Char::kPeriod, i))
				dst.cpy(last(), i + kThSecond);
			else dst.setNull();
			return dst;
		}
		auto cmpSuffix(Str const & suffix) const {
			SzI i;
			return last().findFromEnd($Char::kPeriod, i) && f_stricmp(last()._str + i + kThSecond, suffix._str) == 0;
		}
		auto cmpSuffixWithCase(Str const & suffix) const {
			SzI i;
			return last().findFromEnd($Char::kPeriod, i) && f_strcmp(last()._str + i + kThSecond, suffix._str) == 0;
		}

		auto & setPrefix(Ch const * const & s) { return setPrefix(StrRef(s)); }
		auto & setSuffix(Ch const * const & s) { return setSuffix(StrRef(s)); }
		auto & setSuffixNoPeriod(Ch const * const & s) { return setSuffixNoPeriod(StrRef(s)); }
		auto cmpSuffix(Ch const * const & s) { return cmpSuffix(StrRef(s)); }
		auto cmpSuffixWithCase(Ch const * const & s) { return cmpSuffixWithCase(StrRef(s)); }

		auto & getFileName() { return last(); }
		auto & getFileName() const { return last(); }
		auto & getFileName(Str & t) const { return t = last(); }

		auto & getDirPath(Str & str, Ch const & delimiter = $PathDelimiter::kNativeStyle) const {
			str.alloc(getStrLenRaw(kThFirst, _sz - kSzOne) - kSzOne);
			for_cnt(i, kThFirst, lastIdx()) {
				str.append(get(i));
				if (i < lastIdx() - kThSecond)
					str.append(delimiter);
			}
			return str;
		}
		auto & getFullPath(Str & str, Ch const & delimiter = $PathDelimiter::kNativeStyle) const {
			str.alloc(getStrLenRaw(kThFirst, _sz - kSzOne) - kSzOne);
			for_cnt(i, kThFirst, lastIdx()) {
				str.append(get(i));
				if (i < lastIdx())
					str.append(delimiter);
			}
			return str;
		}
		auto getFullPath(Ch const & delimiter = $PathDelimiter::kNativeStyle) const {
			Str str;
			str.alloc(getStrLenRaw(kThFirst, _sz - kSzOne) - kSzOne);
			for_cnt(i, kThFirst, lastIdx()) {
				str.append(get(i));
				if (i < lastIdx())
					str.append(delimiter);
			}
			return Str(str);
		}

		auto & operator=(ListP const & t) { StrList::cpy(t); return *this; }
		auto & operator+=(ListP const & t) { StrList::append(t); return *this; }

		auto operator+(ListP const & t) const {
			Path r; r.alloc(_sz + t._sz).append(*this).append(t);
			return r;
		}

		auto & operator=(Ch const * const & t) { return parse(t); }

		Path() : StrList() {}
		Path(Path const & t) : Path() { cpy(t); }
		explicit Path(SzI const & cap, Bool const & useCap = kFalse) : Path() { alloc(cap, useCap); }
		explicit Path(Str const * const & src, SzI const & sz = kSzOne) : Path() { cpy(src, sz); }
		explicit Path(ListP const & src, SzI const & beg = kThFirst, SzI sz = kSzNull) : Path() { cpy(src, beg, sz); }
		explicit Path(Str const & src) : Path() { parse(src); }
		explicit Path(Ch const * const str, SzI const & len = kSzNull) : Path() { parse(str, len); }
		explicit Path($JSON::Arr const & src) : Path() { deserialize(src); }
		explicit Path($JSON::Val const & src) : Path() { deserialize(src); }

		auto & gtPath() { return *this; }
		auto & gtPath() const { return *this; }
	};
	extern Path const kEmptyPath;
	extern Path const kStorageDir;

	Bool pathIsEqual(Path const & pathA, Path const & pathB);

	ListP<Path> & pathFilterByCommon(ListP<Path> const & srcA, ListP<Path> const & srcB, ListP<Path> & dst);
	ListP<Path> & pathFilterByType(ListP<Path> const & src, ListP<Path> & dst, Bool const & isDir);
	ListP<Path> & pathFilterBySuffix(ListP<Path> const & src, ListP<Path> & dst, Str const & suffix);
	ListP<Path> & pathFilterByStr(ListP<Path> const & src, ListP<Path> & dst, Str const & str);
}
