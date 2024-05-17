#pragma once

#include "../../Type.hpp"
#include "../../Const.hpp"
#include "../../TypeUtil.hpp"

namespace $TJ::$Util::$Audio::$WaveStruct {

	struct ChunkSign {

		static inline constexpr Sz kSz = 8;
		static inline constexpr Sz kSum = 2;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 4), // id
			OSD(0x4, 4), // sz
		};

		FourCC $id;
		IU32 $sz;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $id,
				& $sz,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	namespace $ChunkSignCode {

		inline constexpr FourCC kRIFF(makeFourCC('R', 'I', 'F', 'F'));
		inline constexpr FourCC kFmt(makeFourCC('f', 'm', 't', ' '));
		inline constexpr FourCC kFact(makeFourCC('f', 'a', 'c', 't'));
		inline constexpr FourCC kData(makeFourCC('d', 'a', 't', 'a'));

	}

	namespace $RIFF_ChunkData {

		struct Base {

			static inline constexpr Sz kSz = 4;
			static inline constexpr Sz kSum = 1;

			static inline constexpr OSD kInfo[]{
				OSD(0x0, 4), // fmt
			};

			FourCC $fmt;

			auto addr() const {
				return new Void const * const [kSum] {\
					& $fmt,
				};
			}
			auto addr() {
				return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
			}

		};

		namespace $FmtCode {

			inline constexpr FourCC kWAVE(makeFourCC('W', 'A', 'V', 'E'));

		}

		namespace $WaveChunk {

			struct Fmt_ChunkData_Base {

				static inline constexpr Sz kSz = 16;
				static inline constexpr Sz kSum = 6;

				static inline constexpr OSD kInfo[]{
					OSD(0x0, 2), // audioFmt
					OSD(0x2, 2), // numChannels
					OSD(0x4, 4), // sampleRate
					OSD(0x8, 4), // byteRate
					OSD(0xC, 2), // blockAlign
					OSD(0xE, 2), // bitsPerSample
				};

				IU16 $audioFmt;
				IU16 $numChannels;
				IU32 $sampleRate;
				IU32 $byteRate;
				IU16 $blockAlign;
				IU16 $bitsPerSample;

				auto addr() const {
					return new Void const * const [kSum] {\
						& $audioFmt,
						& $numChannels,
						& $sampleRate,
						& $byteRate,
						& $blockAlign,
						& $bitsPerSample,
					};
				}
				auto addr() {
					return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
				}

			};

			namespace $Fmt_ChunkData_ExpandBlock {
				// TODO
			}

			struct Fact_ChunkData {

				static inline constexpr Sz kSz = 4;
				static inline constexpr Sz kSum = 1;

				static inline constexpr OSD kInfo[]{
					OSD(0x0, 4), // sampleTotal
				};

				IU32 $sampleTotal;

				auto addr() const {
					return new Void const * const [kSum] {\
						& $sampleTotal,
					};
				}
				auto addr() {
					return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
				}

			};

			namespace $AudioFmtCode {

				inline constexpr IU16 kPCM(0x0001);
				inline constexpr IU16 kIEEE_FLOAT(0x0003);
				inline constexpr IU16 kALAW(0x0006);
				inline constexpr IU16 kMULAW(0x0007);
				inline constexpr IU16 kEXTENSIBLE(0xFFFE);
				inline constexpr IU16 kUNKNOWN(0xFFFF);

			}

		}

	}

}
