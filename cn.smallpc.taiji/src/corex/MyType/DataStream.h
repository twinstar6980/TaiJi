#pragma once

# include "../Const.h"
# include "./Path.h"

namespace $TJ {
	struct DataStm {
		Byte * _data;
		SzI _cap, _sz, _pos;


		~DataStm() { return; }
		DataStm() : _data(kNullPtr), _cap(kSzZero), _sz(kSzNull), _pos(kSzNull) { return; }
		explicit DataStm(Path const & path) {

			return;
		}

		auto tfree() {
			return;
		}
		auto & alloc(SzI const & cap) {
			tfree();
			_data = new Byte[cap];
			_cap = cap;
			_sz = kThNull;
			_pos = kThNull;
			return *this;
		}
		auto & ref(Byte const * const & data, SzI const & sz) {
			_data = const_cast<Byte *>(data);
			_cap = kThNull;
			_sz = sz;
			_pos = kThNull;
			return *this;
		}
		auto & bind(Byte const * const & data, SzI const & sz) {
			return *this;
		}

		tmpl_st(T) auto const & get() const {
		# if defined ABI_ARM
			ifcexp(isTFolat<T>()) {
				T t; memCpy(&fv, src + pos, szOf<F32>());
			} else {
				//auto & p = *reinterpret_cast<T *>(_data + _pos);
			}
		# else
		# endif
			auto & p = *reinterpret_cast<T *>(_data + _pos);
			_pos += szOf<T>();
			return p;
		}
		tmpl_st(T) auto & put(T const & val) {
			*reinterpret_cast<T *>(_data + _pos) = val;
			_pos += szOf<T>();
			return *this;
		}

		auto & setPos(SzI const & pos) {
			_pos = pos;
			return *this;
		}
		auto & goPos(SzI const & pos) {
			_pos += pos;
			return *this;
		}
		auto & backPos(SzI const & pos) {
			_pos -= pos;
			return *this;
		}

	};

	struct DataStream {
		Byte * data;
		SzI pos, cap;

		~DataStream() { return; }
		DataStream() : data(kNullPtr), pos(kSzZero), cap(kSzNull) { return; }

		auto & alloc(SzI const & aCap) {
			// TODO
			return *this;
		}

		auto & setRef(Byte const * const & data_, SzI const & cap_) {
			data = (Byte *)data_;
			cap = cap_;
			pos = kThFirst;
			return *this;
		}

		tmpl_st(T) auto out() {
			auto curPos = pos; pos += szOf<T>();
			return *reinterpret_cast<T *>(data + curPos);
		}
		tmpl_st(T) auto & out(T & dst) {
			dst = *reinterpret_cast<T *>(data + pos);
			pos += szOf<T>();
			return *this;
		}
	};
}
