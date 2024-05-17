#include "./Shell.hpp"

#include "../Entry/Entry.hpp"
#include "../Type/JSON.hpp"
#include "../Util/File/File.hpp"
#include "../Util/CharSet/CharSet.hpp"
#include "../UserLiteral.hpp"
#include "../PlatformSupport.hpp"

#include "../Util/Aduio/WwiseEncodedMedia/Encoder.hpp"

#if defined PLATFORM_ANDROID

#include <jni.h>

::JNIEnv * gJNIEnv;

#endif

namespace $TJ::$Shell {

	char const * getVerName() {
		auto s = $Entry::getVerName();
		auto r = s.$data;
		s.unbind();
		return r;
	}

	void runEntry(unsigned const argCnt, char const * const * const argVal, char const * const logDir
			  #if defined PLATFORM_ANDROID
				  , char const * const storageDir
			  #endif
	) {
		List<Str> arg(argCnt);
		Range(argVal, argVal + argCnt).each(
			[&arg](auto const & v) {
				cpyCStr(arg.appendX().last(), v);
			}
		);
		$Entry::runEntry(arg, refCStr(crv(Str()), logDir)
					 #if defined PLATFORM_ANDROID
						 , cpyCStr(crv(Str()), storageDir)
					 #endif
		);
		return;
	}

#if defined PLATFORM_WINDOWS

	void runEntryBySpecialForWindows(unsigned const argCnt, char const * const * const argVal) {

		static Path const kEnvFile("C:\\ProgramData\\cn.smallpc\\taiji\\env.json"_r);

		// FROM ANSI STRING
		List<Str> srcArg(argCnt);
		Range(argVal, argVal + argCnt).each(
			[&srcArg](auto const & v) {
				auto tmp = $Util::$CharSet::cnvANSIToUTF8(v);
				srcArg.appendX().last().bind(tmp, getCStrLen(tmp));
				tmp = kNullPtr;
			}
		);
		$JSON::Obj env;
		{
			$JSON::$IOMem::realloc($JSON::$IOMem::kDfltBufSz);
			if (!$Util::$File::existFile(kEnvFile)) {
				log.err("can not found env file");
			}
			$JSON::read(kEnvFile, env);
			$JSON::$IOMem::free();
		}
		{
			auto const & envExtern = env["extern"_r].getObj();
			$Util::$Audio::$WwiseEncodedMedia::$Encoder::gFFMPEGFilePath = envExtern["ffmpeg"_r]["exe"_r].getStr();
			$Util::$Audio::$WwiseEncodedMedia::$Encoder::gWW2OGGFilePath = envExtern["ww2ogg"_r]["exe"_r].getStr();
			$Util::$Audio::$WwiseEncodedMedia::$Encoder::gWW2OGGPCBFilePath = envExtern["ww2ogg"_r]["pcb"_r].getStr();
		}
		auto const envLogDir = env["logDir"_r].getStr();
		auto const envCfg = env["cfg"_r].getStr();
		auto const envDfltArg = $JSON::getListFromArr(env["dfltArg"_r], crv(List<Str>()));
		env.free();
		List<Str> arg;
		if (srcArg.isEmpty()) { // default
			arg.alloc(kSz1 + envDfltArg.$sz).append(envCfg).append(envDfltArg);
		} else {
			if (srcArg.first().isEmpty()) { // normal
				arg.cpy(List<Str>().ref(srcArg, kNo2));
			} else if (srcArg.first().$sz > kSz3) { // quick enter
				arg.alloc(kSz1 + srcArg.$sz).append(envCfg);
				for (auto const & e : srcArg)
					arg.appendX().last() = "<"_r + e;
			} else {
				arg.setNull();
				log.err("error cmd");
			}
		}
		$Entry::runEntry(arg, envLogDir);
		//wCmt("finish proc");
		return;
	}

#elif defined PLATFORM_ANDROID

	char const * getABIName() {
		auto s = $PlatformSupport::$Android::getABIName();
		auto r = s.$data;
		s.unbind();
		return r;
	}

	void runEntryBySpecialForAndroid(::JNIEnv & env, ::jobjectArray const & argArr) {
		gJNIEnv = &env;

		Sz argCnt = env.GetArrayLength(argArr);

		Str storageDir;
		Str logDir;
		List<Str> arg;

		storageDir = $PlatformSupport::$Android::jstr2u8s(env, (::jstring)(env.GetObjectArrayElement(argArr, kNo1)));
		logDir = $PlatformSupport::$Android::jstr2u8s(env, (::jstring)(env.GetObjectArrayElement(argArr, kNo2)));
		arg.allocThenUse(argCnt - kSz2);
		for (auto i = kNo1; i + kSz2 < argCnt; ++i) {
			arg[i] = $PlatformSupport::$Android::jstr2u8s(env, (::jstring)(env.GetObjectArrayElement(argArr, kNo3 + i)));
		}

		$Entry::runEntry(arg, logDir, storageDir);

		gJNIEnv = nullptr;
		return;
	}

#endif

}
