# include "./JSONPatch.h"
# include "../UtilKit/JSONUtil.h"

namespace $TJ::$JSON::$Patch {

	ListP<JSONPath> & clearRepeat(ListP<JSONPath> & t) {
		ListP<JSONPath> list;
		for_criter(e, t) {
			if (!list.exist(e))
				list.add(e);
		}
		t = list;
		return t;
	}

	Val & apply(Patch const & patch, Val & data) {
		ListP<JSONPath> jpl, jpl2;
		jpl.add();
		patch._selector.select(data, jpl, jpl2);
		for_criter(e, jpl2) {
			for_criter(ee, e._path) {
				if (ee.isTypeStr()) {
					wLog(">%s", ee.getStr()._str);
				} else {
					wLog(">%u", ee.getUInt());
				}
			}
			wLogLn("");
		}
		patch._modifyer.apply(data, jpl2);
		return data;
	}
	Val & apply(ListP<Patch> const & patch, Val & data) {
		for_criter(e, patch)
			apply(e, data);
		return data;
	}
	Void apply(Path const & patchFile, Path const & srcFile, Path const & dstFile) {
		Val tmp;
		read(srcFile, tmp);
		apply(ListOfInfo<Patch>().read(read(patchFile, Arr().setNull())), tmp);
		write(dstFile, eraseUndef(tmp));
	}

}
