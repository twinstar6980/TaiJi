#pragma once

#include "../Type.hpp"
#include "../Const.hpp"
#include "../TypeUtil.hpp"
#include "./List.hpp"

#include <memory.h>

namespace $TJ {

	/*

		byte-section

			cap & sz :
				if is ref , cap == null
				else		cap >= sz

			when file read|write , use property 'sz'

		byte-stream

			pos :
				initial value is null(-1)
				should startPos before read|write stream

	*/

	template <typename T>
	using Section = List<T>;

	using ByteSection = Section<Byte>;

	struct ByteStream : ByteSection {

	private:

		using TT = ByteStream;
		using TS = ByteSection;
		using TE = Byte;

	public:

		Sz $pos;

	public:

		auto & free() {
			TS::free();
			$pos = kNoX;
			return thiz;
		}

		auto & alloc(Sz const & cap = kSzN) {
			TS::alloc(cap);
			$pos = kNo1;
			return thiz;
		}
		auto & allocThenUse(Sz const & cap = kSzN, Sz const & sz = kSzX) {
			TS::allocThenUse(cap, sz);
			$pos = kNo1;
			return thiz;
		}

		auto & unbind() {
			TS::unbind();
			$pos = kNoX;
			return thiz;
		}
		auto & bind(TS const & t) {
			TS::bind(t);
			$pos = kNo1;
			return thiz;
		}
		auto & bind(TE const * const & data, Sz const & sz, Sz const & cap = kSzX) {
			TS::bind(data, sz, cap);
			$pos = kNo1;
			return thiz;
		}
		auto & unref() {
			TS::unref();
			$pos = kNoX;
			return thiz;
		}
		auto & ref(TS const & t, Sz const & beg = kNo1, Sz sz = kSzX, Sz cap = kSzX) {
			TS::ref(t, beg, sz, cap);
			$pos = kNo1;
			return thiz;
		}
		auto & ref(TE const * const & data, Sz const & sz, Sz cap = kSzX) {
			TS::ref(data, sz, cap);
			$pos = kNo1;
			return thiz;
		}

		auto & cpy(TT const & t) {
			TS::cpy(t);
			$pos = t.$pos;
			return thiz;
		}

		auto curData() {
			return $data + $pos;
		}

		auto eos() {
			return $pos == $sz;
		}

		auto & contractSzUntilPos() {
			assert(!isNull());
			$sz = $pos;
			return thiz;
		}

		auto idleSz() {
			return $sz - $pos;
		}

		auto & setPosBeg(Sz const & pos = kNo1) {
			assert(!isNull());
			$pos = pos;
			return thiz;
		}
		auto & setPosEnd(Sz const & pos = kNo1) {
			assert(!isNull());
			$pos = $sz - kNo2 - pos;
			return thiz;
		}

		template <typename T = Byte>
		auto & posGo(Sz const & sz = kSz1) {
			assert(!isNull());
			$pos += szOf<T>(sz);
			return thiz;
		}
		template <typename T = Byte>
		auto & posBack(Sz const & sz = kSz1) {
			assert(!isNull());
			$pos -= szOf<T>(sz);
			return thiz;
		}

		template <typename T>
		auto & rdNoPos() {
			assert(!isNull());
			return *reinterpret_cast<T *>($data + $pos);
		}
		template <typename T>
		auto & rd() {
			assert(!isNull());
			auto pos = $pos;
			$pos += kTSz<T>;
			assert($pos <= $sz);
			return *reinterpret_cast<T *>($data + pos);
		}

		template <typename T>
		auto & rd(T & t) {
			assert(!isNull());
			assert($pos + kTSz<T> <= $sz);
			std::memcpy(&t, $data + $pos, kTSz<T>);
			$pos += kTSz<T>;
			return thiz;
		}
		template <typename T>
		auto & rd(List<T> & t) {
			assert(!isNull());
			for (auto & e : t)
				rd(e);
			return thiz;
		}
		template <typename T>
		auto & wd(T const & t) {
			assert(!isNull());
			assert($pos + kTSz<T> <= $sz);
			std::memcpy($data + $pos, &t, kTSz<T>);
			$pos += kTSz<T>;
			return thiz;
		}
		template <typename T>
		auto & wd(List<T> const & t) {
			assert(!isNull());
			assert($pos + kTSz<T> * t.$sz <= $sz);
			for (auto const & e : t)
				wd(e);
			return thiz;
		}

	public:

		template <typename T>
		auto & operator>>(T & t) {
			rd(t);
			return thiz;
		}
		template <typename T>
		auto & operator<<(T const & t) {
			wd(t);
			return thiz;
		}

	public:

		implicit ByteStream() :
			ByteSection(), $pos(kNoX) {
		}
		explicit ByteStream(Sz const & cap) :
			ByteStream() {
			alloc(cap);
		}

	};

}
