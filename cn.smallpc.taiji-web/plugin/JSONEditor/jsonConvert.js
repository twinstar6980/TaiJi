
// e.g : 1 -> '.1' , 'name' -> '>nmae'
function genValPathRipeStrFromRawStrSingle(s) {
	return Ty.isNum(s) ? ('.' + s.toString()) : ('>' + s);
}
// e.g : ['name', 1] -> ['>name', '.1']
function genValPathRipeListFromRawList(list) {
	let result = [];
	for (let e of list)
		result.push(genValPathRipeStrFromRawStrSingle(e));
	return result;
}
// e.g : ['name', 1] -> '>name.1'
function genValPathRipeStrFromRawList(list) {
	let result = '';
	for (let e of list)
		result += genValPathRipeStrFromRawStrSingle(e);
	return result;
}
// e.g : '>name.1' -> ['name', 1]
function genValPathRawListFromRipeStr(s) {
	let result = [];
	let pos = 0;
	for (let subStr of s.split(/[\.\>]/).slice(1)) {
		if (s[pos] == '>')
			result.push(subStr);
		else if (s[pos] == '.')
			result.push(parseInt(subStr));
		pos += subStr.length + 1;
	}
	return result;
}

// full : string of type-define
// e.g : null -> [null, null]
// e.g : 't1' -> ['t1', null]
// e.g : 't1<t2>' -> ['t1', 't2']
function genTypeNamePair(full) {
	if (full == null)
		return [null, null];
	else {
		let markInParent = true;
		let parentClass = '', subClass = '';
		for (let ch of full)
			if (markInParent)
				if (ch == '<')
					markInParent = false;
				else
					parentClass += ch;
			else
				subClass += ch;
		subClass = markInParent ? null : jsonCpy(subClass).slice(0, subClass.length - 1);
		return [parentClass, subClass];
	}
}
// e.g : [null, null] -> null
// e.g : ['t1', null] -> 't1'
// e.g : ['t1', 't2'] -> 't1<t2>'
function genTypeNameFull(pair) {
	if (pair[0] == null)
		return null;
	else if (pair[1] == null)
		return jsonCpy(pair[0]);
	else
		return pair[0] + '<' + pair[1] + '>';
}

// e.g : 'string RT_COOMA int' -> ['string', 'int']
function genMemberTypeNamePair(pair) {
	return pair.split(' RT_COMMA ');
}
// e.g : ['string', 'int'] -> 'string RT_COOMA int'
function genMemberTypeNameFull(full) {
	return full[0] + ' RT_COMMA ' + full[1];
}

function getUIDList(src) {
	let result = [];
	for (let k in src)
		if (!Ty.isNull(src[k].uid))
			result.push(src[k].uid);
	return result;
}
function getAliasList(src) {
	let result = [];
	for (let k in src)
		if (!Ty.isNull(src[k].alias))
			result.push(src[k].alias);
	return result;
}

function getMapperIdx(optList, val) {
	let result = -1;
	for (let e of optList)
		if (e.val == val) {
			result = e.idx;
			break;
		}
	return result;
}
function getMapperVal(optList, idx) {
	let result = '';
	for (let e of optList)
		if (e.idx == idx) {
			result = e.val;
			break;
		}
	return result;
}

// cnv Special TypeName to Std TypeName String
// e.g : ['vector', 'int32'] -> ['vector', 'int'] -> 'vector<int>'
function genStdTypeName(pair, aliasList, classList) {
	let resultPair = [null, null];
	switch (pair[0]) {
		case 'map':
			resultPair[0] = pair[0];
			resultPair[1] = genMemberTypeNameFull(['string', genStdTypeName(genTypeNamePair(genMemberTypeNamePair(pair[1])[1]), aliasList, classList)]);
			break;
		case 'vector':
		case 'list':
		case 'set':
			resultPair[0] = pair[0];
			resultPair[1] = genStdTypeName(genTypeNamePair(pair[1]), aliasList, classList);
			break;
		default: {
			let alias = jsonGetValDeep(aliasList, [pair[0]]);
			if (alias != null) {
				resultPair[0] = alias;
				resultPair[1] = pair[1];
			} else if (classList.hasOwnProperty(pair[0])) {
				resultPair[0] = pair[0];
				resultPair[1] = pair[1];
			} else {
				resultPair[0] = null;
				resultPair[1] = null;
			}
			break;
		}
	}
	return genTypeNameFull(resultPair);
}
function loadClassTmplProto(src, className, classPropList, aliasList) {
	let propList = {};
	//console.log(className); // debug
	if (!Ty.isNull(src[className].parent))
		loadClassTmplProto(src, src[className].parent, classPropList, aliasList);
	for (let k in src[className].property)
		propList[k] = genStdTypeName(genTypeNamePair(src[className].property[k]), aliasList, src);
	classPropList[className] = jsonCpy(propList);
	return classPropList;
}
function loadClassTmplProtoList(src, aliasList) {
	let result = {};
	for (let k in src)
		result[k] = jsonCpy(loadClassTmplProto(src, k, {}, aliasList));
	return result;
}

function loadClassTmplDescProto(src, className, classPropDescList) {
	let propDescList = {};
	if (!Ty.isNull(src[className].parent))
		loadClassTmplDescProto(src, src[className].parent, classPropDescList);
	for (let k in src[className].property)
		propDescList[k] = src[className].property[k];
	classPropDescList[className] = jsonCpy(propDescList);
	return classPropDescList;
}
function loadClassTmplDescProtoList(src) {
	let result = {};
	for (let k in src)
		result[k] = jsonCpy(loadClassTmplDescProto(src, k, {}));
	return result;
}

function createEmptyTypeData(typeName, classTmplList) {
	let result;
	switch (typeName) {
		case 'map':
			result = {};
			break;
		case 'list':
		case 'vector':
		case 'set':
			result = [];
			break;
		case 'bool':
			result = false;
			break;
		case 'int':
		case 'uint':
		case 'sint':
		case 'float':
		case 'timeint':
		case 'hashint':
		case 'mapper':
			result = 0;
			break;
		case 'string':
		case 'select':
			result = '';
			break;
		case 'RtPtr':
			result = ['null', '', ''];
			break;
		default:
			result = {};
			for (let subClassName in classTmplList[typeName])
				for (let k in classTmplList[typeName][subClassName])
					jsonAddMember(result, [], k, null);
			break;
	}
	return result;
}
function createTypeData(typeNamePair, src, classTmplList) {
	let result;
	switch (typeNamePair[0]) {
		case 'bool':
		case 'int':
		case 'uint':
		case 'sint':
		case 'float':
		case 'timeint':
		case 'hashint':
		case 'string':
		case 'select':
		case 'mapper':
			result = src;
			break;
		case 'RtPtr':
			result = null;
			if (!Ty.isNull(src)) {
				let rtIdStr = src.split(/[\(\)]/)[1];
				if (rtIdStr == '0')
					result = ['null', '', ''];
				else {
					let rtIdStrMember = rtIdStr.split(/[\@]/);
					result = [(cntChAtStr(rtIdStr, '.') == 2) ? 'uid' : 'alias', rtIdStrMember[1], rtIdStrMember[0]];
				}
			}
			break;
		case 'map':
			result = {};
			for (let k in src)
				result[k] = createTypeData(genTypeNamePair(genMemberTypeNamePair(typeNamePair[1])[1]), src[k], classTmplList);
			break;
		case 'list':
		case 'vector':
		case 'set':
			result = [];
			for (let e of src)
				result.push(createTypeData(genTypeNamePair(typeNamePair[1]), e, classTmplList));
			break;
		default:
			result = {};
			for (let subClassName in classTmplList[typeNamePair[0]])
				for (let k in classTmplList[typeNamePair[0]][subClassName])
					jsonAddMember(result, [], k, Ty.isUndef(src[k]) ? null : createTypeData(genTypeNamePair(classTmplList[typeNamePair[0]][subClassName][k]), src[k], classTmplList));
			break;
	}
	return result;
}
function loadProtoPVZ2JSON(src, classTmplList) {
	let result = [];
	for (let e of src.objects) {
		let dstObj = {};
		dstObj['uid'] = jsonGetVal(e, ['uid']);
		dstObj['alias'] = jsonGetVal(e, ['aliases', 0]);
		dstObj['class'] = jsonGetVal(e, ['objclass']);
		dstObj['data'] = createTypeData([jsonGetVal(e, ['objclass']), null], jsonGetVal(e, ['objdata']), classTmplList);
		result.push(jsonCpy(dstObj));
	}
	return result;
}
function createTypeData_Reserve(typeNamePair, src, classTmplList) {
	let result;
	switch (typeNamePair[0]) {
		case 'bool':
		case 'int':
		case 'uint':
		case 'sint':
		case 'float':
		case 'timeint':
		case 'hashint':
		case 'string':
		case 'select':
		case 'mapper':
			result = src;
			break;
		case 'RtPtr':
			switch (src[0]) {
				case 'null':
					result = 'RTID(0)';
					break;
				case 'uid':
				case 'alias':
					result = 'RTID(' + src[2] + '@' + src[1] + ')';
					break;
			}
			break;
		case 'map':
			result = {};
			for (let k in src)
				result[k] = createTypeData_Reserve(genTypeNamePair(genMemberTypeNamePair(typeNamePair[1])[1]), src[k], classTmplList);
			break;
		case 'list':
		case 'vector':
		case 'set':
			result = [];
			for (let e of src)
				result.push(createTypeData_Reserve(genTypeNamePair(typeNamePair[1]), e, classTmplList));
			break;
		default:
			result = {};
			for (let subClassName in classTmplList[typeNamePair[0]])
				for (let k in classTmplList[typeNamePair[0]][subClassName])
					if (!Ty.isNull(src[k]))
						jsonAddMember(result, [], k, createTypeData_Reserve(genTypeNamePair(classTmplList[typeNamePair[0]][subClassName][k]), src[k], classTmplList));
			break;
	}
	return result;
}
function genProtoPVZ2JSON(src, classTmplList) {
	let result = {};
	result['version'] = 1;
	let dstObjList = [];
	for (let e of src) {
		let dstObj = {};
		if (!Ty.isNull(e.uid) && e.uid != '')
			dstObj['uid'] = e.uid;
		if (!Ty.isNull(e.alias) && e.alias != '') {
			dstObj['aliases'] = [e.alias];
		}
		if (!Ty.isNull(e.class) && e.class != '') {
			dstObj['objclass'] = e.class;
			dstObj['objdata'] = createTypeData_Reserve([e.class, null], e.data, classTmplList);
		}
		dstObjList.push(jsonCpy(dstObj));
	}
	result['objects'] = dstObjList;
	return result;
}
