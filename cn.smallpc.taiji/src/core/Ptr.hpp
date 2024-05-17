#pragma once

#include "./Type.hpp"
#include "./Const.hpp"

namespace $TJ::$Ptr {

	template <typename T>
	inline auto & pAlloc(T *& p) {
		p = new T();
		return p;
	}
	template <typename T>
	inline auto & pAlloc(T *& p, Sz const & n) {
		p = new T[n]();
		return p;
	}
	template <typename T>
	inline auto & pAlloc(T **& p, Sz const & n1, Sz const & n2) {
		p = new T * [n1]();
		for (auto i = kNo1; i < n1; ++i)
			p[i] = new T[n2]();
		return p;
	}

	template <typename T>
	inline auto & pFreeS(T *& p) {
		if (p != kNullPtr) {
			delete p;
			p = kNullPtr;
		}
		return p;
	}
	template <typename T>
	inline auto & pFreeN(T *& p) {
		if (p != kNullPtr) {
			delete[] p;
			p = kNullPtr;
		}
		return p;
	}
	template <typename T>
	inline auto & pFree(T *& p, Sz const & n) {
		if (p != kNullPtr) {
			for (auto i = kNo1; i < n; ++i)
				pFreeN(p[i]);
			delete[] p;
			p = kNullPtr;
		}
		return p;
	}
	template <typename T>
	inline auto & pvFreeS(Void *& p) {
		if (p != kNullPtr) {
			delete static_cast<T *>(p);
			p = kNullPtr;
		}
		return p;
	}

	// todo
	template <typename T>
	class Ptr {

	protected:

		T * $ptr;

	public:

		// getter
		auto const ptr() const {
			return $ptr;
		}

		// setter
		auto & ptr(T const * ptr) {
			$ptr = const_cast<T *>(ptr);
			return thiz;
		}

		auto & alloc(Sz const & n) {
			pAlloc($ptr, n);
			return thiz;
		}
		auto & free() {
			return thiz;
		}

		auto & operator=(T const * ptr) {
			thiz.ptr(ptr);
			return thiz;
		}
		auto & operator*() {
			return *$ptr;
		}

		Ptr() {}

		~Ptr() {
			free();
		}

	};

}

namespace $TJ {
	using namespace $Ptr;
}
