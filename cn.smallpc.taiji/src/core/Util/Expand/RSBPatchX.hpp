#pragma once

#include "../../Type.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/InfoType.hpp"
#include "../../Type/Map.hpp"
#include "../../Type/JSON.hpp"
#include "../../UserLiteral.hpp"
#include "../Image/InfoType.hpp"
#include "../ResPkg/RSB/Info.hpp"
#include "../Info/RSBInfoX.hpp"
#include "./JSONPatch.hpp"

namespace $TJ::$Util::$Expand::$RSBPatchX {

	namespace $PatchInfo {

		using $JSONPatch::ValSelector;

		using $ResPkg::$RSB::Category;
		using $ResPkg::$RSB::ResStoreMethod;

		using $Info::$RSBInfoX::ResType;
		using $Info::$RSBInfoX::ImageRes;

		enum class Tactic : IU8 { kUnKnown, kBase, kCreate, kCreateForce, kRemove };

		inline auto getTactic(Str const & str) {
			Tactic r;
			switch (str.hashByPtr(operator""_sh)) {
				default:
					throw VAMGException("RSBPatchX", "unknown tactic : %s", str.$data);
					break;
				case "base"_sh:
					r = Tactic::kBase;
					break;
				case "create"_sh:
					r = Tactic::kCreate;
					break;
				case "create-force"_sh:
					r = Tactic::kCreateForce;
					break;
				case "remove"_sh:
					r = Tactic::kRemove;
					break;
			}
			return r;
		}
		inline auto getTactic($JSON::Obj const & parent) {
			return (parent.exist("tactic"_r) && parent["tactic"_r].isStr()) ? getTactic(parent["tactic"_r].getStr()) : Tactic::kBase;
		}

		struct PatchBase {

			$Str::Selector $target;
			Tactic $tactic;
			Bool $reset;

			auto & read($JSON::Obj const & t) {
				t["target"_r].get($target);
				$tactic = getTactic(t);
				if (t.exist("reset"_r))
					t["reset"_r].get($reset);
				else
					$reset = kBoF;
				return thiz;
			}

		};

		template <typename T>
		struct PatchItem : PatchBase {

			T $data;

			auto & read($JSON::Obj const & t) {
				PatchBase::read(t);
				if (t.exist("data"_r))
					$data.read(t["data"_r]);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

		};

		struct ResBase {

			Tactic $tactic;
			ExistShell<Path> $src;
			ExistShell<Path> $path;
			ExistShell<Str> $type;

			auto & read($JSON::Obj const & t) {
				$tactic = getTactic(t);
				if ($src.$state = t.exist("src"_r))
					cnvJSONArrToPath(t["src"_r], $src.$data);
				if ($path.$state = t.exist("path"_r))
					cnvJSONArrToPath(t["path"_r], $path.$data);
				if ($type.$state = t.exist("type"_r))
					t["type"_r].get($type.$data);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

		};

		struct AtlasInfo {

			ExistShell<ImgSz> $sz;
			ExistShell<InfoMap<ImageRes>> $img;
			ExistShell<Str> $fmt;

			auto & read($JSON::Obj const & t) {
				if ($sz.$state = t.exist("sz"_r))
					$sz.$data.read(t["sz"_r]);
				if ($img.$state = t.exist("img"_r))
					$img.$data.read(t["img"_r]);
				if ($fmt.$state = t.exist("fmt"_r))
					t["fmt"_r].get($fmt.$data);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

		};

		struct ResData : ResBase {

			ResType $resType;
			Void * $expand;

			auto & freeExpand() {
				if ($resType == ResType::kNormal)
					$expand = kNullPtr;
				else if ($resType == ResType::kAtlas)
					pvFreeS<AtlasInfo>($expand);
				$resType = ResType::kNormal;
				return thiz;
			}

			auto isType(ResType const & t) const {
				return $resType == t;
			}
			auto isNormalType() const {
				return isType(ResType::kNormal);
			}
			auto isAtlasType() const {
				return isType(ResType::kAtlas);
			}

			auto & getAtlasInfo() {
				return *static_cast<AtlasInfo *>($expand);
			}
			auto & getAtlasInfo() const {
				return *static_cast<AtlasInfo const *>($expand);
			}

			auto & setType(ResType const & t) {
				$resType = t;
				if (t == ResType::kNormal)
					$expand = kNullPtr;
				else if (t == ResType::kAtlas)
					$expand = new AtlasInfo();
				return thiz;
			}
			auto & setTypeNormal() {
				return setType(ResType::kNormal);
			}
			auto & setTypeAtlas() {
				return setType(ResType::kAtlas);
			}

			auto & read($JSON::Obj const & t) {
				ResBase::read(t);
				if (!t.exist("atlas"_r))
					setTypeNormal();
				else
					setTypeAtlas().getAtlasInfo().read(t["atlas"_r]);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

			auto & operator=(ResData const & t) {
				ResBase::operator=(t);
				$resType = t.$resType;
				if (t.$resType == ResType::kAtlas) {
					$expand = new AtlasInfo();
					getAtlasInfo() = t.getAtlasInfo();
				}
				return thiz;
			}

			~ResData() {
				freeExpand();
			}

		};

		using Res = PatchItem<ResData>;

		struct SubGroupData {

			ExistShell<Category> $category;
			ExistShell<ResStoreMethod> $resStoreMethod;
			ExistShell<InfoList<Res>> $res;

			auto & read($JSON::Obj const & t) {
				if ($category.$state = t.exist("category"_r))
					$category.$data.read(t["category"_r]);
				if ($resStoreMethod.$state = t.exist("resStoreMethod"_r))
					$resStoreMethod.$data.read(t["resStoreMethod"_r]);
				if ($res.$state = t.exist("res"_r))
					$res.$data.read(t["res"_r]);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

		};

		using SubGroup = PatchItem<SubGroupData>;

		struct GroupData {

			ExistShell<Bool> $composite;
			ExistShell<InfoList<SubGroup>> $subGroup;

			auto & read($JSON::Obj const & t) {
				if ($composite.$state = t.exist("composite"_r))
					t["composite"_r].get($composite.$data);
				if ($subGroup.$state = t.exist("subGroup"_r))
					$subGroup.$data.read(t["subGroup"_r]);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

		};

		using Group = PatchItem<GroupData>;

		struct Info {

			ExistShell<InfoList<Group>> $group;

			auto & read($JSON::Obj const & t) {
				if ($group.$state = t.exist("group"_r))
					$group.$data.read(t["group"_r]);
				return thiz;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}

		};

	}

	inline auto findRes(Path const & path, List<$ResPkg::$RSB::$Info::Res> const & list, Sz & i) {
		return list.range().findBy([&path](auto & v) { return $Path::equal(v.$path, path); }, i);
	}
	inline auto findResPacket(Path const & path, $ResPkg::$RSB::$Info::Info const & info) {
		for (auto const & e : info.$group) {
			for (auto const & ee : e.$v.$subGroup) {
				if (Sz i; findRes(path, ee.$v.$res, i)) {
					return ee.$k;
				}
			}
		}
		return Str();
	}

	namespace $Apply {

		extern Void applyExport(Path const & patchFile, Path const & srcInfoXFile, Path const & srcInfoFile, Path const & dstInfoXFile, Path const & dstInfoFile, Bool const & applyToResFile, Path const & srcResDir, Path const & dstPacketDir, Path const & dstResDir);

	}

}
