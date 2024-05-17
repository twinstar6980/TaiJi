# pragma once

# include "../Type.h"
# include "../MyType/StrList.h"
# include "../MyType/Path.h"
# include "../MyType/JSON.h"

namespace $TJ::$JSON {
	tmpl_st(T) SzI cntStrSum(T const & src) {
		auto strSum = kSzZero;
		ifcexp(isTVal<T>()) {
			switch (src._type) {
				default: break;
				case ValType::kObj: strSum += cntStrSum(src.getObj()); break;
				case ValType::kArr: strSum += cntStrSum(src.getArr()); break;
				case ValType::kStr: ++strSum; break;
			}
		} elifcexp(isTObj<T>()) {
			for_criter(e, src) {
				if (e._key.isTypeStr()) ++strSum;
				strSum += cntStrSum(e._val);
			}
		} elifcexp(isTArr<T>()) {
			for_criter(e, src)
				strSum += cntStrSum(e);
		}
		return strSum;
	}

	tmpl_st(T) Void getAllStr(T const & src, StrList & dst) {
		ifcexp(isTVal<T>()) {
			switch (src._type) {
				default: break;
				case ValType::kObj: getAllStr(src.getObj(), dst); break;
				case ValType::kArr: getAllStr(src.getArr(), dst); break;
				case ValType::kStr: if (!dst.exist(src.getStr())) dst.add(src.getStr()); break;
			}
		} elifcexp(isTObj<T>()) {
			for_criter(e, src) {
				if (e._key.isOK() && !dst.exist(e._key.get()))
					dst.add(e._key.get());
				getAllStr(e._val, dst);
			}
		} elifcexp(isTArr<T>()) {
			for_criter(e, src)
				getAllStr(e, dst);
		}
		return;
	}
	tmpl_st(T) auto getAllStr(T const & src, Arr & dst, SzI strSum = kSzNull) {
		if (strSum == kSzNull) strSum = cntStrSum(src);
		StrList strList(strSum);
		getAllStr(src, strList);
		dst.alloc(strList._sz);
		for_criter(e, strList)
			dst.add(e);
		strList.tfree();
		return;
	}

	tmpl_st(T) inline auto & getArrForBaseType(Arr const & src, ListB<T> & dst) {
		dst.alloc(src._sz);
		for_criter(e, src)
			e.getX(dst.addNull().last());
		return dst;
	}
	tmpl_st(T) inline auto & setArrForBaseType(ListB<T> const & src, Arr & dst) {
		dst.alloc(src._sz);
		for_criter(e, src)
			dst.add(e);
		return dst;
	}

	inline auto & getArrStr(Arr const & src, StrList & dst) {
		dst.alloc(src._sz);
		for_criter(e, src)
			dst.add(e.getStr());
		return dst;
	}
	inline auto & setArrStr(StrList const & src, Arr & dst) {
		dst.alloc(src._sz);
		for_criter(e, src)
			dst.add(e);
		return dst;
	}

	namespace $IOBuf {
		constexpr SzI kDfltBufSz(kSzBinMega * 64);

		Void freeBuf();
		Void modBufSz(SzI const & sz);
	}

	$JSON::Val & read(Str const & src, $JSON::Val & dst);
	$JSON::Obj & read(Str const & src, $JSON::Obj & dst);
	$JSON::Arr & read(Str const & src, $JSON::Arr & dst);

	Str & write(Str & dst, $JSON::Val const & src);
	Str & write(Str & dst, $JSON::Obj const & src);
	Str & write(Str & dst, $JSON::Arr const & src);

	$JSON::Val & read(Path const & src, $JSON::Val & dst);
	$JSON::Obj & read(Path const & src, $JSON::Obj & dst);
	$JSON::Arr & read(Path const & src, $JSON::Arr & dst);

	Void write(Path const & dst, $JSON::Val const & src);
	Void write(Path const & dst, $JSON::Obj const & src);
	Void write(Path const & dst, $JSON::Arr const & src);

	tmpl_st(T) auto write(T const & src) { // show on cmd
		wLogLn("%s", write(Str().setNull(), src)._str);
		return;
	}
	inline auto fmtFile(Path const & srcFile, Path const & dstFile) { return write(dstFile, read(srcFile, Val().set())); }
}
