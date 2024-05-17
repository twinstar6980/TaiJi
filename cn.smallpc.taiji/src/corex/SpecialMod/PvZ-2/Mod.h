#pragma once

# include "../../MyType/Str.h"
# include "../../UtilKit/JSONUtil.h"
# include "../../MyType/ListP.h"
# include "../../MyType/ListOfOther.h"

# include "../../RTONUtil/Encoder.h"
# include "../../UtilKit/JSONPatch.h"
# include "./RSBPatch.h"
# include "../SpecialMod_Export.h"

namespace $TJ::$SpecialMod::$PvZ_2::$Mod {

	struct PatchInfo {
		Bool _enable;
		Str _name, _type;
		$JSON::Arr _target;

		auto & read($JSON::Obj const & src) {
			src["Enable"].getX(_enable);
			src["Name"].getX(_name);
			src["Type"].getX(_type);
			_target = src["Target"].getArr();
			return *this;
		}

	};

	using Mod = ListOfInfo<PatchInfo>;

	struct RSBBuildCfg {
		Path _rsbFile, _rsbDir;
		Str _resInfoGroup, _resInfoSubGroup;
		Path _resInfoPath;

		auto & read($JSON::Obj const & src) {
			_rsbFile.read(src["RSBFile"].getArr());
			_rsbDir.read(src["RSBDir"].getArr());
			src["ResInfo"][kThFirst].getX(_resInfoGroup);
			src["ResInfo"][kThSecond].getX(_resInfoSubGroup);
			_resInfoPath.read(src["ResInfo"][kThThird].getArr());
			return *this;
		}
	};

	struct EnvCfg {
		ListOfInfo<RSBBuildCfg> _rsbBuild;
		Map<Str, Path> _path;
		SzI _memSzWhenRSBPack, _memSzWhenRTONEnCode;
		Bool _useCacheStrWhenRTONEnCode;

		auto & read($JSON::Obj const & src) {
			_rsbBuild.read(src["RSBBuild"].getArr());
			for_criter(e, src["Path"].getObj()) {
				_path.addByKey(e._key.get()).getLast()._val.read(e._val.getArr());
			}
			src["MemSzWhenRSBPack"].getX(_memSzWhenRSBPack);
			src["MemSzWhenRTONEnCode"].getX(_memSzWhenRTONEnCode);
			src["UseCacheStrWhenRTONEnCode"].getX(_useCacheStrWhenRTONEnCode);
			return *this;
		}
	};

	Void expApply(Path const & modDir, Path const & envFile);

	Void expIntro(Path const & envFile);
	Void expOutro(Path const & envFile);
}
