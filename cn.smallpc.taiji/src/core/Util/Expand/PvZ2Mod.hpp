#pragma once

#include "../../Type.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/InfoType.hpp"
#include "../../Type/Map.hpp"
#include "../../Type/JSON.hpp"
#include "../../UserLiteral.hpp"

namespace $TJ::$Util::$Expand::$PvZ2Mod {

	struct PatchInfo {

		Bool $enable;
		Str $name, $type;
		$JSON::Arr $target;

		auto & read($JSON::Obj const & t) {
			t["enable"_r].get($enable);
			t["name"_r].get($name);
			t["type"_r].get($type);
			$target = t["target"_r].getArr();
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

	using Mod = InfoList<PatchInfo>;

	struct RSBBuildCfg {

		Path $file, $dir;
		Path $oriInfoFile;

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["file"_r], $file);
			cnvJSONArrToPath(t["dir"_r], $dir);
			cnvJSONArrToPath(t["oriInfoFile"_r], $oriInfoFile);
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

	struct EnvCfg {

		InfoList<RSBBuildCfg> $rsb;
		Map<Str, Path> $path;
		Sz $memSzWhenRSBPack, $memSzWhenRTONEnCode;
		Bool $enableStrIdxWhenRTONEnCode;

		auto & read($JSON::Obj const & t) {
			$rsb.read(t["rsb"_r]);
			$path.alloc(t["path"_r].getObj().$sz);
			for (auto const & e : t["path"_r].getObj())
				cnvJSONArrToPath(e.$v, $path.appendX().last().setKey(e.$k.getStr()).$v);
			t["memSzWhenRSBPack"_r].get($memSzWhenRSBPack);
			t["memSzWhenRTONEnCode"_r].get($memSzWhenRTONEnCode);
			t["enableStrIdxWhenRTONEnCode"_r].get($enableStrIdxWhenRTONEnCode);
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

	extern Void introExport(Path const & envFile);
	extern Void outroExport(Path const & envFile);
	extern Void applyExport(Path const & envFile, Path const & modDir);

}
