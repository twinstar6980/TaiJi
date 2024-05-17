#include "./JSONPatch.hpp"

namespace $TJ::$Util::$Expand::$JSONPatch {

	List<$JSON::ValPath> & clearRepeat(List<$JSON::ValPath> & t) {
		List<$JSON::ValPath> r;
		r.alloc(t.$sz);
		for (auto const & e : t) {
			if (!r.range().exist(e))
				r.append(e);
		}
		t = r;
		return t;
	}

	static auto printPath($JSON::ValPath const & t) {
		for (auto const & e : t.$path) {
			if (e.isStr()) {
				log.txt(">%s", e.getStr().$data);
			} else {
				log.txt(">%u", e.getNum());
			}
		}
		log.txtln("");
		return;
	}
	static auto printPath(List<$JSON::ValPath> const & t) {
		for (auto const & e : t) {
			printPath(e);
		}
		return;
	}

	namespace $Apply {

		extern $JSON::Val & apply(Patch const & patch, $JSON::ValPath const & initPath, $JSON::Val & data) {
			List<$JSON::ValPath> oriPath, tgtPath;
			oriPath.alloc().append(initPath);
			if (patch.$path.$state) {
				tgtPath.alloc(patch.$path.$data.$sz);
				for (auto const & e : patch.$path.$data) {
					auto & t = tgtPath.appendX().last();
					t.$path = initPath.$path + e.$path;
				}
			} else if (patch.$selector.$state) {
				patch.$selector.$data.select(data, oriPath, tgtPath);
			} else {
				tgtPath = oriPath;
			}
			printPath(tgtPath);
			if (patch.$modifyer.$state) {
				patch.$modifyer.$data.apply(data, tgtPath);
			}
			apply(patch.$inherit, tgtPath, data);
			return data;
		}
		extern $JSON::Val & apply(Patch const & patch, List<$JSON::ValPath> const & initPath, $JSON::Val & data) {
			if (!initPath.isNull()) {
				for (auto const & e : initPath)
					apply(patch, e, data);
			}
			return data;
		}
		extern $JSON::Val & apply(List<Patch> const & patch, List<$JSON::ValPath> const & initPath, $JSON::Val & data) {
			if (!patch.isNull()) {
				for (auto const & e : patch)
					apply(e, initPath, data);
			}
			return data;
		}
		extern $JSON::Val & apply(List<Patch> const & patch, $JSON::ValPath const & initPath, $JSON::Val & data) {
			if (!patch.isNull()) {
				for (auto const & e : patch)
					apply(e, initPath, data);
			}
			return data;
		}
		extern $JSON::Val & apply(List<Patch> const & patch, $JSON::Val & data) {
			$JSON::ValPath initPath;
			initPath.$path.alloc().setEmpty();
			for (auto const & e : patch)
				apply(e, initPath, data);
			return data;
		}

		extern Void applyExport(Path const & patchFile, Path const & srcFile, Path const & dstFile) {
			$JSON::Val tmp;
			$JSON::read(srcFile, tmp);
			apply(InfoList<Patch>().read($JSON::read(patchFile, crv($JSON::Val()))), tmp);
			$JSON::write(dstFile, $JSON::eraseUndef(tmp, crv($JSON::Val())));
			return;
		}

	}

	namespace $Diff {

		static auto & makeSimplePatch(Patch & result, $JSON::ValPath const & path, Modifyer const & modifyer) {
			result.$path.$state = kBoT;
			result.$path.$data.alloc(kSz1).append(path);
			result.$selector.$state = kBoF;
			result.$modifyer.$state = kBoT;
			result.$modifyer.$data = modifyer;
			result.$inherit.setNull();
			return result;
		}

		Void diff($JSON::Val const & src, $JSON::Val const & dst, $JSON::ValPath const & path, List<Patch> & patch) {
			if (src == dst)
				return;
			if (src.$$ty() == dst.$$ty()) {
				if (src.isObj()) {
					for (auto const & e : src.getObj()) {
						auto nextPath = path;
						nextPath.assign(e.$k.getStr());
						printPath(nextPath);
						if (dst.getObj().existKey(e.$k)) {
							diff(e.$v, dst[e.$k.getStr()], nextPath, patch);
						} else {
							auto & t = patch.appendX().last();
							makeSimplePatch(t, nextPath, { "remove"_s });
						}
					}
					for (auto const & e : dst.getObj()) {
						auto nextPath = path;
						nextPath.assign(e.$k.getStr());
						printPath(nextPath);
						if (!src.getObj().existKey(e.$k)) {
							$JSON::Val tmp;
							auto & t = patch.appendX().last();
							tmp.setObj().getObj().add(e.$k.getStr(), e.$v);
							makeSimplePatch(t, nextPath, { "append-member"_s, tmp });
						}
					}
				} else if (src.isArr()) {
					for (auto i = kNo1; i < src.getArr().$sz; ++i) {
						if (Sz j; !dst.getArr().range().find(src[i], j)) {
							auto & t = patch.appendX().last();
							makeSimplePatch(t, path, { "remove"_s });
						}
					}
					for (auto i = kNo1; i < dst.getArr().$sz; ++i) {
						if (Sz j; !src.getArr().range().find(dst[i], j)) {
							auto & t = patch.appendX().last();
							makeSimplePatch(t, path, { "append-element"_s, dst[i] });
						}
					}
				} else {
					auto & t = patch.appendX().last();
					makeSimplePatch(t, path, { "replace"_s, dst });
				}
			} else {
				auto & t = patch.appendX().last();
				makeSimplePatch(t, path, { "replace"_s, dst });
			}
			return;
		}

		extern Void diffExport(Path const & srcFile, Path const & dstFile, Path const & patchFile) {
			$JSON::Val src, dst;
			$JSON::read(srcFile, src);
			$JSON::read(dstFile, dst);
			InfoList<Patch> patch;
			$JSON::ValPath vp;
			vp.$path.alloc();
			patch.alloc();
			diff(src, dst, vp, patch);
			$JSON::write(patchFile, patch.write(crv($JSON::Val())));
			return;
		}

	}

}
