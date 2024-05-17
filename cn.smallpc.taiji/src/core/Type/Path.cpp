#include "./Path.hpp"
#include "../Util/File/File.hpp"

namespace $TJ {

	namespace $Path {

		Bool equal(Path const & a, Path const & b) {
			if (a.$sz == b.$sz) {
				auto iterB = b.begin();
				return a.range().eachUntil(
					[&iterB](auto & v) {
						return !$Str::cmpIgnoreCase(v, *(iterB++));
					}
				) == kNoX;
			}
			return kBoF;
		}

		List<Path> & filterByCommon(List<Path> const & srcA, List<Path> const & srcB, List<Path> & dst) {
			dst.alloc(srcA.$sz);
			for (auto const & e : srcA)
				if (srcB.range().exist(e))
					dst.append(e);
			return dst;
		}
		List<Path> & filterByType(List<Path> const & src, List<Path> & dst, FileType const & ty) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				if ($Util::$File::getType(e) == ty)
					dst.append(e);
			return dst;
		}
		List<Path> & filterByExtension(List<Path> const & src, List<Path> & dst, Str const & extension) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				if (e.cmpExtensionIgnoreCase(extension))
					dst.append(e);
			return dst;
		}
		List<Path> & filterBySubStr(List<Path> const & src, List<Path> & dst, Str const & str) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				if (e.last().range().existSub(str.range()))
					dst.append(e);
			return dst;
		}

		List<Path> & select(Selector const & selector, List<Path> const & ori, List<Path> & result) {
			result.alloc(ori.$sz);
			for (auto const & e : ori) {
				if (e.$sz == selector.$sz) {
					auto ok = kBoT;
					for (auto i = kNo1; ok && i < selector.$sz; ++i)
						ok &= $Str::selectorMatch(selector[i], e[i], kBoT);
					if (ok)
						result.append(e);
				}
			}
			return result;
		}
		List<Path> & select(Selector const & selector, Path const & parentDir, List<Path> & result) {
			List<Path> list;
			$Util::$File::getPathListOfFile(parentDir, list, kBoF);
			return select(selector, list, result);
		}

	}

}
