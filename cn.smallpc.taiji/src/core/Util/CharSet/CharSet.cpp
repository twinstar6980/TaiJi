#include "./CharSet.hpp"

#include "../../Type.hpp"
#include "../../Const.hpp"
#include "../../Ptr.hpp"

#if defined PLATFORM_WINDOWS

#include <windows.h>

#endif

namespace $TJ::$Util::$CharSet {

#if defined PLATFORM_WINDOWS

	auto chw2ch(ChW const * const & src, UINT const dstCP) {
		Ch * r;
		auto len = WideCharToMultiByte(dstCP, 0, src, -1, kNullPtr, 0, kNullPtr, kNullPtr);
		if (len <= decltype(len)(0)) {
			r = kNullPtr;
		} else {
			pAlloc(r, len + kSz1);
			WideCharToMultiByte(dstCP, 0, src, -1, r, len, kNullPtr, kNullPtr);
			r[len - kNo2] = $Ch::kNull;
		}
		return r;
	}
	auto ch2chw(Ch const * const & src, UINT const srcCP) {
		ChW * r;
		auto len = MultiByteToWideChar(srcCP, 0, src, -1, kNullPtr, 0);
		if (len <= decltype(len)(0)) {
			r = kNullPtr;
		} else {
			pAlloc(r, len + kSz1);
			MultiByteToWideChar(srcCP, 0, src, -1, r, len);
			r[len - kNo2] = $Ch::kNull;
		}
		return r;
	}

	Ch * cnvUTF16ToUTF8(ChW const * const & src) {
		return chw2ch(src, CP_UTF8);
	}
	ChW * cnvUTF8ToUTF16(Ch const * const & src) {
		return ch2chw(src, CP_UTF8);
	}

	auto cnvSet(Ch const * const & src, UINT const srcCP, UINT const dstCP) {
		auto tmp = ch2chw(src, srcCP);
		auto dst = chw2ch(tmp, dstCP);
		pFreeN(tmp);
		return dst;
	}

	Ch * cnvUTF8ToANSI(Ch const * const & src) {
		return cnvSet(src, CP_UTF8, CP_ACP);
	}
	Ch * cnvANSIToUTF8(Ch const * const & src) {
		return cnvSet(src, CP_ACP, CP_UTF8);
	}

#endif

}
