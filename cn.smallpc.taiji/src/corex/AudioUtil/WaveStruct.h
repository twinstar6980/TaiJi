# pragma once

# include "../Type.h"
# include "../Const.h"
# include "../MyType/OSD.h"

namespace $TJ::$AudioUtil::$WaveStruct {
	namespace $ChunkSignCode {
		constexpr FourCC kRIFF(makeFourCC('R', 'I', 'F', 'F'));
		constexpr FourCC kFmt(makeFourCC('f', 'm', 't', ' '));
		constexpr FourCC kFact(makeFourCC('f', 'a', 'c', 't'));
		constexpr FourCC kData(makeFourCC('d', 'a', 't', 'a'));
	}
	namespace $ChunkSign {
		constexpr SzI kBlockSize(8);
		constexpr SzI kMemberSum(2);
		namespace $MemberOSD {
			constexpr OSD kID(0x0, 4);
			constexpr OSD kSize(0x4, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			FourCC _id;
			UI32 _sz;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_id,
						& _sz,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_id,
						& _sz,
				};
			}
		};
	}
	namespace $RIFF_ChunkData {
		namespace $Base {
			constexpr SzI kBlockSize(4);
			constexpr SzI kMemberSum(1);
			namespace $MemberOSD {
				constexpr OSD kFmt(0x0, 4);
			}
			extern OSD const * const kMemberOSDPtr[kMemberSum];
			struct Member {
				FourCC _fmt;

				constexpr auto getSz() const { return kBlockSize; }
				constexpr auto getSum() const { return kMemberSum; }

				OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

				auto getPtr() {
					return new Void * const [kMemberSum] {
						&_fmt,
					};
				}
				auto getPtr() const {
					return new Void const * const [kMemberSum] {
						&_fmt,
					};
				}
			};
		}
		namespace $FmtCode {
			constexpr FourCC kWAV(makeFourCC('W', 'A', 'V', 'E'));
		}
		namespace $WaveChunk {
			namespace $Fmt_ChunkData_Base {
				constexpr SzI kBlockSize(16);
				constexpr SzI kMemberSum(6);
				namespace $MemberOSD {
					constexpr OSD kAudioFmt(0x0, 2);
					constexpr OSD kNumChannels(0x2, 2);
					constexpr OSD kSampleRate(0x4, 4);
					constexpr OSD kByteRate(0x8, 4);
					constexpr OSD kBlockAlign(0xC, 2);
					constexpr OSD kBitsPerSample(0xE, 2);
				}
				extern OSD const * const kMemberOSDPtr[kMemberSum];
				struct Member {
					UI16 _audioFmt;
					UI16 _numChannels;
					UI32 _sampleRate;
					UI32 _byteRate;
					UI16 _blockAlign;
					UI16 _bitsPerSample;

					constexpr auto getSz() const { return kBlockSize; }
					constexpr auto getSum() const { return kMemberSum; }

					OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

					auto getPtr() {
						return new Void * const [kMemberSum] {
							&_audioFmt,
								& _numChannels,
								& _sampleRate,
								& _byteRate,
								& _blockAlign,
								& _bitsPerSample,
						};
					}
					auto getPtr() const {
						return new Void const * const [kMemberSum] {
							&_audioFmt,
								& _numChannels,
								& _sampleRate,
								& _byteRate,
								& _blockAlign,
								& _bitsPerSample,
						};
					}
				};
				namespace $Fmt_ChunkData_ExpandBlock {
					// TODO
				}
			}
			namespace $Fact_ChunkData {
				constexpr SzI kBlockSize(4);
				constexpr SzI kMemberSum(1);
				namespace $MemberOSD {
					constexpr OSD kSampleTotal(0x0, 4);
				}
				extern OSD const * const kMemberOSDPtr[kMemberSum];
				struct Member {
					UI32 _sampleTotal;

					constexpr auto getSz() const { return kBlockSize; }
					constexpr auto getSum() const { return kMemberSum; }

					OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

					auto getPtr() {
						return new Void * const [kMemberSum] {
							&_sampleTotal,
						};
					}
					auto getPtr() const {
						return new Void const * const [kMemberSum] {
							&_sampleTotal,
						};
					}
				};
			}
			namespace $AudioFmtCode {
				constexpr UI16 kWAVE_FORMAT_PCM(0x0001);
				constexpr UI16 kWAVE_FORMAT_IEEE_FLOAT(0x0003);
				constexpr UI16 kWAVE_FORMAT_ALAW(0x0006);
				constexpr UI16 kWAVE_FORMAT_MULAW(0x0007);
				constexpr UI16 kWAVE_FORMAT_EXTENSIBLE(0xFFFE);
				constexpr UI16 kUnKnown(0xFFFF);
			}
		}
	}
}
