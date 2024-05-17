# pragma once

# include "../MyType/OSD.h"
# include "../MyType/ListOfOther.h"

# include "../Const.h"
# include "./PtrUtil.h"
# include "./MemoryUtil.h"

namespace $TJ::$DataBlockUtil {
	tmpl_st(T) auto & clear(T & member) {
		auto mp = member.getPtr();
		auto mosp = member.getOSDPtr();
		for_cnt(i, kThFirst, member.getSum())
			memSet(mp[i], (*mosp[i])._sz);
		delPtrArr(mp);
		return member;
	}
	tmpl_st(T) auto & clearArr(ListOfBlock<T> & member) {
		for_cnt(i, kThFirst, member._sz)
			clear(member[i]);
		return member;
	}

	tmpl_st(T) auto & read(Byte const * const data, T & member) {
		auto mp = member.getPtr();
		auto mosp = member.getOSDPtr();
		for_cnt(i, kThFirst, member.getSum())
			memCpy(mp[i], data + (*mosp[i])._ofs, (*mosp[i])._sz);
		delPtrArr(mp);
		return member;
	}
	tmpl_st(T) auto & readArr(Byte const * const data, ListOfBlock<T> & member) {
		for_cnt(i, kThFirst, member._sz)
			read(data + member.getSz(kThFirst, i), member[i]);
		return member;
	}
	tmpl_st(T) auto & readArr(Byte const * const data, ListOfBlock<T> & member, SzI const & blockSz) {
		for_cnt(i, kThFirst, member._sz)
			read(data + (blockSz * i), member[i]);
		return member;
	}

	tmpl_st(T) auto & write(Byte * const data, T const & member) {
		auto mp = member.getPtr();
		auto mosp = member.getOSDPtr();
		memSet(data, member.getSz());
		for_cnt(i, kThFirst, member.getSum())
			memCpy(data + (*mosp[i])._ofs, mp[i], (*mosp[i])._sz);
		delPtrArr(mp);
		return member;
	}
	tmpl_st(T) auto & writeArr(Byte * const data, ListOfBlock<T> const & member) {
		for_cnt(i, kThFirst, member._sz)
			write(data + member.getSz(kThFirst, i), member[i]);
		return member;
	}
	tmpl_st(T) auto & writeArr(Byte * const data, ListOfBlock<T> const & member, SzI const & blockSz) {
		for_cnt(i, kThFirst, member._sz)
			write(data + (blockSz * i), member[i]);
		return member;
	}

	tmpl_st(T) auto & writeNoClearBlock(Byte * const data, T const & member) {
		auto mp = member.getPtr();
		auto mosp = member.getOSDPtr();
		for_cnt(i, kThFirst, member.getSum())
			memCpy(data + (*mosp[i])._ofs, mp[i], (*mosp[i])._sz);
		delPtrArr(mp);
		return member;
	}
	tmpl_st(T) auto & writeArrNoClearBlock(Byte * const data, ListOfBlock<T> const & member) {
		for_cnt(i, kThFirst, member._sz)
			write(data + member.getSz(kThFirst, i), member[i]);
		return member;
	}
	tmpl_st(T) auto & writeArrNoClearBlock(Byte * const data, ListOfBlock<T> const & member, SzI const & blockSz) {
		for_cnt(i, kThFirst, member._sz)
			write(data + (blockSz * i), member[i]);
		return member;
	}
}
