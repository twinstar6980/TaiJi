function jsonCpy(src) {
	return JSON.parse(JSON.stringify(src));
}

function jsonGetVal(parent, path) {
	if (path.length == 1)
		return (Ty.isUndef(parent) || Ty.isNull(parent)) ? null : ((Ty.isUndef(parent[path[0]]) || Ty.isNull(parent[path[0]])) ? null : parent[path[0]]);
	else
		return jsonGetVal(parent[path[0]], path.slice(1));
}
function jsonGetValDeep(parent, path) {
	if (path.length == 1)
		return (Ty.isUndef(parent) || Ty.isNull(parent)) ? null : ((Ty.isUndef(parent[path[0]]) || Ty.isNull(parent[path[0]])) ? null : jsonCpy(parent[path[0]]));
	else
		return jsonGetValDeep(parent[path[0]], path.slice(1));
}

function jsonModVal(parent, path, val) {
	if (path.length == 1)
		parent[path[0]] = val;
	else
		jsonModVal(parent[path[0]], path.slice(1), val);
	return;
}

function jsonAddMember(parent, path, key, val) {
	if (path.length == 0)
		parent[key] = val;
	else
		jsonAddMember(parent[path[0]], path.slice(1), key, val);
	return;
}
function jsonAddEle(parent, path, val) {
	if (path.length == 0)
		parent.push(val);
	else
		jsonAddEle(parent[path[0]], path.slice(1), val);
	return;
}

function jsonRemoveMember(parent, path) {
	if (path.length == 1)
		delete parent[path[0]];
	else
		jsonRemoveMember(parent[path[0]], path.slice(1));
	return;
}
function jsonRemoveEle(parent, path) {
	if (path.length == 1)
		parent.splice(path[0], 1);
	else
		jsonRemoveEle(parent[path[0]], path.slice(1));
	return;
}
