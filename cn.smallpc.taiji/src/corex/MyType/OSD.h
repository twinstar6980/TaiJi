#	pragma once

#	include "../Type.h"
#	include "./PairVal.h"

namespace $TJ {
	struct OSD : PairVal<SzI> {
		SzI & _ofs, & _sz;

		auto getBeg() const { return _ofs; }
		auto getEnd() const { return static_cast<SzI>(_ofs + _sz); }

		OSD() : PairVal(), _ofs(_left), _sz(_right) {}
		constexpr explicit OSD(SzI const & ofs, SzI const & sz) : PairVal(ofs, sz), _ofs(_left), _sz(_right) {}
	};
}
