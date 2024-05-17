#pragma once

# include "../Const.h"
# include "./Str.h"

# include <bitset>

namespace $TJ {
	template <UI32 Size>
	struct _BitSet {
		std::bitset<Size> _data;

		Bool & operator[](SzI const & Idx) {
			return _data[Idx];
		}
		Bool const & operator[](SzI const & Idx) const {
			return _data[Idx];
		}

		Bool GetBit(SzI const & Idx) const {
			return _data.test(Idx);
		}
		_BitSet & SetBit(SzI const & Idx, Bool const & State) {
			_data.set(Idx);
			return this;
		}

		UI32 Get32() const {
			return static_cast<UI32>(_data.to_ulong());
		}
		UI32 Get64() const {
			return static_cast<UI32>(_data.to_ullong());
		}
		Str getStr() const {
			return Str(_data.to_string().c_str());
		}
		_BitSet & Set32(UI32 const & Data) {
			std::bitset<Size> NewData(Data);
			_data = NewData;
			return *this;
		}
		_BitSet & Set64(UI64 const & Data) {
			std::bitset<Size> NewData(Data);
			_data = NewData;
			return *this;
		}
		_BitSet & setStr(Ch const * const & Data) {
			std::bitset<Size> NewData(Data);
			_data = NewData;
			return *this;
		}

		constexpr _BitSet(Ch const * const & Data) : _data(Data) {
			return;
		}
	};
}
