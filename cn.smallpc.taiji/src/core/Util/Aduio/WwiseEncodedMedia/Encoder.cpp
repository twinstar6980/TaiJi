#include "./Encoder.hpp"

#include "../WaveStruct.hpp"
#include "../../File/File.hpp"
#include "../../../Type/Map.hpp"
#include "../../../Type/BlockFunc.hpp"

namespace $TJ::$Util::$Audio::$WwiseEncodedMedia::$Encoder {

	enum class EncodeFmt : IU32 {
		kPCM = 0xFFFE,
		kADPCM = 0x0002,
		kAAC = 0xAAC0,
		kVorbis = 0xFFFF,
	};

#if defined PLATFORM_WINDOWS

	Str gFFMPEGFilePath;
	Str gWW2OGGFilePath;
	Str gWW2OGGPCBFilePath;

	static Str const kTmpEncodedAudioFilePath("C:\\ProgramData\\cn.smallpc\\taiji\\tmp\\audio-e.tmp"_r);
	static Str const kTmpAudioFilePath("C:\\ProgramData\\cn.smallpc\\taiji\\tmp\\audio.tmp"_r);
	static Str const kTmpAudio2FilePath("C:\\ProgramData\\cn.smallpc\\taiji\\tmp\\audio2.tmp"_r);

#endif

	namespace $DeCode {

	#if defined PLATFORM_WINDOWS
		static Void dec(ByteSection const & encoded, ByteSection & audio) {
			Map<FourCC, OSD> subChunkOSD;
			{
				ByteStream stm;
				stm.ref(encoded).setPosBeg();

				$WaveStruct::ChunkSign chunkSign;
				$StaticBlock::read(chunkSign, stm);
				if (chunkSign.$id != makeFourCC('R', 'I', 'F', 'F'))
					throw VAMGException("WwiseEncodedMedia-Decoder", "not RIFF file");
				if (stm.rd<FourCC>() != makeFourCC('W', 'A', 'V', 'E'))
					throw VAMGException("WwiseEncodedMedia-Decoder", "not WAVE file");

				subChunkOSD.alloc();
				while (!stm.eos()) {
					$StaticBlock::read(chunkSign, stm);
					subChunkOSD.appendByKV(chunkSign.$id, { stm.$pos, chunkSign.$sz });
					stm.posGo(chunkSign.$sz);
				}
				stm.unref();
			}
			{
				ByteSection fmtChunk;
				ByteSection dataChunk;
				{
					auto & chunkOSD = subChunkOSD[makeFourCC('f', 'm', 't', ' ')];
					fmtChunk.ref(encoded, chunkOSD.$ofs, chunkOSD.$sz);
				}
				{
					auto & chunkOSD = subChunkOSD[makeFourCC('d', 'a', 't', 'a')];
					dataChunk.ref(encoded, chunkOSD.$ofs, chunkOSD.$sz);
				}
				$WaveStruct::$RIFF_ChunkData::$WaveChunk::Fmt_ChunkData_Base fmtInfo;
				$StaticBlock::read(fmtInfo, fmtChunk);
				switch (static_cast<EncodeFmt>(fmtInfo.$audioFmt)) {
					default:
						throw VAMGException("WwiseEncodedMedia-Decoder", "unknown encode format : 0x%04X", fmtInfo.$audioFmt);
						break;
					case EncodeFmt::kPCM: {
						audio.cpy(encoded);
						auto chunkOfs = subChunkOSD[makeFourCC('f', 'm', 't', ' ')].$ofs;
						audio[chunkOfs + 0x00] = 0x01;
						audio[chunkOfs + 0x01] = 0x00;
						$File::writeFile(Path(kTmpAudioFilePath), audio);
						audio.free();
						Str cmd;
						cmd = gFFMPEGFilePath
							+ " "_r + "-loglevel warning"_r
							+ " "_r + "-y"_r
							+ " "_r + "-i "_r + kTmpAudioFilePath
							+ " "_r + "-f wav"_r
							+ " "_r + kTmpAudio2FilePath
							;
						std::system(cmd.$data);
						$File::readFileForce(Path(kTmpAudio2FilePath), audio);
						$File::remove(Path(kTmpAudioFilePath));
						$File::remove(Path(kTmpAudio2FilePath));
						break;
					}
					case EncodeFmt::kADPCM: {/*
						$File::writeFile(Path(kTmpAudioFilePath), encoded);
						Str cmd;
						cmd = gFFMPEGFilePath
							+ " "_r + "-loglevel warning"_r
							+ " "_r + "-y"_r
							+ " "_r + "-f adpcm"_r
							+ " "_r + "-i"_r
							+ " "_r + kTmpAudioFilePath
							+ " "_r + "-f wav"_r
							+ " "_r + kTmpAudio2FilePath
							;
						std::system(cmd.$data);
						$File::readFileForce(Path(kTmpAudio2FilePath), audio);
						break;*/
						log.war("can not convert adpcm");
						audio.alloc().setEmpty();
						break;
					}
					case EncodeFmt::kAAC: {
						$File::writeFile(Path(kTmpAudioFilePath), dataChunk);
						Str cmd;
						cmd = gFFMPEGFilePath
							+ " "_r + "-loglevel warning"_r
							+ " "_r + "-y"_r
							+ " "_r + "-i "_r + kTmpAudioFilePath
							+ " "_r + "-f wav"_r
							+ " "_r + kTmpAudio2FilePath
							;
						std::system(cmd.$data);
						$File::readFileForce(Path(kTmpAudio2FilePath), audio);
						$File::remove(Path(kTmpAudioFilePath));
						$File::remove(Path(kTmpAudio2FilePath));
						break;
					}
					case EncodeFmt::kVorbis: {
						$File::writeFile(Path(kTmpEncodedAudioFilePath), encoded);
						Str cmd;
						cmd = gWW2OGGFilePath
							+ " "_r + "--pcb "_r + gWW2OGGPCBFilePath
							+ " "_r + kTmpEncodedAudioFilePath
							+ " "_r + "-o "_r + kTmpAudioFilePath
							;
						std::system(cmd.$data);
						cmd = gFFMPEGFilePath
							+ " "_r + "-loglevel warning"_r
							+ " "_r + "-y"_r
							+ " "_r + "-i "_r + kTmpAudioFilePath
							+ " "_r + "-f wav"_r
							+ " "_r + kTmpAudio2FilePath
							;
						std::system(cmd.$data);
						$File::readFileForce(Path(kTmpAudio2FilePath), audio);
						$File::remove(Path(kTmpEncodedAudioFilePath));
						$File::remove(Path(kTmpAudioFilePath));
						$File::remove(Path(kTmpAudio2FilePath));
						break;
					}
				}
				fmtChunk.unref();
				dataChunk.unref();
			}
			return;
		}
	#endif

		Void decExport(Path const & encodedFile, Path const & audioFile) {
		#if defined PLATFORM_WINDOWS
			ByteSection encodedData, audioData;
			$File::readFileForce(encodedFile, encodedData);
			dec(encodedData, audioData);
			encodedData.free();
			$File::writeFile(audioFile, audioData);
			audioData.free();
		#endif
			return;
		}

	}

}
