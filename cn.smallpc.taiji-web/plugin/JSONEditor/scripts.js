
let dir = {
	config: 'C:\\ProgramData\\cn.smallpc\\taiji\\web\\JSON-Editor\\cfg\\'
}

function genDataListNameOfSheet(name, type) {
	return '_datalist_sheet_' + name + '_' + type;
}

function makeDocInfoDataListEmpty(name, type) {
	return DocUtil.makeDataList(genDataListNameOfSheet(name, type), null).attr('_n', name).attr('_t', type);
}

function createDataListNodeOfSheet(sheet, name, type) {
	let result = makeDocInfoDataListEmpty(name, type);
	for (let optText of (type == 'uid') ? getUIDList(sheet) : ((type == 'alias') ? getAliasList(sheet) : []))
		result.append(DocUtil.makeOpt(optText, null));
	return result;
}

let $C = {
	classTmpl: {},
	classTmplDesc: {},
	typeAlias: {},
	option: {},
	mapper: {}
}

let $D = {
	doc: {},
	cur: null,
	cl: null,

	curDoc: function () {
		return this.doc[this.cur].data;
	},
	getCLDoc: function () {
		if (this.cl === null)
			return null;
		return this.doc[this.cl].data;
	},

	initDocInfoX: function (name) {
		$('#_datalist_sheetname').append(DocUtil.makeOpt(name, null));
		$('#_sheet_obj_uid_shell').append(makeDocInfoDataListEmpty(name, 'uid'));
		$('#_sheet_obj_alias_shell').append(makeDocInfoDataListEmpty(name, 'alias'));
		return this;
	},
	eraseDocInfoX: function (name) {
		$('#_datalist_sheetname').children().each(function () {
			if ($(this).text() === name)
				$(this).remove();
		});
		$('#_sheet_obj_uid_shell').children().each(function () {
			if ($(this).attr('_n') === name)
				$(this).remove();
		});
		$('#_sheet_obj_alias_shell').children().each(function () {
			if ($(this).attr('_n') === name)
				$(this).remove();
		});
		return this;
	},
	updateDocInfoX: function (docID, name) {
		$('#_sheet_obj_uid_shell').children().each(function () {
			if ($(this).attr('_n') === name)
				$(this).replaceWith(createDataListNodeOfSheet($D.doc[docID].data, name, 'uid'));
		});
		$('#_sheet_obj_alias_shell').children().each(function () {
			if ($(this).attr('_n') === name)
				$(this).replaceWith(createDataListNodeOfSheet($D.doc[docID].data, name, 'alias'));
		});
		return this;
	},
	renameDocInfoX: function (oldName, newName) {
		$('#_datalist_sheetname').children().each(function () {
			if ($(this).text() === oldName)
				$(this).text(newName);
		});
		$('#_sheet_obj_uid_shell').children().each(function () {
			if ($(this).attr('_n') === oldName) {
				$(this).attr('_n', newName);
				this.id = genDataListNameOfSheet(newName, 'uid');
			}
		});
		$('#_sheet_obj_alias_shell').children().each(function () {
			if ($(this).attr('_n') === oldName) {
				$(this).attr('_n', newName);
				this.id = genDataListNameOfSheet(newName, 'alias');
			}
		});
		return this;
	},

	setCL: function (docID) { // todo
		this.eraseDocInfoX('CurrentLevel');
		this.cl = docID;
		if (this.cl !== null) {
			this.initDocInfoX('CurrentLevel');
		}
		this.updateCL();
		return this;
	},
	updateCL: function () {
		if (this.cl === null)
			return;
		this.updateDocInfoX(this.cl, 'CurrentLevel');
		return this;
	},

	updateCurDocInfoX: function () {
		this.updateDocInfoX(this.cur, '.');
		this.updateDocInfoX(this.cur, this.cur);
		this.updateCL();
		return this;
	},

	rename: function (oldName, newName) {
		$D.doc[newName] = $D.doc[oldName];
		delete $D.doc[oldName];
		this.renameDocInfoX(oldName, newName);
		return this;
	},
	remove: function (key) {
		delete this.doc[key];
		this.eraseDocInfoX(key);
		$('#doc-table>tbody>tr').each(function () {
			if ($(this).attr('_k') == key)
				$(this).remove();
		});
		$('#_datalist_sheetname').children().each(function () {
			if ($(this).text() == key)
				$(this).remove();
		});
		if (this.cl == key)
			this.cl = null;
		return this;
	},
	append: function (doc, key, path) {
		this.doc[key] = { data: doc };
		this.initDocInfoX(key).updateDocInfoX(key, key);
		for (let e of this.doc[key].data) {
			if (e.Class == 'LevelDefinition') {
				this.setCL(key);
				break;
			}
		}
		$('#doc-table>tbody')
			.append($('<tr/>').attr('_k', key)
				.append($('<td/>')
					.append(MDUtil.makeIcBtn('file_download', () => { $D.saveFile($(this).parent().parent().attr('_k'), true); }))
					.append(MDUtil.makeIcBtn('remove', () => { $D.remove($(this).parent().parent().attr('_k')); }))
					.append(MDUtil.makeIcBtn('keyboard_arrow_right', () => { $D.startADocEdit($(this).parent().parent().attr('_k')); }))
				)
				.append($('<td/>')
					.append(MDUtil.makeCheckBox({ _d_s_t: 'doc' }, null, false))
				)
				.append($('<td/>')
					.append(MDUtil.makeRadio({ _k: key }, () => {
						$('#-doc-table-cl-enable').prop('checked', true);
						$D.setCL($(this).parent().parent().parent().attr('_k'));
					}, 'CurrentLevel', false))
				)
				.append($('<td/>')
					.append($('<div/>').addClass('mdui-textfield')
						.append($('<input/>').addClass('mdui-textfield-input').attr('type', 'text').val(key).bind('change', () => {
							$D.rename($(this).parent().parent().parent().attr('_k'), this.value);
							$(this).parent().parent().parent().attr('_k', this.value);
						}))
					)
				)
				.append($('<td/>').text(path))
			);
		return this;
	},
	saveFile : function (key, ripeOrNot) {
		if (!this.doc.hasOwnProperty(key))
			return;
		saveJSONFile(ripeOrNot ? genProtoPVZ2JSON(this.doc[key].data, $C.classTmpl) : this.doc[key].data, key);
		mdui.snackbar({
			message: 'save finish',
			position: 'top'
		});
	},

	startADocEdit: function (key) {
		this.closeADocEdit();
		this.cur = key;
		this.initDocInfoX('.').updateDocInfoX(this.cur, '.');
		reCreateObjTable();
		reCreateWaveTable();
		mdui.Tab('#page-selector').show(1);
		return this;
	},
	closeADocEdit: function () {
		this.cur = null;
		this.eraseDocInfoX('.');
		// clean table
		return this;
	},

	getCLWaveManager: function () {
		if (this.getCLDoc() === null)
			return null;
		for (let e of this.getCLDoc()) {
			if (e.class === 'WaveManagerProperties')
				return e.data;
		}
		return null;
	}
}

var gCurValTypeList = [];
var gCurValPathList = [];

var gCurWorldCompletionData = undefined;

function reBuildDataEditUI(objIdx) {
	reCreateDataTable([$D.curDoc()[objIdx].class], [objIdx, 'data']);
	reBuildPathListLess(-1);
	appendPathItem([$D.curDoc()[objIdx].class], [objIdx, 'data']);
	return;
}

function appendPathItem(typePair, valPath) {
	$('#_cur_val_path').append($('<li/>').addClass('mdui-list-item').attr('_lv', gCurValPathList.length).bind('click', function () {
		let level = $(this).attr('_lv').toInt();
		reCreateDataTable(gCurValTypeList[level], gCurValPathList[level]);
		reBuildPathListLess(level);
	}).text(genValPathRipeStrFromRawStrSingle(valPath[valPath.length - 1])));
	gCurValTypeList.push(typePair);
	gCurValPathList.push(valPath);
	return;
}
function reBuildPathListLess(level) {
	$('#_cur_val_path').children().each(function () {
		if ($(this).attr('_lv').toInt() > level)
			$(this).remove();
	});
	gCurValTypeList.splice(level + 1, gCurValTypeList.length - 1 - level);
	gCurValPathList.splice(level + 1, gCurValPathList.length - 1 - level);
	return;
}
function getCurValType() {
	return gCurValTypeList[gCurValTypeList.length - 1];
}
function getCurValPath() {
	return gCurValPathList[gCurValPathList.length - 1];
}

function reCreateObjTable() {
	$D.updateCurDocInfoX();
	let bodyChildList = [];
	for (let i in $D.curDoc()) {
		bodyChildList.push($('<tr/>').attr('_i', i)
			.append($('<td/>')
				.append(MDUtil.makeIcBtn('add', function () {
					$D.curDoc().push(jsonCpy($D.curDoc()[$(this).parent().parent().attr('_i').toInt()]));
					reCreateObjTable();
				}))
				.append(MDUtil.makeIcBtn('remove', function () {
					jsonRemoveEle($D.curDoc(), [$(this).parent().parent().attr('_i').toInt()]);
					reCreateObjTable();
				}))
				.append(MDUtil.makeIcBtn('keyboard_arrow_right', function () {
					reBuildDataEditUI($(this).parent().parent().attr('_i').toInt());
					mdui.Tab('#_select_ui').show(2);
				}))
			)
			.append($('<td/>')
				.append(MDUtil.makeCheckBox({ _d_s_t: 'obj' }, null, false).append($('<span/>').addClass('mdui-m-l-1').text(i.toString())))
			)
			.append($('<td/>')
				.append($('<div>').addClass('mdui-textfield')
					.append($('<input>').addClass('mdui-textfield-input').attr('type', 'text').val(getStrIfExist($D.curDoc()[i].uid)).bind('change', function () {
						$D.curDoc()[$(this).parent().parent().parent().attr('_i').toInt()].uid = this.value;
						$D.updateCurDocInfoX();
					})
					)
				)
			)
			.append($('<td/>')
				.append($('<div>').addClass('mdui-textfield')
					.append($('<input>').addClass('mdui-textfield-input').attr('type', 'text').val(getStrIfExist($D.curDoc()[i].alias)).bind('change', function () {
						$D.curDoc()[$(this).parent().parent().parent().attr('_i').toInt()].alias = this.value;
						$D.updateCurDocInfoX();
					})
					)
				)
			)
			.append($('<td/>')
				.append($('<div>').addClass('mdui-textfield')
					.append($('<input>').addClass('mdui-textfield-input').attr('type', 'text').attr('list', '_datalist_classname').val(getStrIfExist($D.curDoc()[i].class)).bind('change', function () {
						$D.curDoc()[$(this).parent().parent().parent().attr('_i').toInt()].class = this.value;
						$D.curDoc()[$(this).parent().parent().parent().attr('_i').toInt()].data = createEmptyTypeData(this.value, $C.classTmpl);
					})
					)
				)
			)
		);
	}
	let result = $('<table/>').addClass('mdui-table mdui-table-hoverable').attr('id', '_obj_table')
		.append($('<thead/>')
			.append($('<tr/>')
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('add', function () {
						$('[_d_s_t=obj]').each(function () {
							if (this.checked) {
								$D.curDoc().push(jsonCpy($D.curDoc()[$(this).parent().parent().parent().attr('_i').toInt()]));
							}
						});
						reCreateObjTable();
					}))
					.append(MDUtil.makeIcBtn('remove', function () {
						$($('[_d_s_t=obj]').get().reverse()).each(function () {
							if (this.checked) {
								jsonRemoveEle($D.curDoc(), [$(this).parent().parent().parent().attr('_i').toInt()]);
							}
						});
						reCreateObjTable();
					}))
					.append(MDUtil.makeIcBtn('fiber_new', function () {
						$D.curDoc().push({
							uid: null,
							alias: null,
							class: null,
							data: null
						});
						reCreateObjTable();
					}))
				)
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('loop', function () {
						$('[_d_s_t=obj]').each(function () {
							this.checked = !this.checked;
						});
					}))
					.append(MDUtil.makeIcBtn('check', function () {
						$('[_d_s_t=obj]').prop('checked', true);
					}))
					.append(MDUtil.makeIcBtn('clear', function () {
						$('[_d_s_t=obj]').prop('checked', false);
					}))
				)
				.append($('<th/>').text('UID').css('width', '20%'))
				.append($('<th/>').text('Alias').css('width', '30%'))
				.append($('<th/>').text('Class').css('width', '30%'))
			)
		)
		.append($('<tbody/>').append(bodyChildList))
	;
	$('#_obj_table').replaceWith(result);
	mdui.mutation($('#_obj_table'));
	return;
}

function createValNode(typeNamePair, valPath) {
	let val = jsonGetVal($D.curDoc(), valPath);
	let valIsExist = !Ty.isNull(val);
	let result = $('<div/>').addClass('mdui-row mdui-valign mdui-align').attr({
		_vt: genTypeNameFull(typeNamePair),
		_vp: genValPathRipeStrFromRawList(valPath)
	});
	switch(typeNamePair[0]) {
		case 'bool': // [ CHECKBOX ]
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield').css('overflow', 'visible')
					.append(MDUtil.makeCheckBox({ title: genTypeNameFull(typeNamePair) }, function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().parent().attr('_vp'), this.checked));
					}, !Ty.isNull(val) && val === true))
				)
			);
			break;
		case 'uint': // [ NUMBER ]
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'number', step: 1, min: 0,
						title: genTypeNameFull(typeNamePair),
					}).val(!Ty.isNull(val) ? val : 0).bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr("_vp"), this.value.toInt()));
					})
					)
				)
			);
			break;
		case 'sint': // [ NUMBER ]
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'number', step: 1,
						title: genTypeNameFull(typeNamePair)
					}).val(!Ty.isNull(val) ? val : 0).bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr('_vp'), this.value.toInt()));
					})
					)
				)
			);
			break;
		case 'float': // [ NUMBER ]
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'number', step: 0.0000001,
						title: genTypeNameFull(typeNamePair)
					}).val(!Ty.isNull(val) ? val : 0).bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr('_vp'), this.value.toFloat()));
					})
					)
				)
			);
			break;
		case 'timeint':
		case 'hashint': // [ NUMBER ]
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'number', step: 1, min: 0,
						title: genTypeNameFull(typeNamePair)
					}).val(!Ty.isNull(val) ? val : 0).bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr('_vp'), this.value.toInt()));
					})
					)
				)
			);
			break;
		case 'string': // [ TEXT ]
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'text',
						title: genTypeNameFull(typeNamePair)
					}).val(!Ty.isNull(val) ? val : '').bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr('_vp'), this.value));
					})
					)
				)
			);
			break;
		case 'RtPtr': // [ SELECT TEXT TEXT ]
			if (!Ty.isNull(val) && val[0] != 'Null' && val[0] != 'uid' && val[0] != 'alias')
				console.log('--------- ERROR RtPtr Type ----------');
			result.append($('<div/>').addClass('mdui-col-xs-2')
				.append($('<select/>').addClass('mdui-select').attr('mdui-select', '').attr({
					title: genTypeNameFull(typeNamePair)
				}).bind('change', function () {
					let _cur_val_pathString = $(this).parent().parent().attr('_vp');
					let sheetName = $(this).parent().next().children().children().val();
					jsonModVal($D.curDoc(), genValPathRawListFromRipeStr(_cur_val_pathString).concat(0), this.value);
					switch (this.value) {
						case 'null':
							jsonModVal($D.curDoc(), genValPathRawListFromRipeStr(_cur_val_pathString).concat(1), '');
							jsonModVal($D.curDoc(), genValPathRawListFromRipeStr(_cur_val_pathString).concat(2), '');
							$(this).parent().next().children().children().prop('disabled', true).val('');
							$(this).parent().next().next().children().children().prop('disabled', true).attr('list', '').val('');
							break;
						case 'uid':
						case 'alias':
							$(this).parent().next().children().children().prop('disabled', false);
							$(this).parent().next().next().children().children().prop('disabled', false).attr('list', genDataListNameOfSheet(sheetName, this.value));
							break;
					}
					mdui.updateTextFields($(this).parent().parent());
				})
					.append(DocUtil.makeOpt('null', 'null').prop('selected', Ty.isNull(val) || (val[0] == 'null')))
					.append(DocUtil.makeOpt('uid', 'uid').prop('selected', !Ty.isNull(val) && (val[0] == 'uid')))
					.append(DocUtil.makeOpt('alias', 'alias').prop('selected', !Ty.isNull(val) && (val[0] == 'alias')))
				)
			);
			result.append($('<div/>').addClass('mdui-col-xs-5')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'text', list: '_datalist_sheetname',
						title: genTypeNameFull(typeNamePair)
					}).val(!Ty.isNull(val) ? val[1] : '').bind('change', function () {
						let _cur_val_pathString = $(this).parent().parent().parent().attr('_vp');
						let sheetName = this.value;
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr(_cur_val_pathString).concat(1), this.value);
						$(this).parent().parent().next().children().children().attr('list', genDataListNameOfSheet(sheetName, $(this).parent().parent().prev().children().val()));
					}).prop('disabled', Ty.isNull(val) || (val[0] == 'Null'))
					)
				)
			);
			result.append($('<div/>').addClass('mdui-col-xs-5')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'text', list: !Ty.isNull(val) ? genDataListNameOfSheet(val[1], val[0]) : '',
						title: genTypeNameFull(typeNamePair)
					}).val(!Ty.isNull(val) ? val[2] : '').bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr('_vp')).concat(2), this.value);
					}).prop('disabled', Ty.isNull(val) || (val[0] == 'Null'))
					)
				)
			);
			break;
		case 'select': {
			let isEmptyOrError = true;
			let optNodeList = [];
			for (let opt of $C.option[typeNamePair[1]]) {
				if (opt === val)
					isEmptyOrError = false;
				optNodeList.push(DocUtil.makeOpt(opt, opt).prop('selected', opt === val));
			}
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<select/>').addClass('mdui-select').attr('mdui-select', '').attr({
						title: genTypeNameFull(typeNamePair)
					}).bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().attr('_vp'), this.value));
					}).append(optNodeList).prepend(DocUtil.makeOpt('', '').prop('selected', isEmptyOrError)))
			);
			break;
		}
		case 'mapper':
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<div/>').addClass('mdui-textfield')
					.append($('<input/>').addClass('mdui-textfield-input').attr({
						type: 'text', list: '_datalist_mapper_' + typeNamePair[1],
						title: genTypeNameFull(typeNamePair)
					}).val(val + ' * ' + getMapperVal($C.mapper[typeNamePair[1]], val)).bind('change', function () {
						jsonModVal($D.curDoc(), genValPathRawListFromRipeStr($(this).parent().parent().parent().attr('_vp'), this.value.split(' * ')[0].toInt()));
					})
					)
				)
			);
			break;
		default:
			result.append($('<div/>').addClass('mdui-col-xs-12')
				.append($('<button/>').addClass('mdui-btn mdui-btn-raised mdui-btn-dense mdui-ripple').attr({
					title: genTypeNameFull(typeNamePair)
				}).text(genTypeNameFull(typeNamePair)).bind('click', function () {
					let type = genTypeNamePair($(this).parent().parent().attr('_vt'));
					let path = genValPathRawListFromRipeStr($(this).parent().parent().attr('_vp'));
					console.log(path);
					appendPathItem(jsonCpy(type), path);
					reCreateDataTable(type, path);
				}))
			);
			break;
	}
	return [valIsExist, result];
}

function createDataTable(typeNamePair, valPath) {
	let headRow = $('<tr/>');
	let bodyChildList = [];
	if (typeNamePair[1] == null) {
		headRow
			.append($('<th/>').css('width', '10%')
				.append(MDUtil.makeIcBtn('done_all', function () {
					$('[_d_s_t=prop]').each(function () {
						if (!this.checked) {
							$(this).trigger('click');
						}
					})
				}))
				.append(MDUtil.makeIcBtn('clear_all', function () {
					$('[_d_s_t=prop]').each(function () {
						if (this.checked) {
							$(this).trigger('click');
						}
					})
				}))
			)
			.append($('<th/>').text('Property').css('width', '30%'))
			.append($('<th/>').text('Value').css('width', '60%'))
		;
		let classTmpl = $C.classTmpl[typeNamePair[0]];
		for (let subClassName in classTmpl) {
			bodyChildList.push($('<tr/>').append($('<th/>').attr('colSpan', 3).text(subClassName)));
			for (let propName in classTmpl[subClassName]) {
				let newTypeNamePair = genTypeNamePair(classTmpl[subClassName][propName]);
				let newValPath = valPath.concat(propName);
				let valNodeInfo = createValNode(newTypeNamePair, newValPath);
				let propDesc = (
					$C.classTmplDesc.hasOwnProperty(typeNamePair[0]) &&
					$C.classTmplDesc[typeNamePair[0]].hasOwnProperty(subClassName) &&
					$C.classTmplDesc[typeNamePair[0]][subClassName].hasOwnProperty(propName)
				) ? $C.classTmplDesc[typeNamePair[0]][subClassName][propName] : '';
				if (!valNodeInfo[0]) {
					valNodeInfo[1].find('input,button,select').prop('disabled', true).css('pointer-events', 'none');
				}
				bodyChildList.push($('<tr/>')
					.append($('<td/>')
						.append(MDUtil.makeCheckBox({
							_d_s_t: 'prop',
							_vt: genTypeNameFull(newTypeNamePair),
							_vp: genValPathRipeStrFromRawList(newValPath)
						}, function () {
							let type = genTypeNamePair($(this).attr('_vt'));
							let path = $(this).attr('_vp');
							let valNodeTarget = $(this).parent().parent().next().next().children();
							if (!this.checked) {
								jsonModVal($D.curDoc(), genValPathRawListFromRipeStr(path), null);
								let valueNodeInfo = createValNode(type, genValPathRawListFromRipeStr(path));
								valueNodeInfo[1].find('input,button,select').prop('disabled', true).css('pointer-events', 'none');
								valNodeTarget.replaceWith(valueNodeInfo[1]);
							} else {
								jsonModVal($D.curDoc(), genValPathRawListFromRipeStr(path), createEmptyTypeData(type[0], $C.classTmpl));
								valNodeTarget.find('input,button,select').prop('disabled', false).css('pointer-events', 'auto');
							}
							mdui.mutation(valNodeTarget);
							mdui.updateTextFields(valNodeTarget);
						}, valNodeInfo[0]))
					)
					.append($('<td/>').attr('mdui-tooltip', '{ content: "' + propDesc.toString() + '" }').text(propName))
					.append($('<td/>').append(valNodeInfo[1]))
				);
			}
		}
	} else {
		if (typeNamePair[0] == 'list' || typeNamePair[0] == 'vector' || typeNamePair[0] == 'set') {
			headRow
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('add', function () {
						$('[_d_s_t=ele]').each(function () {
							if (this.checked) {
								jsonAddEle($D.curDoc(), getCurValPath(), jsonGetValDeep($D.curDoc(), getCurValPath().concat($(this).parent().parent().parent().attr('_i').toInt())));
							}
						});
						reCreateDataTable(getCurValType(), getCurValPath());
					}))
					.append(MDUtil.makeIcBtn('remove', function () {
						$($('[_d_s_t=ele]').get().reverse()).each(function () {
							if (this.checked) {
								jsonRemoveEle($D.curDoc(), getCurValPath().concat($(this).parent().parent().parent().attr('_i').toInt()));
							}
						});
						reCreateDataTable(getCurValType(), getCurValPath());
					}))
					.append(MDUtil.makeIcBtn('fiber_new', function () {
						jsonAddEle($D.curDoc(), getCurValPath(), createEmptyTypeData(genTypeNamePair(getCurValType()[1])[0], $C.classTmpl));
						reCreateDataTable(getCurValType(), getCurValPath());
					}))
				)
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('loop', function () {
						$('[_d_s_t=ele]').each(function () {
							this.checked = !this.checked;
						});
					}))
					.append(MDUtil.makeIcBtn('check', function () {
						$('[_d_s_t=ele]').prop('checked', true);
					}))
					.append(MDUtil.makeIcBtn('clear', function () {
						$('[_d_s_t=ele]').prop('checked', false);
					}))
				)
				.append($('<th/>').text('Value').css('width', '80%'))
			;
			for (let i in jsonGetVal($D.curDoc(), valPath)) {
				bodyChildList.push($('<tr/>').attr('_i', i.toString())
					.append($('<td/>')
						.append(MDUtil.makeIcBtn('add', function () {
							jsonAddEle($D.curDoc(), getCurValPath(), jsonGetValDeep($D.curDoc(), getCurValPath().concat($(this).parent().parent().attr('_i').toInt())));
							reCreateDataTable(getCurValType(), getCurValPath());
						}))
						.append(MDUtil.makeIcBtn('remove', function () {
							jsonRemoveEle($D.curDoc(), getCurValPath().concat($(this).parent().parent().attr('_i').toInt()));
							reCreateDataTable(getCurValType(), getCurValPath());
						}))
					)
					.append($('<td/>')
						.append(MDUtil.makeCheckBox({ _d_s_t: 'ele' })
							.append($('<span/>').text(i.toString()))
						)
					)
					.append($('<td/>').text())
					.append($('<td/>').append(createValNode(genTypeNamePair(typeNamePair[1]), valPath.concat(i))[1]))
				);
			}
		} else if (typeNamePair[0] == 'map') {
			headRow
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('add', function () {
						$('[_d_s_t=member]').each(function () {
							if (this.checked) {
								jsonAddMember($D.curDoc(), getCurValPath(), '~' + $(this).parent().parent().parent().attr('_i'), jsonGetValDeep($D.curDoc(), getCurValPath().concat($(this).parent().parent().parent().attr('_i'))));
							}
						});
						reCreateDataTable(getCurValType(), getCurValPath());
					}))
					.append(MDUtil.makeIcBtn('remove', function () {
						$('[_d_s_t=member]').each(function () {
							if (this.checked) {
								jsonRemoveMember($D.curDoc(), getCurValPath().concat($(this).parent().parent().parent().attr('_i')));
							}
						});
						reCreateDataTable(getCurValType(), getCurValPath());
					}))
					.append(MDUtil.makeIcBtn('fiber_new', function () {
						jsonAddMember($D.curDoc(), getCurValPath(), 'new ' + getTimeStr(), createEmptyTypeData(genTypeNamePair(genMemberTypeNamePair(getCurValType()[1])[1])[0], $C.classTmpl));
						reCreateDataTable(getCurValType(), getCurValPath());
					}))
				)
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('loop', function () {
						$('[_d_s_t=member]').each(function () {
							this.checked = !this.checked;
						});
					}))
					.append(MDUtil.makeIcBtn('check', function () {
						$('[_d_s_t=member]').prop('checked', true);
					}))
					.append(MDUtil.makeIcBtn('clear', function () {
						$('[_d_s_t=member]').prop('checked', false);
					}))
				)
				.append($('<th/>').text('Key').css('width', '30%'))
				.append($('<th/>').text('Value').css('width', '50%'))
			;
			for (let key in jsonGetVal($D.curDoc(), valPath)) {
				bodyChildList.push($('<tr/>').attr('_i', key)
					.append($('<td/>')
						.append(MDUtil.makeIcBtn('add', function () {
							jsonAddMember($D.curDoc(), getCurValPath(), '~' + $(this).parent().parent().attr('_i'), jsonGetValDeep($D.curDoc(), getCurValPath().concat($(this).parent().parent().attr('_i'))));
							reCreateDataTable(getCurValType(), getCurValPath());
						}))
						.append(MDUtil.makeIcBtn('remove', function () {
							jsonRemoveMember($D.curDoc(), getCurValPath().concat($(this).parent().parent().attr('_i')));
							reCreateDataTable(getCurValType(), getCurValPath());
						}))
					)
					.append($('<td/>')
						.append(MDUtil.makeCheckBox({ _d_s_t: 'member' }))
					)
					.append($('<td/>').append($('<input/>').addClass('mdui-textfield-input').attr('type', 'text').val(key)))
					.append($('<td/>').append(createValNode(genTypeNamePair(genMemberTypeNamePair(typeNamePair[1])[1]), valPath.concat(key))[1]))
				);
			}
		}
	}
	return $('<table/>').addClass('mdui-table mdui-table-hoverable').attr('id', '_data_table')
		.append($('<thead/>').append(headRow))
		.append($('<tbody/>').append(bodyChildList));
}
function reCreateDataTable(typeNamePair, valPath) {
	$('#_data_table').replaceWith(createDataTable(typeNamePair, valPath));
	mdui.mutation();
	return;
}

let gWaveManagerProps = {};
let gWaveEventMap = {};

function cnvWaveInfoFromRaw(objectList) {
	for (let object of objectList)
		if (object.Class == 'WaveManagerProperties')
			gWaveManagerProps = object.Data;
	console.log(gWaveManagerProps);
	for (let waveEventList of gWaveManagerProps.Waves)
		for (let waveEvent of waveEventList) {
			//let alias = waveEvent.substring(5, waveEvent.length - 1 - 14);
			let alias = waveEvent[2];
			for (let object of objectList)
				if (object.alias == alias)
					gWaveEventMap[alias] = object.Data;
		}
	console.log(gWaveEventMap);
	return;
}

function findSub(parent) {


}

function hasParent(sub, parent) {
	let cur = sub;
	while (cur !== null) {
		if (cur === parent)
			return true;
		cur = $C.classTmpl[cur].parent;
	}
	return false;
}

function findObjByAlias(doc, alias) {
	for (let e of doc) {
		if (e.alias === alias)
			return e;
	}
	return null;
}

const stageHTML = "<div class='stage'style='width: 100%;'><div class='lawn'><div class='row'><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div></div><div class='row'><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div></div><div class='row'><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div></div><div class='row'><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div></div><div class='row'><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div><div class='tile'></div></div></div><div class='path'></div></div>";

function createWaveActionEditorContent(className) {
	let result = [];
	switch (className) {
		default: break;
		case 'WaveActionProperties':
			break;
		case 'ModifyConveyorWaveActionProps':
			break;
		case 'DinoWaveActionProps':

			break;
		case 'FrostWindWaveActionProps':
			break;
		case 'SpawnGridItemsWaveActionProps':
			break;
		case 'SpawnGravestonesWaveActionProps':
			break;
		case 'SpawnModernPortalsWaveActionProps':
			break;
		case 'SproutPlantingWaveActionProps':
			break;
		case 'SunDropperWaveActionProps':
			break;
		case 'TidalChangeWaveActionProps':
			break;
		case 'ZombieSpawnerActionProps':
			break;
		case 'BeachStageEventZombieSpawnerProps':
			break;
		case 'HamsterZombieSpawnerProps':
			break;
		case 'RaidingPartyZombieSpawnerProps':
			break;
		case 'StormZombieSpawnerProps':
			break;
		case 'SpawnZombiesJitteredWaveActionProps':
			break;
		case 'SpawnZombiesFromGroundSpawnerProps':
			break;
		case 'SpawnZombiesFromGridItemSpawnerProps':
			break;
		case 'ZombieRainSpawnerProps':
			break;
		case 'SpiderRainZombieSpawnerProps':
			break;
		case 'ParachuteRainZombieSpawnerProps':
			break;
	}
}

function reCreateWaveTable() {
	let bodyChildList = [];
	for (let i in $D.getCLWaveManager().Waves) {
		let curWaveActionList = $('<ul/>').addClass('mdui-list mdui-list-dense');
		for (let e of $D.getCLWaveManager().Waves[i]) {
			curWaveActionList.append($('<li/>').addClass('mdui-list-item mdui-row mdui-p-x-0')
				.append($('<div/>').addClass('mdui-col-xs-2')
					.append(MDUtil.makeIcBtn('add', function () {
						reCreateWaveTable();
					}))
					.append(MDUtil.makeIcBtn('remove', function () {
						reCreateWaveTable();
					}))
					.append(MDUtil.makeIcBtn('keyboard_arrow_right', function () {
						reCreateWaveTable();
					}))
				)
				.append($('<div/>').addClass('mdui-col-xs-2')
					.append($('<div>').addClass('mdui-textfield')
						.append($('<input>').addClass('mdui-textfield-input').attr('type', 'text').val(e[1])).bind('change', function () {
							
						})
					)
				)
				.append($('<div/>').addClass('mdui-col-xs-2')
					.append($('<div>').addClass('mdui-textfield')
						.append($('<input>').addClass('mdui-textfield-input').attr('type', 'text').val(e[2])).bind('change', function () {
							
						})
					)
				)
				.append($('<div/>').addClass('mdui-col-xs-6')
					.append($('<div>').addClass('mdui-textfield')
						.append($('<input>').addClass('mdui-textfield-input').attr('type', 'text').val(findObjByAlias($D.getCLDoc(), e[2]).class)).bind('change', function () {
							
						})
					)
				)
			);
		}
		bodyChildList.push($('<tr/>').attr('_i', i)
			.append($('<td/>')
				.append(MDUtil.makeIcBtn('add', function () {
					//$D.curDoc().push(jsonCpy($D.curDoc()[$(this).parent().parent().attr('_i').toInt()]));
					reCreateWaveTable();
				}))
				.append(MDUtil.makeIcBtn('remove', function () {
					//jsonRemoveEle($D.curDoc(), [$(this).parent().parent().attr('_i').toInt()]);
					reCreateWaveTable();
				}))
			)
			.append($('<td/>')
				.append(MDUtil.makeCheckBox({ _d_s_t: 'wave' }, null, false)
					.append($('<span/>').addClass('mdui-m-l-1').text((i.toInt() + 1).toString()))
				)
			)
			.append($('<td/>')
				.append($('<div/>').addClass('')
					.append(MDUtil.makeIcBtn('fiber_new', function () {
						reCreateWaveTable();
					}))
				)
				.append(curWaveActionList))
		);
	}
	let result = $('<table/>').addClass('mdui-table mdui-table-hoverable').attr('id', '_wave_table')
		.append($('<thead/>')
			.append($('<tr/>')
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('add', function () {
						$('[_d_s_t=wave]').each(function () {
							if (this.checked) {
								//$D.curDoc().push(jsonCpy($D.curDoc()[$(this).parent().parent().parent().attr('_i').toInt()]));
							}
						});
						reCreateWaveTable();
					}))
					.append(MDUtil.makeIcBtn('remove', function () {
						$($('[_d_s_t=wave]').get().reverse()).each(function () {
							if (this.checked) {
								//jsonRemoveEle($D.curDoc(), [$(this).parent().parent().parent().attr('_i').toInt()]);
							}
						});
						reCreateWaveTable();
					}))
					.append(MDUtil.makeIcBtn('fiber_new', function () {
						//$D.curDoc().push();
						reCreateWaveTable();
					}))
				)
				.append($('<th/>').css('width', '10%')
					.append(MDUtil.makeIcBtn('loop', function () {
						$('[_d_s_t=wave]').each(function () {
							this.checked = !this.checked;
						});
					}))
					.append(MDUtil.makeIcBtn('check', function () {
						$('[_d_s_t=wave]').prop('checked', true);
					}))
					.append(MDUtil.makeIcBtn('clear', function () {
						$('[_d_s_t=wave]').prop('checked', false);
					}))
				)
				.append($('<th/>').text('Action').css('width', '80%'))
			)
		)
		.append($('<tbody/>').append(bodyChildList))
	;
	$('#_wave_table').replaceWith(result);
	mdui.mutation($('#_wave_table'));
	return;
}

function initEnv() {
	$('#doc-table-shell, #obj-table-shell, #wave-table-shell').css('height', (MDUtil.getWindowHeightNoAppbar() - 64).toString() + 'px');
	$('#data-table-shell').css('height', (MDUtil.getWindowHeightNoAppbar() - 96).toString() + 'px');
	$('.stage').css('height', ($('.stage').width() / 8.0 * 5.0).toString() + 'px');

	$('#download-cur-raw').bind('click', () => { $D.saveFile($D.cur, false); });
	$('#download-cur-ripe').bind('click', () => { $D.saveFile($D.cur, true); });

	$('#doc-table-upload').bind('click', () => {
		for (let path of FS.selectMultiFile()) {
			let json = FS.readAsJSON(path);
			$D.append(loadProtoPVZ2JSON(json, $C.classTmpl), path.split('.json')[0], path);
		}
	});
	$('#doc-table-remove').bind('click', () => {
		$('[_d_s_t=doc]').each(() => {
			if (this.checked) {
				$D.remove($(this).parent().parent().parent().attr('_k'));
			}
		});
	});
	$('#doc-table-new').bind('click', () => {
		$D.append([], 'auto ' + getTimeStr(), 'null');
	});

	$('#doc-table-reverse').bind('click', () => { $('[_d_s_t=doc]').each(() => { this.checked = !this.checked; }); });
	$('#doc-table-select').bind('click', () => { $('[_d_s_t=doc]').prop('checked', true); });
	$('#doc-table-unselect').bind('click', () => { $('[_d_s_t=doc]').prop('checked', false); });

	$('#doc-table-cl-enable').bind('click', () => {
		if (this.checked) {
			$('[name=CurrentLevel]').each(() => {
				if (this.checked)
					$D.setCL(this.value);
			});
		} else {
			$D.setCL(null);
			$('[name=CurrentLevel]').prop('checked', false);
		}
		$('[_d_s_t=doc]').prop('checked', false);
	});

	{
		if (!FS.exist(dir.config + 'type-alias.json')) {
			MDUtil.showTopSnackbarForError('config file not found');
			return;
		}
		$C.typeAlias = FS.readAsJSON(dir.config + 'type-alias.json');

		if (!FS.exist(dir.config + 'option.json')) {
			MDUtil.showTopSnackbarForError('config file not found');
			return;
		}
		$C.option = FS.readAsJSON(dir.config + 'option.json');

		if (!FS.exist(dir.config + 'mapper-ROW-7.9.1.json')) {
			MDUtil.showTopSnackbarForError('config file not found');
			return;
		}
		$C.mapper = FS.readAsJSON(dir.config + 'mapper-ROW-7.9.1.json');
		{
			$('#_datalist_mappername').empty();
			$('#_mapper_info_shell').empty();
			for (let k in $C.mapper) {
				$('#_datalist_mappername').append(DocUtil.makeOpt(k, k));
				let optNodeList = DocUtil.makeDataList('_datalist_mapper_' + k);
				for (let e of $C.mapper[k])
					optNodeList.append(DocUtil.makeOpt(e.idx.toString() + ' * ' + e.val, null));
				$('#_mapper_info_shell').append(optNodeList);
			}
		}

		if (!FS.exist(dir.config + 'class-template.json')) {
			MDUtil.showTopSnackbarForError('config file not found');
			return;
		}
		$C.classTmpl = loadClassTmplProtoList(FS.readAsJSON(dir.config + 'class-template.json'), $C.typeAlias);
		$('#_datalist_classname').empty();
		for (let k in $C.classTmpl)
			$('#_datalist_classname').append(DocUtil.makeOpt(k, null));
		mdui.mutation();

		if (!FS.exist(dir.config + 'class-template-description.json')) {
			MDUtil.showTopSnackbarForError('config file not found');
			return;
		}
		$C.classTmplDesc = loadClassTmplDescProtoList(FS.readAsJSON(dir.config + 'class-template-description.json'));

	}
}

function eventCreateFillDialog(type, parm) {
	switch (type) {
		case "Array-Integer": {
			let style = parm[0], parm_a = parm[1], parm_b = parm[2];
			console.log(Ty.isStr(style));
			mdui.dialog({
				title: "Fill " + type,
				content: "fill integer " + ((style == "Interval") ? ("between [ " + parm_a.toString() + ", " + parm_b.toString() + " ]") : (parm_a.toString() + " x " + parm_b.toString())) + " ?",
				buttons: [
					{
						text: 'Close'
					},
					{
						text: 'OK',
						onClick: function() {
							if (style == "Interval")
								for (let v = parm_a; v <= parm_b; ++v)
									jsonAddEle($D.curDoc(), getCurValPath(), v);
							else if (style == "Count")
								for (let cnt = 0; cnt < parm_b; ++cnt)
									jsonAddEle($D.curDoc(), getCurValPath(), parm_a);
							reCreateDataTable(getCurValType(), getCurValPath());
							return;
						}
					}
				]
			});
			break;
		}
		case "Array-Mapper": {
			let mapperTarget = parm[0], mapperName = parm[1];
			(new mdui.Dialog(createNode("div", { class: "mdui-dialog" }, null, null, [
				createNode("div", { class: "mdui-dialog-title" }, null, "Fill " + type + " " + mapperTarget, null),
				createNode("div", { class: "mdui-dialog-content mdui-shadow-2" }, null, null, [
					createMapperTableForFillDialog(mapperName, "_fill_mapper_table")
				]),
				createNode("div", { class: "mdui-dialog-actions" }, null, null, [
					createNode("button", { class: "mdui-btn mdui-ripple", "mdui-dialog-close": null }, null, "Close", null),
					createNode("button", { class: "mdui-btn mdui-ripple", "mdui-dialog-confirm": null }, {
						click: function () {
							for (let row of getHEleByID("_fill_mapper_table").getElementsByTagName("tbody")[0].getElementsByTagName("tr"))
								if (row.getAttribute("class") == "mdui-table-row-selected")
									jsonAddEle($D.curDoc(), getCurValPath(),
										(mapperTarget == "Index") ? row.getElementsByTagName("td")[1].innerText.toInt() : row.getElementsByTagName("td")[2].innerText.toString());
							reCreateDataTable(getCurValType(), getCurValPath());
							return;
						}
					}, "OK", null)
				]),
			]), { destroyOnClosed: true })).open();
			mdui.mutation();
			break;
		}
		case "Array-Object-Mapper": {
			let className = parm[0], mapperTarget = parm[1], mapperName = parm[2];
			let rowList = [];
			for (let ele of $C.mapper[mapperName])
				rowList.push(createNode("tr", null, null, null, [
					createNode("td", null, null, ele.Index, null),
					createNode("td", null, null, ele.Value, null)
				]));
			(new mdui.Dialog(createNode("div", { class: "mdui-dialog" }, null, null, [
				createNode("div", { class: "mdui-dialog-title" }, null, "Fill " + type + " " + mapperTarget, null),
				createNode("div", { class: "mdui-dialog-content mdui-shadow-2" }, null, null, [
					createDataTableForFillDialog(className, "_fill_data_table"),
					createMapperTableForFillDialog(mapperName, "_fill_mapper_table")
				]),
				createNode("div", { class: "mdui-dialog-actions" }, null, null, [
					createNode("button", { class: "mdui-btn mdui-ripple", "mdui-dialog-close": null }, null, "Close", null),
					createNode("button", { class: "mdui-btn mdui-ripple", "mdui-dialog-confirm": null }, {
						click: function () {
							let mapperSelect = [];
							for (let row of getHEleByID("_fill_mapper_table").getElementsByTagName("tbody")[0].getElementsByTagName("tr"))
								if (row.getAttribute("class") == "mdui-table-row-selected")
									mapperSelect.push((mapperTarget == "Index") ? row.getElementsByTagName("td")[1].innerText.toInt() : row.getElementsByTagName("td")[2].innerText.toString());
							let objTmpl = {};
							for (let row of getHEleByID("_fill_data_table").getElementsByTagName("tbody")[0].getElementsByTagName("tr"))
								if (row.getAttribute("class") == "mdui-table-row-selected") {
									let tdList = row.getElementsByTagName("td");
									let val = undefined;
									console.log(tdList[2]);
									let inputVal = tdList[2].getElementsByTagName("input")[0].value;
									if (inputVal.length > 0 && inputVal[0] == '$')
										val = inputVal;
									else
										switch (getAttrFromEle(tdList[2], "_type_")) {
											case "bool": {
												val = inputVal == "true";
												break;
											}
											case "sint":
											case "uint":
											case "mapper":
											case "timeint":
											case "hashint": {
												val = inputVal.toInt();
												break;
											}
											case "float": {
												val = inputVal.toFloat();
												break;
											}
											case "string":
											case "select": {
												val = inputVal.toString();
												break;
											}
											case "RtPtr": {
												// TODO
												break;
											}
										}
									objTmpl[tdList[1].innerText.toString()] = val;
								}
							let list = [];
							for (let mapperEle of mapperSelect) {
								let obj = jsonCpy(objTmpl);
								for (let key in obj)
									if (obj[key] == "$ mapper")
										obj[key] = mapperEle;
								jsonAddEle($D.curDoc(), getCurValPath(), obj);
							}
							reCreateDataTable(getCurValType(), getCurValPath());
							return;
						}
					}, "OK", null)
				]),
			]), { destroyOnClosed: true })).open();
			mdui.mutation();
			break;
		}
		case "WorldCompletionData": {
			let style = parm[0], parm_a = parm[1], parm_b = parm[2];
			console.log(Ty.isStr(style));
			mdui.dialog({
				title: "Fill " + type,
				content: JSON.stringify(gCurWorldCompletionData),
				buttons: [
					{
						text: 'Close'
					},
					{
						text: 'OK',
						onClick: function() {
							jsonAddEle($D.curDoc(), getCurValPath(), gCurWorldCompletionData);
							reCreateDataTable(getCurValType(), getCurValPath());
							return;
						}
					}
				]
			});
			break;
		}
	}
	return;
}

function genWorldCompletionDataFromJSON(worldmap) {
	let worldmap_data = worldmap.objects[0].objdata;
	let data = {
		w: worldmap_data.m_worldId,
		e: [],
		r: true
	}
	for (let event of worldmap_data.m_eventList)
		data.e.push({ i: event.m_eventId });
	return data;
}

function createMapperTableForFillDialog(mapperName, id) {
	let rowList = [];
	for (let ele of $C.mapper[mapperName])
		rowList.push(createNode("tr", null, null, null, [
			createNode("td", null, null, ele.Index, null),
			createNode("td", null, null, ele.Value, null)
		]));
	return createNode("table", { class: "mdui-table mdui-table-selectable mdui-m-t-2", id: id }, null, null, [
		createNode("thead", null, null, null, [
			createNode("tr", null, null, null, [
				createNode("th", null, null, "Index", null),
				createNode("th", null, null, "Value", null)
			])
		]),
		createNode("tbody", null, null, null, rowList)
	]);
}
function createDataTableForFillDialog(className, id) {
	let trueItemCnt;
	let bodyChildList = [];
	let classTmpl = $C.classTmpl[className];
	for (let subClassName in classTmpl)
		for (let propName in classTmpl[subClassName])
			bodyChildList.push(createNode("tr", null, null, null, [
				createNode("td", { class: "mdui-textfield" }, null, null, [
					createNode("p", null, null, propName, null)
				]),
				createNode("td", { class: "mdui-textfield", _type_: classTmpl[subClassName][propName] }, null, null, [
					createNode("input", { class: "mdui-textfield-input", type: "text", id: "_fill_val_div_" + propName })
				])
			]));
	return createNode("table", {
		class: "mdui-table mdui-table-selectable mdui-table-hoverable",
		id: id
	}, null, null, [
		createNode("thead", null, null, null, [
			createNode("tr", null, null, null, [
				createNode("th", null, null, "Property", null),
				createNode("th", null, null, "Value", null)
			])
		]),
		createNode("tbody", null, null, null, bodyChildList)
	]);
}

function loadWorldMapForFillTool(inputFile) {
	let reader = new FileReader();
	reader.readAsText(inputFile);
	reader.onload = function () {
		let json = JSON.parse(this.result);
		gCurWorldCompletionData = genWorldCompletionDataFromJSON(json);
		$('#_fill_WorldCompletionData_worldmap_name').text(json.objects[0].objdata.m_worldName);
	};
	return;
}
