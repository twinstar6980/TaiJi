
# include "./Path.h"
# include "../UtilKit/DevUtil.h"

namespace $TJ {
	Bool pathIsEqual(Path const & pathA, Path const & pathB) {
		auto result = pathA._sz == pathB._sz;
		if (result) {
			for_cnt(idx, kThFirst, pathA._sz)
				if (f_stricmp(pathA[idx]._str, pathB[idx]._str) != 0) {
					result = kFalse;
					break;
				}
		}
		return result;
	}

	ListP<Path> & pathFilterByCommon(ListP<Path> const & srcA, ListP<Path> const & srcB, ListP<Path> & dst) {
		for_criter(e, srcA)
			if (srcB.exist(e))
				dst.add(e);
		return dst;
	}
	ListP<Path> & pathFilterByType(ListP<Path> const & src, ListP<Path> & dst, Bool const & isDir) {
		dst.setNull();
		for_criter(e, src)
			if ($DevUtil::isDir(e) == isDir)
				dst.add(e);
		return dst;
	}
	ListP<Path> & pathFilterBySuffix(ListP<Path> const & src, ListP<Path> & dst, Str const & suffix) {
		dst.setNull();
		for_criter(e, src)
			if (e.cmpSuffix(suffix))
				dst.add(e);
		return dst;
	}
	ListP<Path> & pathFilterByStr(ListP<Path> const & src, ListP<Path> & dst, Str const & str) {
		dst.setNull();
		for_criter(e, src)
			if (e.last().existSub(str))
				dst.add(e);
		return dst;
	}
}
