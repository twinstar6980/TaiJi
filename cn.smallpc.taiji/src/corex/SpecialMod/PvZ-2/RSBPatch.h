#pragma once

# include "../../MyType/Str.h"
# include "../../UtilKit/JSONUtil.h"
# include "../../MyType/ListP.h"

# include "../../InfoUtil/ResPkgInfo.h"
# include "../../ResPkgUtil/RSBUtil/StructInfo.h"
# include "../../ImageUtil/ImageInfoType.h"

namespace $TJ::$SpecialMod::$PvZ_2::$RSBPatch {
	namespace $PatchInfo {
		using Category = $ResPkgUtil::$RSBUtil::Category;

		tmpl_st(T) struct TypeShell {
			Bool _exist;
			T _val;

			TypeShell() : _exist(kFalse), _val() { return; }
		};

		enum class Tactic : UI8 { kUnKnown, kOverWrite, kPassIfExist, kBaseOn, kRemove };

		inline auto getTactic(Str const & str) {
			if (str == "BaseOn")
				return Tactic::kBaseOn;
			elif(str == "PassIfExist")
				return Tactic::kPassIfExist;
			elif(str == "OverWrite")
				return Tactic::kOverWrite;
			elif(str == "Remove")
				return Tactic::kRemove;
			else
				return Tactic::kOverWrite;
		}
		inline auto getTactic($JSON::Obj const & parent) {
			return (parent.exist("Tactic") && parent["Tactic"].isTypeStr()) ? getTactic(parent["Tactic"].getStr()) : Tactic::kOverWrite;
		}

		using ResType = $InfoUtil::$ResPkgInfo::ResType;
		using ImageRes = $InfoUtil::$ResPkgInfo::ImageRes;
		struct ResBase {
			Tactic _tactic;
			TypeShell<Path> _path;
			TypeShell<Str> _type;

			auto & tfree() {
				_tactic = Tactic::kUnKnown;
				_path._val.tfree();
				_type._val.tfree();
				return *this;
			}
			auto & cpy(ResBase const & src) {
				_tactic = src._tactic;
				_path = src._path;
				_type = src._type;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				_tactic = getTactic(src);
				if (_path._exist = src.exist("Path"))
					_path._val.read(src["Path"].getArr());
				if (_type._exist = src.exist("Type"))
					src["Type"].getX(_type._val);
				return *this;
			}
			protected:
			auto & getBase() { return *this; }
			auto & getBase() const { return *this; }
		};
		struct AtlasInfo {
			TypeShell<SzI> _texFmt;
			TypeShell<$ImageUtil::ImageSize> _sz;
			TypeShell<MapOfInfo<ImageRes>> _img;

			auto & cpy(AtlasInfo const & src) {
				_texFmt = src._texFmt;
				_sz = src._sz;
				_img = src._img;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				if (_texFmt._exist = src.exist("TexFmt"))
					src["TexFmt"].getX(_texFmt._val);
				if (_sz._exist = src.exist("Size"))
					_sz._val.read(src["Size"].getArr());
				if (_img._exist = src.exist("Image"))
					_img._val.read(src["Image"].getObj());
				return *this;
			}
		};
		struct Res : ResBase {
			ResType _resType;
			Void * _append;

			auto & getThis() { return *this; }

			auto isType(ResType const & type) const { return _resType == type; }
			auto isNormalType() const { return _resType == ResType::kNormal; }
			auto isAtlasType() const { return _resType == ResType::kAtlas; }

			auto & setType(ResType const & type) {
				_resType = type;
				if (type == ResType::kNormal)
					_append = kNullPtr;
				elif(type == ResType::kAtlas)
					_append = new AtlasInfo();
				return *this;
			}
			auto & setNormalType() { return setType(ResType::kNormal); }
			auto & setAtlasType() { return setType(ResType::kAtlas); }

			auto & tfree() {
				getBase().tfree();
				if (_resType == ResType::kAtlas) safeDelVoidPtrSingle<AtlasInfo>(_append);
				return *this;
			}
			auto & getAtlasInfo() { return *static_cast<AtlasInfo *>(_append); }
			auto & getAtlasInfo() const { return *static_cast<AtlasInfo const *>(_append); }

			auto & cpy(Res const & src) {
				getBase().cpy(src);
				_resType = src._resType;
				if (_resType == ResType::kAtlas) {
					_append = new AtlasInfo();
					getAtlasInfo().cpy(src.getAtlasInfo());
				}
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				getBase().read(src);
				if (!src.exist("AtlasInfo"))
					setNormalType();
				else
					setAtlasType().getAtlasInfo().read(src["AtlasInfo"].getObj());
				return *this;
			}

			~Res() {
				if (_resType == ResType::kAtlas) safeDelVoidPtrSingle<AtlasInfo>(_append);
				return;
			}
		};
		struct SubGroup {
			Tactic _tactic;
			TypeShell<Category> _category;
			TypeShell<MapOfInfo<Res>> _res;

			auto & read($JSON::Obj const & src) {
				_tactic = getTactic(src);
				if (_category._exist = src.exist("Category"))
					_category._val.read(src["Category"].getArr());
				if (_res._exist = src.exist("Res"))
					_res._val.read(src["Res"].getObj());
				return *this;
			}
		};
		struct Group {
			Tactic _tactic;
			TypeShell<Bool> _isComposite;
			TypeShell<MapOfInfo<SubGroup>> _subGroup;

			auto & read($JSON::Obj const & src) {
				_tactic = getTactic(src);
				if (_isComposite._exist = src.exist("IsComposite"))
					src["IsComposite"].getX(_isComposite._val);
				if (_subGroup._exist = src.exist("SubGroup"))
					_subGroup._val.read(src["SubGroup"].getObj());
				return *this;
			}
		};
		struct PatchInfo {
			TypeShell<MapOfInfo<Group>> _group;

			auto & read($JSON::Obj const & src) {
				if (_group._exist = src.exist("Group"))
					_group._val.read(src["Group"].getObj());
				return *this;
			}
		};
	}

	namespace $PatchResInfo {
		Void patch($PatchInfo::PatchInfo const & patch, $InfoUtil::$ResPkgInfo::ResPkgInfo & info);
	}
	namespace $PatchStructInfo {
		Void patch($PatchInfo::PatchInfo const & patch, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & info);
	}
	namespace $PatchRes {
		Void patch($PatchInfo::PatchInfo const & patchInfo, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & info, Path const & srcResDir, Path const & dstGroupDir);
	}
}
