# include "./WEMEncoder.h"

# include "../WaveStruct.h"

#if defined PLATFORM_WINDOWS
#include "../../../libx/wem2ogg/_mylinker.h"
#endif

namespace $TJ::$AudioUtil::$WEMEncoder {
	using namespace $AudioUtil::$WaveStruct;
	using namespace $AudioUtil::$WaveStruct::$RIFF_ChunkData::$WaveChunk;
	Void WEMDeCodeToOGG(Path const & srcFilePath, Path const & dstFilePath) {
	# if defined PLATFORM_WINDOWS
		fwem2ogg(srcFilePath.getFullPath(Str().getThis())._str, dstFilePath.getFullPath(Str().getThis())._str);
	# endif
		return;
	}
	Void RWave(Byte const * const & src, SzI const & srcSz) {
		SzI pos;
		pos = kSzZero;
		$ChunkSign::Member FmtSign;
		$ChunkSign::Member DataSign;
		$Fmt_ChunkData_Base::Member FmtInfo;
		$DataBlockUtil::read(src + 0xC, FmtSign);
		$DataBlockUtil::read(src + 0xC + 0x8, FmtInfo);
		$DataBlockUtil::read(src + 0xC + 0x8 + FmtSign._sz, DataSign);

		Byte * stream(kNullPtr);
		newPtr(stream, DataSign._sz);
		memCpy(stream, src + 0xC + 0x8 + FmtSign._sz + 0x8, DataSign._sz);
		wLogLn("%u", (unsigned)stream[0]);

		return;
	}
	Void RWave(Path const & path) {
		Byte * data(kNullPtr); SzI sz;
		$DevUtil::ForceReadData(path, data, sz);
		RWave(data, sz);
		delPtrArr(data);
		return;
	}
}
