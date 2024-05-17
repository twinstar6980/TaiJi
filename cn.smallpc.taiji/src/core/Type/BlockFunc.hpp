#pragma once

#include "../Type.hpp"
#include "../Const.hpp"
#include "./List.hpp"
#include "./ByteStream.hpp"

#include <memory.h>

namespace $TJ {

	namespace $StaticBlock {

	#if defined ENABLE_CONCEPTS

		template<typename T>
		concept CTStaticBlock = requires(T t, T const ct) {
			T::kSum;
			T::kSz;
			T::kInfo;
			t.addr();
			ct.addr();
		};

	#else

	#define CTStaticBlock typename

	#endif

		template <CTStaticBlock T>
		auto & clear(T & t) {
			auto addr = t.addr();
			for (auto i = kNo1; i < T::kSum; ++i)
				std::memset(addr[i], kByteNull, T::kInfo[i].$sz);
			delete[] addr;
			return t;
		}
		template <CTStaticBlock T>
		auto & clear(List<T> & t) {
			for (auto & e : t)
				clear(e);
			return t;
		}

		template <CTStaticBlock T>
		auto & read(T & t, ByteSection const & data) {
			auto addr = t.addr();
			for (auto i = kNo1; i < T::kSum; ++i)
				std::memcpy(addr[i], data.$data + T::kInfo[i].$ofs, T::kInfo[i].$sz);
			delete[] addr;
			return t;
		}
		template <CTStaticBlock T>
		auto & read(List<T> & t, ByteSection const & data, Sz const & blockSz = T::kSz) {
			auto pos = kNo1;
			for (auto & e : t) {
				read(e, ByteSection().ref(data, pos, T::kSz));
				pos += blockSz;
			}
			return t;
		}
		template <CTStaticBlock T>
		auto & read(T & t, ByteStream & data, Sz const & blockSz = T::kSz) {
			auto addr = t.addr();
			for (auto i = kNo1; i < T::kSum; ++i)
				std::memcpy(addr[i], data.curData() + T::kInfo[i].$ofs, T::kInfo[i].$sz);
			data.posGo<Byte>(blockSz);
			delete[] addr;
			return t;
		}
		template <CTStaticBlock T>
		auto & read(List<T> & t, ByteStream & data, Sz const & blockSz = T::kSz) {
			for (auto & e : t)
				read(e, data, blockSz);
			return t;
		}

		template <CTStaticBlock T>
		auto & write(T const & t, ByteSection & data) {
			auto addr = t.addr();
			for (auto i = kNo1; i < T::kSum; ++i)
				std::memcpy(data.$data + T::kInfo[i].$ofs, addr[i], T::kInfo[i].$sz);
			delete[] addr;
			return t;
		}
		template <CTStaticBlock T>
		auto & write(List<T> const & t, ByteSection & data, Sz const & blockSz = T::kSz) {
			auto pos = kNo1;
			for (auto & e : t) {
				write(e, ByteSection().ref(data, pos, T::kSz));
				pos += blockSz;
			}
			return t;
		}
		template <CTStaticBlock T>
		auto & write(T const & t, ByteStream & data, Sz const & blockSz = T::kSz) {
			auto addr = t.addr();
			for (auto i = kNo1; i < T::kSum; ++i)
				std::memcpy(data.curData() + T::kInfo[i].$ofs, addr[i], T::kInfo[i].$sz);
			data.posGo<Byte>(blockSz);
			delete[] addr;
			return t;
		}
		template <CTStaticBlock T>
		auto & write(List<T> const & t, ByteStream & data, Sz const & blockSz = T::kSz) {
			for (auto & e : t)
				write(e, data, blockSz);
			return t;
		}

	}

	namespace $DynamicBlock {

	#if defined ENABLE_CONCEPTS

		template<typename T>
		concept CTDynamicBlock = requires(T t, T const ct) {
			requires(true);
		};

	#else

	#define CTDynamicBlock typename

	#endif

		template <CTDynamicBlock T>
		auto & read(List<T> & t, ByteSection const & data) {
			auto pos = kNo1;
			for (auto & e : t) {
				e.read(ByteSection().ref(data, pos));
				pos += e.sz();
			}
			return t;
		}
		template <CTDynamicBlock T>
		auto & write(List<T> const & t, ByteSection & data) {
			auto pos = kNo1;
			for (auto & e : t) {
				e.write(ByteSection().ref(data, pos));
				pos += e.sz();
			}
			return t;
		}

		template <CTDynamicBlock T>
		auto & read(List<T> & t, ByteStream & data) {
			for (auto & e : t)
				e.read(data);
			return t;
		}
		template <CTDynamicBlock T>
		auto & write(List<T> const & t, ByteStream & data) {
			for (auto & e : t)
				e.write(data);
			return t;
		}

	}

}
