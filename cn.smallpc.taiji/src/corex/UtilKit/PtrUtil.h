#	pragma once

#	include "../Type.h"
#	include "../Const.h"

namespace $TJ::$PtrUtil {
	tmpl_st(T) inline auto & newPtr(T *& dst) {
		dst = new T;
		return dst;
	}
	tmpl_st(T) inline auto & newPtr(T const *& dst) {
		dst = new T;
		return dst;
	}

	tmpl_st(T) inline auto & newPtr(T *& dst, SzI const & sz) {
		dst = new T[sz];
		return dst;
	}
	tmpl_st(T) inline auto & newPtr(T const *& dst, SzI const & sz) {
		dst = new T[sz];
		return dst;
	}

	tmpl_st(T) inline auto & newPtrLv2ArrArr(T **& dst, SzI const & szLv1, SzI const & szLv2) {
		dst = new T * [szLv1];
		for_cnt(i, kThFirst, szLv1)
			dst[i] = new T[szLv2];
		return dst;
	}
	tmpl_st(T) inline auto & newPtrLv2ArrArr(T const *& dst, SzI const & szLv1, SzI const & szLv2) {
		dst = new T * [szLv1];
		for_cnt(i, kThFirst, szLv1)
			dst[i] = new T[szLv2];
		return dst;
	}
	tmpl_st(T) inline auto & newPtrLv2ArrArr(T * const & dst, SzI const & szLv1, SzI const & szLv2) {
		dst = new T * [szLv1];
		for_cnt(i, kThFirst, szLv1)
			dst[i] = new T[szLv2];
		return dst;
	}
	tmpl_st(T) inline auto & newPtrLv2ArrArr(T const * const & dst, SzI const & szLv1, SzI const & szLv2) {
		dst = new T * [szLv1];
		for_cnt(i, kThFirst, szLv1)
			dst[i] = new T[szLv2];
		return dst;
	}

	tmpl_st(T) inline auto & delPtrSingle(T *& dst) {
		delete dst;
		dst = kNullPtr;
		return dst;
	}
	tmpl_st(T) inline auto & delPtrSingle(T const *& dst) {
		delete dst;
		dst = kNullPtr;
		return dst;
	}

	tmpl_st(T) inline auto & delPtrArr(T *& dst) {
		delete[] dst;
		dst = kNullPtr;
		return dst;
	}
	tmpl_st(T) inline auto & delPtrArr(T const *& dst) {
		delete[] dst;
		dst = kNullPtr;
		return dst;
	}

	tmpl_st(T) inline auto & safeDelPtrSingle(T *& dst) {
		if (dst) {
			delete dst;
			dst = kNullPtr;
		}
		return dst;
	}
	tmpl_st(T) inline auto & safeDelPtrSingle(T const *& dst) {
		if (dst) {
			delete dst;
			dst = kNullPtr;
		}
		return dst;
	}

	tmpl_st(T) inline auto & safeDelPtrArr(T *& dst) {
		if (dst) {
			delete[] dst;
			dst = kNullPtr;
		}
		return dst;
	}
	tmpl_st(T) inline auto & safeDelPtrArr(T const *& dst) {
		if (dst) {
			delete[] dst;
			dst = kNullPtr;
		}
		return dst;
	}

	tmpl_st(T) inline auto & delPtrArrArr(T **& dst, SzI const & szLv1) {
		for_cnt(i, kThFirst, szLv1) {
			delete[] dst[i];
			dst[i] = kNullPtr;
		}
		delete[] dst;
		dst = kNullPtr;
		return dst;
	}
	tmpl_st(T) inline auto & delPtrArrArr(T const **& dst, SzI const & szLv1) {
		for_cnt(i, kThFirst, szLv1) {
			delete[] dst[i];
			dst[i] = kNullPtr;
		}
		delete[] dst;
		dst = kNullPtr;
		return dst;
	}

	tmpl_st(T) inline auto & delVoidPtrSingle(Void *& dst) {
		delete static_cast<T *>(dst);
		dst = kNullPtr;
		return dst;
	}
	tmpl_st(T) inline auto & delVoidPtrSingle(Void const *& dst) {
		delete static_cast<T const *>(dst);
		dst = kNullPtr;
		return dst;
	}

	tmpl_st(T) inline auto & safeDelVoidPtrSingle(Void *& dst) {
		if (dst) {
			delete static_cast<T *>(dst);
			dst = kNullPtr;
		}
		return dst;
	}
	tmpl_st(T) inline auto & safeDelVoidPtrSingle(Void const *& dst) {
		if (dst) {
			delete static_cast<T const *>(dst);
			dst = kNullPtr;
		}
		return dst;
	}
}
namespace $TJ {
	using namespace $PtrUtil;
}
