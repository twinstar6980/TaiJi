"use strict"

/*
		struct _Manifest {
			struct _Version {
				UI32 major;
				UI32 minor;
				UI32 content;

				_Version & Cpy(_Version const & src) {
					major = src.major;
					minor = src.minor;
					content = src.content;
					return *this;
				}
				_Version & Read($JSON::_Obj const & src) {
					major = src["major"].GetUInt();
					minor = src["minor"].GetUInt();
					content = src["content"].GetUInt();
					return *this;
				}
				$JSON::_Obj & Write($JSON::_Obj & dst) const {
					dst.Add("major", major);
					dst.Add("minor", minor);
					dst.Add("content", content);
					return dst;
				}
			};
			struct _Platform {
				Str PlatformName;
				UI32 RSBSize;
				Str full_MD5;
				UI32 DiffSize;
				Str patch_MD5;

				_Platform & Cpy(_Platform const & src) {
					return *this;
				}
				_Platform & Read($JSON::_Obj const & src) {
					src["PlatformName"].GetX(PlatformName);
					if (src["RSBSize"].IsTypeUInt())
						RSBSize = src["RSBSize"].GetUInt();
					else
						RSBSize = 0u;
					if (src["full_MD5"].IsTypeStr())
						src["full_MD5"].GetX(full_MD5).UpperCase();
					else
						full_MD5.SetNull();
					if (src["DiffSize"].IsTypeUInt())
						DiffSize = src["DiffSize"].GetUInt();
					else
						DiffSize = 0u;
					if (src["patch_MD5"].IsTypeStr())
						src["patch_MD5"].GetX(patch_MD5).UpperCase();
					else
						patch_MD5.SetNull();
					return *this;
				}
				$JSON::_Obj & Write($JSON::_Obj & dst) const {
					dst.Add("PlatformName", PlatformName);
					dst.Add("RSBSize", RSBSize, RSBSize == kSzZero);
					dst.Add("full_MD5", full_MD5, full_MD5.IsNull());
					dst.Add("DiffSize", DiffSize, DiffSize == kSzZero);
					dst.Add("patch_MD5", patch_MD5, patch_MD5.IsNull());
					return dst;
				}
			};
			_Version Version;
			ListOfInfo<_Platform> Platform;

			_Manifest & Cpy(_Manifest const & src) {
				return *this;
			}
			_Manifest & Read($JSON::_Obj const & src) {
				Version.Read(src["Version"].GetObj());
				Platform.Read(src["Platform"].GetArr());
				return *this;
			}
			$JSON::_Obj & Write($JSON::_Obj & dst) const {
				Version.Write(dst.AddObj("Version").GetLast().mVal.GetObj());
				Platform.Write(dst.AddArr("Platform").GetLast().mVal.GetArr());
				return dst;
			}
		};
		Str const dlfmt("https://pvz2-live.ecs.popcap.com/%s_%u.%u.%u_%s\n");
		Void AnalysisManifest(Path const & SrcFile, Path const & DstDir) {
			$JSON::Arr SrcData;
			$JSON::Read(SrcFile, SrcData);
			ListOfInfo<_Manifest> ManifestList;
			ManifestList.Read(SrcData);
			$JSON::Write(Path(Path(DstDir).Add("ExportManifest.json")), ManifestList.Write($JSON::Arr().SetNull()));
			
			Str t1, t2, t3, t4, t5, t6, t7, t8;
			
			t1.SetNull();
			t2.SetNull();
			t3.SetNull();
			t4.SetNull();
			t5.SetNull();
			t6.SetNull();
			t7.SetNull();
			t8.SetNull();
			for (auto const & Manifest : ManifestList)
				for (auto const & Platform : Manifest.Platform) {
					if (Platform.PlatformName.Cmp(_StrRef("iphone"))) {
						t1.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb", Platform.RSBSize, Platform.full_MD5.IsNull() ? "" : Platform.full_MD5.mStr);
						t5.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch", Platform.DiffSize, Platform.patch_MD5.IsNull() ? "" : Platform.patch_MD5.mStr);
					}
					if (Platform.PlatformName.Cmp(_StrRef("ipad2"))) {
						t2.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb", Platform.RSBSize, Platform.full_MD5.IsNull() ? "" : Platform.full_MD5.mStr);
						t6.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch", Platform.DiffSize, Platform.patch_MD5.IsNull() ? "" : Platform.patch_MD5.mStr);
					}
					if (Platform.PlatformName.Cmp(_StrRef("ipad3"))) {
						t3.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb", Platform.RSBSize, Platform.full_MD5.IsNull() ? "" : Platform.full_MD5.mStr);
						t7.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch", Platform.DiffSize, Platform.patch_MD5.IsNull() ? "" : Platform.patch_MD5.mStr);
					}
					if (Platform.PlatformName.Cmp(_StrRef("ipod4"))) {
						t4.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb", Platform.RSBSize, Platform.full_MD5.IsNull() ? "" : Platform.full_MD5.mStr);
						t8.PrintFmtByAppend("File: %s_%u.%u.%u_%s\nSize: %u bytes\nMD5: %s\n\n", Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch", Platform.DiffSize, Platform.patch_MD5.IsNull() ? "" : Platform.patch_MD5.mStr);
					}
				}
			$DevUtil::WriteData(Path(DstDir).Add("iPhone_Check.txt"), (Byte *)t1.mStr, t1.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPad2_Check.txt"), (Byte *)t2.mStr, t2.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPad3_Check.txt"), (Byte *)t3.mStr, t3.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPod4_Check.txt"), (Byte *)t4.mStr, t4.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPhone_Patch_Check.txt"), (Byte *)t5.mStr, t5.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPad2_Patch_Check.txt"), (Byte *)t6.mStr, t6.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPad3_Patch_Check.txt"), (Byte *)t7.mStr, t7.mLen);
			$DevUtil::WriteData(Path(DstDir).Add("iPod4_Patch_Check.txt"), (Byte *)t8.mStr, t8.mLen);
			
			t1.SetNull();
			t2.SetNull();
			t3.SetNull();
			t4.SetNull();
			t5.SetNull();
			t6.SetNull();
			t7.SetNull();
			t8.SetNull();
			for (auto const & Manifest : ManifestList)
				for (auto const & Platform : Manifest.Platform) {
					if (Platform.PlatformName.Cmp(_StrRef("iphone")))
						t1.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch");
					if (Platform.PlatformName.Cmp(_StrRef("ipad2")))
						t2.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch");
					if (Platform.PlatformName.Cmp(_StrRef("ipad3")))
						t3.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch");
					if (Platform.PlatformName.Cmp(_StrRef("ipod4")))
						t4.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "diff.rsbpatch");
					if (Platform.PlatformName.Cmp(_StrRef("iphone")))
						t5.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb");
					if (Platform.PlatformName.Cmp(_StrRef("ipad2")))
						t6.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb");
					if (Platform.PlatformName.Cmp(_StrRef("ipad3")))
						t7.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb");
					if (Platform.PlatformName.Cmp(_StrRef("ipod4")))
						t8.PrintFmtByAppend(dlfmt.mStr, Platform.PlatformName.mStr, Manifest.Version.major, Manifest.Version.minor, Manifest.Version.content, "main.rsb");
				}
			Str Text;
			Text.Cat(t1).Cat('\n');
			Text.Cat(t2).Cat('\n');
			Text.Cat(t3).Cat('\n');
			Text.Cat(t4).Cat('\n');
			Text.Cat(t5).Cat('\n');
			Text.Cat(t6).Cat('\n');
			Text.Cat(t7).Cat('\n');
			Text.Cat(t8).Cat('\n');
			$DevUtil::WriteData(Path(DstDir).Add("DownloadLink.txt"), (Byte *)Text.mStr, Text.mLen);
			return;
		}

		Void CnvMapperTextToJSON(Path const & SrcPath, Path const & DstPath) {
			File SrcFile;
			$JSON::Arr DstArr;
			SI32 Index = 0;
			Ch Value[128] = { 0 };
			SrcFile.OpenR(SrcPath);
			DstArr.SetNull();
			while (!SrcFile.IsEnd()) {
				//fscanf(SrcFile.mFile, "%s\n%d\n", Value, &Index);
				//fscanf(SrcFile.mFile, "%d\t%s\n", &Index, Value);
				xLOGLN("%d, %s", Index, Value);
				DstArr.AddObj().GetLast().GetObj().Add("Index", Index).Add("Value", _StrRef(Value));
			}
			SrcFile.Close();
			$JSON::Write(DstPath, DstArr);
			DstArr.SetNull();
			return;
		}*/

function analysisManifest(vserionObjLsst) {
	let downloadLinkFolder = "https://pvz2-live.ecs.popcap.com/";
	let link_MainRSB = [], link_DiffPatch = [];
	for (let vserionObj of vserionObjLsst) {
		let version = vserionObj.Version;
		let verStr = version.major.toString() + "." + version.minor.toString() + "." + version.content.toString();
		for (let platform of vserionObj.Platform) {
			link_MainRSB.push(downloadLinkFolder + platform.PlatformName + "_" + verStr + "_main.rsb");
			link_MainRSB.push(downloadLinkFolder + platform.PlatformName + "_" + verStr + "_diff.rsbpatch");
		}
	}
	return;
}

function modWorldMapNormalize(json) {
	let result = jsonCpy(json);
	for (let e of result.objects[0].objdata.m_eventList)
		if (e.m_eventType == "plant" || e.m_eventType == "plantbox" || e.m_eventType == "upgrade" || e.m_eventType == "bonus") {
			e.m_eventType = "plant";
			e.m_dataString = "imitater"
		}
	return result;
}

function getHomeWorld(src) {
	let result = {};
	for (let e of src) {
		let bg = "";
		let objdata = e.objdata;
		if (objdata.hasOwnProperty("Premium") && objdata.Premium) {
			bg = "premium";
		} else {
			if (objdata.hasOwnProperty("HomeWorld")) {
				bg = objdata.HomeWorld;
			} else {
				bg = null;
			}
		}
		result[objdata.TypeName] = bg;
	}
	return result;
}

function getWorldEvent() { // ...
	return;
}

function batchGenPlantLevelInfo(pbegin, pend, l, x, m) {
	let result = [];
	for (let p = pbegin; p <= pend; ++p)
		result.push({ p: p, l: l, x: x, m: m });
	return result;
}

function modVerCNLevel(json) { // from PropertySheets
	let result = jsonCpy(json);
	for (let obj of result.objects)
		if (obj.hasOwnProperty("objdata")) {
			let objdata = obj.objdata;
			if (objdata.hasOwnProperty("PlantLevelStats"))
				for (let ele of objdata.PlantLevelStats) {
					ele.AttackLevel = 1.0;
					ele.HitPointsLevel = 1.0;
				}
			if (objdata.hasOwnProperty("ZombieLevelStats"))
				for (let ele of objdata.ZombieLevelStats) {
					ele.AttackLevel = 1.0;
					ele.HitPointsLevel = 1.0;
				}
		}
	return result;
}

function cnvCostumePropertySheetToMapper(json) {
	let result = [];
	for (let obj of json.objects)
		if (obj.objclass == "CostumePropertySheet") {
			for (let ele of obj.objdata.CostumeList)
				result.push({ Index: ele.CostumeID, Value: ele.PlantTypeName + " - " + ele.LayerName });
			break;
		}
	return result;
}

function testWorldMapPieceType(map) {
	let mapObj = map.objects[0].objdata;
	let result = { world: mapObj.m_worldName, piece: [], event: [] };
	for (let e of mapObj.m_mapPieces)
		if (result.piece.indexOf(e.m_eventType) == -1)
			result.piece.push(e.m_eventType);
	for (let e of mapObj.m_eventList)
		if (result.event.indexOf(e.m_eventType) == -1)
			result.event.push(e.m_eventType);
	return result;
}

function cnvMapperListToProductItemList(mapperList) {
	let result = {
		plant: [],
		costume: [],
		gameupgrade: []
	};
	for (let ele of mapperList.Plant)
		result.plant.push(ele.Value);
	for (let ele of mapperList.Costume)
		result.costume.push(ele.Index.toString());
	for (let ele of mapperList.GameFeature)
		result.gameupgrade.push(ele.Value);
	return result;
}

function genStdPVZ2JSON(objList) {
	return {
		version: 1,
		objects: objList
	}
}

function genProductID(type, item, cnt) {
	return "product." + type + "." + item + "." + cnt;
}

function genCategoryID(name) {
	return "category." + name;
}

function createNewProduct(type, item, cnt, desc, shortDesc) {
	return {
		aliases: [genProductID(type, item, cnt, desc, shortDesc)],
		objclass: "StoreProductProps",
		objdata: {
			IsFree: true,
			ObjectType: type,
			ObjectItem: item,
			ObjectCount: cnt,
			LawnDescription: desc,
			LawnShortDescription: shortDesc,
			Sku: "sku." + type + "." + item + "." + cnt.toString()
		}
	}
}

function createNewCategory(alias, name, childCategories, products) {
	return {
		aliases: [alias],
		objclass: "StoreCategoryProps",
		objdata: {
			Name: name,
			ChildCategories: childCategories,
			Products: products
		}
	}
}

function genFreeStoreConfig(storeInfo) {
	let products_json = [], store_front_json = [], magento_filter_json = [{
		aliases: ["MagentoFilter"],
		objclass: "ProductFilterPropertySheet",
		objdata: { ObjectTypeFilters: [] }
	}];
	let typeList = {};
	let categoryIDList = [];
	products_json.push({
		aliases: ["product.null"],
		objclass: "StoreProductProps",
		objdata: {
			Sku: "com.popcap.pvz2.android.sprout.5.consume"
		}
	});
	for (let ele of storeInfo) {
		let productIDList = [];
		for (let item of ele.item)
			for (let cnt of ele.count) {
				products_json.push(createNewProduct(ele.type, item, cnt, cnt.toString(), item));
				productIDList.push(genProductID(ele.type, item, cnt));
			}
		if (!typeList.hasOwnProperty(ele.type)) {
			typeList[ele.type] = null;
			let obj = { Type: ele.type };
			if (ele.item.length > 1)
				obj["SubTypeFilters"] = jsonCpy(ele.item);
			magento_filter_json[0].objdata.ObjectTypeFilters.push(obj);
		}
		store_front_json.push(createNewCategory("category." + ele.category + "." + ele.type + "." + ele.item.length.toString() + "." + ele.count.length.toString(), ele.category, undefined, productIDList));
		categoryIDList.push("category." + ele.category + "." + ele.type + "." + ele.item.length.toString() + "." + ele.count.length.toString());

	}
	store_front_json.push(createNewCategory("android PvZ2 Store", "android PvZ2 Store", categoryIDList, undefined));
	store_front_json.push(createNewCategory("iOS PvZ2 Store", "iOS PvZ2 Store", categoryIDList, undefined));
	return [genStdPVZ2JSON(products_json), genStdPVZ2JSON(store_front_json), genStdPVZ2JSON(magento_filter_json)];
}

class JSToolUI {

	static getAllPropertyName(json, dst) {
		if (Ty.isObj(json)) {
			for (let k in json) {
				dst.push(k);
				this.getAllPropertyName(json[k], dst);
			}
		} else if (Ty.isArr(json)) {
			for (let e of json)
				this.getAllPropertyName(e, dst);
		}
		return dst;
	}

	static mixLawnStringJSON(cur, src) {
		let func = function mixLawnStringJSON(cur, src) {
			for (let k in src) {
				if (cur.hasOwnProperty(k)) {
					cur[k] = src[k];
				} else {
					cur['--' + k] = src[k];
				}
			}
		}
		func(cur.objects[0].objdata.LocStringValues, src.objects[0].objdata.LocStringValues);
		return cur;
	}

	static getLevelEventID(json) {
		let result = [];
		for (let e of json.objects[0].objdata.m_eventList) {
			switch (e.m_eventType) {
				default: break;
				case 'none':
				case 'doodad':
				case 'path_node':
				case 'star_gate':
				case 'key_gate':
				case 'powerup':
				case 'upgrade':
				case 'plantbox':
				case 'plant':
				case 'giftbox':
				case 'pinata':
				case 'level':
					result.push(e.m_eventId);
					break;
			}
		}
		return result;
	}
	static makePPMapEventDataByMapJSON(json) {
		let worldId = json.objects[0].objdata.m_worldId;
		let eventId = this.getLevelEventID(json);
		let result = { w: worldId, e: [], r: false };
		for (let e of eventId) {
			result.e.push({ i: e });
		}
		return result;
	}

	static cnvInfoX2Patch(ori) {
		let rgs = [];
		for (let ogk in ori.group) {
			let ogv = ori.group[ogk];
			let rg = {
				target: ogk,
				tactic: "create",
				data: {
					composite: ogv.composite,
					subGroup: []
				}
			};
			for (let osgk in ogv.subGroup) {
				let osgv = ogv.subGroup[osgk];
				let rsg = {
					target: osgk,
					tactic: "create",
					data: {
						category: osgv.category,
						resStoreMethod: [true, true],
						res: []
					}
				};
				for (let ork in osgv.res) {
					let orv = osgv.res[ork];
					let rr = {
						target: ork,
						tactic: "create",
						data: {}
					};
					rr.data['src'] = null;
					for (let k in orv) {
						rr.data[k] = orv[k];
					}
					if (orv.hasOwnProperty("atlas")) {
						rr.data.atlas['fmt'] = ':normal';
					}
					rsg.data.res.push(rr);
				}
				rg.data.subGroup.push(rsg);
			}
			rgs.push(rg);
		}
		return { group: rgs };
	}

	static init() {

		$('#refresh').bind('click', function () { DocUtil.reload(); });

		$('#about').bind('click', function () {
			(new mdui.Dialog($('<div/>').addClass('mdui-dialog')
				.append($('<div/>').addClass('mdui-dialog-title').text('About'))
				.append($('<div/>').addClass('mdui-dialog-content')
					.append($('<p/>').text('JS-Tool. by smallpc'))
					.append($('<div/>').addClass('mdui-typo')
						.append($('<span/>').text(' | '))
						.append($('<a/>').attr('href', 'javascript:;').text('smallpc@qq.com'))
						.append($('<span/>').text(' | '))
						.append($('<a/>').attr('href', 'javascript:;').text('a.small.pink.chick@gmail.com'))
						.append($('<span/>').text(' | '))
						.append($('<a/>').attr('target', '_blank').attr('href', 'https://tieba.baidu.com/home/main?un=%E4%B8%80%E8%B7%AF%E4%B8%8A%E6%B5%B7%E5%A4%96').text('baidu-tieba'))
						.append($('<span/>').text(' | '))
						.append($('<a/>').attr('target', '_blank').attr('href', 'https://space.bilibili.com/12258540').text('bilibili'))
						.append($('<span/>').text(' | '))
					)
				)
				.append($('<div/>').addClass('mdui-dialog-actions')
					.append($('<button/>').addClass('mdui-btn mdui-ripple').attr('mdui-dialog-close', '').text('close'))
				))).open();
		});

		$('#getAllPropertyName').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = JSToolUI.getAllPropertyName(json, []);
			FS.saveJSONFile(result, 'propertyName');
		});
		$('#mixLawnStringJSON').bind('click', function () {
			let pathCur = FS.selectOneFile();
			let pathSrc = FS.selectOneFile();
			let jsonCur = FS.readAsJSON(pathCur);
			let jsonSrc = FS.readAsJSON(pathSrc);
			let result = JSToolUI.mixLawnStringJSON(jsonCur, jsonSrc);
			FS.saveJSONFile(result, 'LawnString');
		});
		$('#makePPMapEventDataByMapJSON').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = JSToolUI.makePPMapEventDataByMapJSON(json);
			FS.saveJSONFile(result, 'map');
		});
		$('#cnvInfoX2Patch').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = JSToolUI.cnvInfoX2Patch(json);
			FS.saveJSONFile(result, 'patch');
		});
		$('#testWorldMapPieceType').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = testWorldMapPieceType(json);
			FS.saveJSONFile(result, result.world);
		});
		$('#modWorldMapNormalize').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = modWorldMapNormalize(json);
			FS.saveJSONFile(result, 'modWorldMapNormalize');
		});
		$('#modVerCNLevel').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = modVerCNLevel(json);
			FS.saveJSONFile(result, 'modVerCNLevel');
		});
		$('#cnvCostumePropertySheetToMapper').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = cnvCostumePropertySheetToMapper(json);
			FS.saveJSONFile(result, 'cnvCostumePropertySheetToMapper');
		});
		$('#getHomeWorld').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let result = getHomeWorld(json.objects);
			FS.saveJSONFile(result, 'getHomeWorld');
		});
		$('#genFreeStoreConfig').bind('click', function () {
			let path = FS.selectOneFile();
			let json = FS.readAsJSON(path);
			let cfg = genFreeStoreConfig(json);
			FS.saveJSONFile(cfg[0], "products");
			FS.saveJSONFile(cfg[1], "storefront");
			FS.saveJSONFile(cfg[2], "magento_filter");
		});

	}

}

$(function () { JSToolUI.init(); });