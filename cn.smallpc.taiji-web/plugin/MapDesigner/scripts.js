

var $M = MapDataUtil.makeEmptyMap(); // map

var gCurResID = 1536;

var gAnimDivInnerList = {};
var gAnimOriginList = {};

function createOptionList(optionList) {
	let result = [];
	for (let e of optionList)
		result.push($('<option/>').text(e));
	return result;
}

function createOptionListWithValue(optionList) {
	let result = [];
	for (let e of optionList)
		result.push($('<option/>').val(e).text(e));
	return result;
}

function createMultiInfoItem(type, idx) {
	let tipContent = '';
	if (type === 0) {
		tipContent = $M.piece[idx].ref_type + ' ' + $M.piece[idx].ref_idx.toString();
	} else {
		tipContent = $M.event[idx].name;
	}
	let result = $('<div/>').addClass('mdui-row mdui-valign').attr('mdui-tooltip', '{ content: \'' + tipContent + '\' }')
		.append(MDUtil.makeIc((type === 0) ? 'terrain' : 'place').addClass('mdui-col-xs-2'))
		.append($('<span/>').addClass('mdui-col-xs-4 mdui-typo-subheading-opacity').text(idx.toString()))
		.append($('<button/>').addClass('mdui-col-xs-2 mdui-btn mdui-btn-icon').append(MDUtil.makeIc('code')).attr('_t', type.toString()).attr('_i', idx.toString()).bind('click', function () {
			mdui.Tooltip($(this).parent()).close();
			MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(false).appendTarget($(this).attr('_t').toInt(), $(this).attr('_i').toInt());
		})).append($('<button/>').addClass('mdui-col-xs-2 mdui-btn mdui-btn-icon').append(MDUtil.makeIc('clear')).attr('_t', type.toString()).attr('_i', idx.toString()).bind('click', function () {
			mdui.Tooltip($(this).parent()).close();
			MapDesignerUI.stageState.item_select.removeTarget($(this).attr('_t').toInt(), $(this).attr('_i').toInt());
		})).append($('<button/>').addClass('mdui-col-xs-2 mdui-btn mdui-btn-icon').append(MDUtil.makeIc('delete')).attr('_t', type.toString()).attr('_i', idx.toString()).bind('click', function () {
			mdui.Tooltip($(this).parent()).close();
			MapDesignerUI.stageState.item_select.deleteTarget($(this).attr('_t').toInt(), $(this).attr('_i').toInt());
		}));
	return result;
}

function saveResultToFile(type) {
	if (type == 'raw')
		FS.saveJSONFile($M, 'map_raw');
	else if (type == 'ripe')
		FS.saveJSONFile(MapDataUtil.makeStdPvZ2JSON(MapDataUtil.makeMapProto($M)), 'map_ripe');
	MDUtil.showTopSnackbar('save map finish', 3000, true);
}

function getResSizeRate(resID) {
	return 600 / resID;
}
function getCurResSizeRate() { // 目前无用，恒定1536
	return getResSizeRate(gCurResID);
}

function appendItem(type, idx) {
	if (type === 0) {
		let node = createPiece($M.piece[idx], idx);
		MapDesignerUI.element.map.layer.getPiecePL($M.piece[idx].parallax, $M.piece[idx].layer).append(node);
		MapDesignerUI.element.map.item.appendPiece(node);
	} else {
		let node = createEvent($M.event[idx], idx);
		MapDesignerUI.element.map.layer.event.append(node);
		MapDesignerUI.element.map.item.appendEvent(node);
	}
}
function removeItem(type, idx) {
	if (type === 0) {
		$M.piece.splice(idx, 1);
		MapDesignerUI.element.map.item.removePiece(idx);
	} else {
		$M.event.splice(idx, 1);
		MapDesignerUI.element.map.item.removeEvent(idx);
	}
}

function createMap(map) {
	cleanMap();

	if (!MapDesignerUI.cfg.world.hasOwnProperty(map.name)) {
		mdui.snackbar({
			message: 'world-config not found : ' + map.name,
			position: 'top',
			timeout: 10000
		});
		return;
	}

	$M = map;

	if (!Ty.isNull(MapDesignerUI.cfg.setting.forceWorldName))
		$M.name = MapDesignerUI.cfg.setting.forceWorldName;

	// set ctrl
	MapDesignerUI.element.ctrl.map._info.prop('disabled', false);
	MapDesignerUI.element.ctrl.map._view.prop('disabled', false);
	for (let k in MapDesignerUI.element.ctrl.map.info) {
		MapDesignerUI.element.ctrl.map.info[k].val($M[k]);
	}

	// set map

	for (let parallax = gParallaxRange.max; parallax >= gParallaxRange.min; --parallax) {
		for (let layer = gLayerRange.min; layer <= gLayerRange.max; ++layer) {
			MapDesignerUI.element.map.layer_shell.append($('<div/>').addClass('map_layer').attr('_t', 0).attr('_p', parallax).attr('_l', layer));
		}
	}
	MapDesignerUI.element.map.layer_shell.append($('<div/>').addClass('map_layer').attr('_t', 1));
	MapDesignerUI.element.map.layer.getNodeMap();

	for (let i in $M.piece) appendItem(0, i.toInt());
	for (let i in $M.event) appendItem(1, i.toInt());

	// set item-info
	MapDesignerUI.element.map.item_info.shell.show();
	// set item-shop
	MapDesignerUI.element.map.item_shop.shell.show();
	MapDesignerUI.element.map.item_shop.piece_img.append(createShopPieceImg($M.name, MapDesignerUI.cfg.world[$M.name].img));
	MapDesignerUI.element.map.item_shop.piece_anim.append(createShopPieceAnim($M.name, MapDesignerUI.cfg.world[$M.name].anim));
	MapDesignerUI.element.map.item_shop.event.append($('<div/>').addClass('mdui-p-a-1 mdui-row-xs-2 mdui-grid-list mdui-shadow-6').append(
		createShopEventItem('normal', 'level', 'level - normal', $('<div/>').attr('_pvz2-anim', 'level_node/normal').html(gAnimDivInnerList['level_node/normal'])),
		createShopEventItem('minigame', 'level', 'level - minigame', $('<div/>').attr('_pvz2-anim', 'level_node/minigame').html(gAnimDivInnerList['level_node/minigame'])),
		createShopEventItem('miniboss', 'level', 'level - miniboss', $('<div/>').attr('_pvz2-anim', 'level_node/miniboss').html(gAnimDivInnerList['level_node/miniboss'])),
		createShopEventItem('boss', 'level', 'level - boss', $('<div/>').attr('_pvz2-anim', 'level_node/boss/' + $M.name).html(gAnimDivInnerList['level_node/boss/' + $M.name])),
		createShopEventItem('pinata', null, 'pinata', $('<img/>').attr('src', genUrl('item-event-pinata', [MapDesignerUI.cfg.world[$M.name].pinata, false]))),
		createShopEventItem('plant', null, 'plant', $('<img/>').attr('src', genUrl('item-event-plant', ['sunflower'])))
	));

	// set datalist
	MapDesignerUI.element.datalist.event_upgrade.append(createOptionList(MapDesignerUI.cfg.option.GameFeature));
	MapDesignerUI.element.datalist.event_plant.append(createOptionList(MapDesignerUI.cfg.option.Plant));
	{
		let p = [];
		for (let e of MapDesignerUI.cfg.option.Plant) {
			if (MapDesignerUI.cfg['plant-card-bg'][e] == $M.name) {
				p.push(e);
			}
		}
		MapDesignerUI.element.datalist.event_plant_only_cur_world.append(createOptionList(p));
	}
	MapDesignerUI.element.datalist.event_level_data_option.append(createOptionList([$M.name + '/']));
	MapDesignerUI.element.datalist.event_level_narr.append(createOptionList(MapDesignerUI.cfg.option.Narration));

	// init mdui
	mdui.updateTextFields();
	mdui.updateSliders();
	mdui.mutation();

	// last
	mdui.Tab('#page_selector').show(2);
	modMapDivState(1.0, 0.0, 0.0, undefined, undefined);
	modMapDivState(1.0, 0, -$M.pos_y, undefined, undefined);
}

let gOptionListMap = {
	level_type: [
		$('<option/>').text('normal').val('normal'),
		$('<option/>').text('minigame').val('minigame'),
		$('<option/>').text('miniboss').val('miniboss'),
		$('<option/>').text('nonfinalboss').val('nonfinalboss'),
		$('<option/>').text('boss').val('boss'),
		$('<option/>').text('danger').val('danger')
	],

}

// img - [str world, int idx]
// anim - [str world, int idx]
// level_node [str type, * str world]
// event_pinata - [str world, bool isGot]
function genUrl(type, parm) {
	let url = path.res_dir + gCurResID.toString() + '/';
	switch (type) {
		case 'just-prefix': break;
		case 'missing-img':
			url += 'worldmap/common/misssing_artpiece.png';
			break;
		case 'item-piece-img':
			url += 'worldmap/';
			if (MapDesignerUI.cfg.world[parm[0]].img.indexOf(parm[1]) == -1) {
				url += 'common/misssing_artpiece.png';
				break;
			}
			url += parm[0] + '/' + 'island' + parm[1].toString() + '.png';
			break;
		case 'item-piece-anim-img-dir':
			if (gIsLinearMap) {
				url += 'worldmap/' + parm[0] + '/' + 'anim' + parm[1].toString() + '/';
			} else {
				url += 'worldmap/anim_' + parm[0] + parm[1].toString() + '/';
			}
			break;
		case 'item-piece-anim-ripe-json':
			if (gIsLinearMap) {
				url += 'worldmap/' + parm[0] + '/' + 'anim' + parm[1].toString() + '/' + 'anim' + parm[1].toString() + '.css.json';
			} else {
				url += 'worldmap/anim_' + parm[0] + parm[1].toString() + '/' + 'anim_' + parm[0] + parm[1].toString() + '.css.json';
			}
			break;
		case 'item-event-level-node-img-dir':
			url += 'worldmap/';
			switch (parm[0]) {
				case 'normal':
					url += 'level_node/';
					break;
				case 'minigame':
					url += 'level_node_minigame/';
					break;
				case 'miniboss':
					url += 'level_node_gargantuar/';
					break;
				case 'boss':
					url += 'zomboss_node_' + parm[1] + '/';
					break;
				case 'danger':
					url += 'danger_node_' + parm[1] + '/';
					break;
				case 'boss_hologram':
					url += 'zomboss_node_hologram/';
					break;
			}
			break;
		case 'item-event-level-node-ripe-json':
			url += 'worldmap/';
			switch (parm[0]) {
				case 'normal':
					url += 'level_node/level_node.css.json';
					break;
				case 'minigame':
					url += 'level_node_minigame/level_node_minigame.css.json';
					break;
				case 'miniboss':
					url += 'level_node_gargantuar/level_node_gargantuar.css.json';
					break;
				case 'boss':
					url += 'zomboss_node_' + parm[1] + '/zomboss_node_' + parm[1] + '.css.json';
					break;
				case 'danger':
					url += 'danger_node_' + parm[1] + '/danger_node_' + parm[1] + '.css.json';
					break;
				case 'boss_hologram':
					url += 'zomboss_node_hologram/zomboss_node_hologram.css.json';
					break;
			}
			break;
		case 'item-event-upgrade':
			url += 'worldmap/common/';
			if (MapDesignerUI.cfg.upgrade.hasOwnProperty(parm[0])) {
				url += 'upgrade_' + MapDesignerUI.cfg.upgrade[parm[0]] + '.png';
			} else {
				url += 'misssing_artpiece.png';
			}
			break;
		case 'item-event-plant':
			if (MapDesignerUI.cfg['plant-card-bg'].hasOwnProperty(parm[0])) {
				url += 'UI/packets/' + parm[0] + '.png';
			} else {
				url += 'worldmap/common/misssing_artpiece.png';
			}
			break;
		case 'item-event-plant-card':
			if (MapDesignerUI.cfg['plant-card-bg'].hasOwnProperty(parm[0])) {
				url += 'UI/packets/' + MapDesignerUI.cfg['plant-card-bg-mapper'][MapDesignerUI.cfg['plant-card-bg'][parm[0]]] + '.png';
			} else {
				url += 'worldmap/common/misssing_artpiece.png';
			}
			break;
		case 'item-event-pinata':
			if (MapDesignerUI.cfg.world.hasOwnProperty(parm[0])) {
				url += 'worldmap/spine_pinatas/' + (parm[1] ? ('pinatas_dust_spine_' + MapDesignerUI.cfg.world[parm[0]].pinata) : ('pinata_' + MapDesignerUI.cfg.world[parm[0]].pinata + '_spine')) + '.png';
			} else {
				url += 'worldmap/common/misssing_artpiece.png';
			}
			break;
	}
	return url;
}

function modMapDivScale(scale) {
	MapDesignerUI.stageState.scale = scale;
	MapDesignerUI.element.map.layer_shell.css('transform', 'scale(' + scale.toString() + ')')
}
function modMapDivPos(x, y) {
	MapDesignerUI.stageState.pos_x = x;
	MapDesignerUI.stageState.pos_y = y;
	for (let parallax = gParallaxRange.max; parallax >= gParallaxRange.min; --parallax)
		MapDesignerUI.element.map.layer.getPieceP(parallax).css('transform', 'translate(' + (-$M.pos_x - x * MapDataUtil.parallaxSpeed[parallax]).toPxString() + ',' + (-$M.pos_y - y).toPxString() + ')');
	MapDesignerUI.element.map.layer.event.css('transform', 'translate(' + (-$M.pos_x - x).toPxString() + ',' + (-$M.pos_y - y).toPxString() + ')');
}
function modMapLayerState(pieceLayer, eventState) {
	MapDesignerUI.stageState.layer_piece = pieceLayer;
	MapDesignerUI.stageState.layer_event = eventState;
	if (Ty.isBool(pieceLayer)) {
		MapDesignerUI.element.map.layer.piece[pieceLayer ? 'attr' : 'removeAttr']('_v', '');
	} else {
		MapDesignerUI.element.map.layer.piece.removeAttr('_v');
		MapDesignerUI.element.map.layer.getPieceL(pieceLayer).attr('_v', '');
	}
	MapDesignerUI.element.map.layer.event[eventState ? 'attr' : 'removeAttr']('_v', '');
}
// x & y : position, float
// layer : draw-layer, bool or int, if bool, change all draw-layer
function modMapDivState(scale, x, y, layer_piece, layer_event) {
	if (Ty.isUndef(scale)) scale = 1.0;
	if (Ty.isUndef(x)) x = 0.0;
	if (Ty.isUndef(y)) y = 0.0;
	if (Ty.isUndef(layer_piece)) layer_piece = true;
	if (Ty.isUndef(layer_event)) layer_event = true;

	if (Ty.isNum(scale)) {
		modMapDivScale(scale);
		MapDesignerUI.element.ctrl.map.view_show.scale.text((scale * 100.0).toFixed(0).toString() + ' %');
	}
	if (Ty.isNum(x)) {
		modMapDivPos(x, MapDesignerUI.stageState.pos_y);
		MapDesignerUI.element.ctrl.map.view_show.pos_x.text(x.toFixed(1).toString());
		MapDesignerUI.element.ctrl.map.view.pos_x.val(100.0 * x / $M.size_w);
		mdui.updateSliders(MapDesignerUI.element.ctrl.map.view.pos_x.parent());
	}
	if (Ty.isNum(y)) {
		modMapDivPos(MapDesignerUI.stageState.pos_x, y);
		MapDesignerUI.element.ctrl.map.view_show.pos_y.text(y.toFixed(1).toString());
		MapDesignerUI.element.ctrl.map.view.pos_y.val(100.0 * y / $M.size_h);
		mdui.updateSliders(MapDesignerUI.element.ctrl.map.view.pos_y.parent());
	}
	if (Ty.isBool(layer_piece)) {
		modMapLayerState(layer_piece, MapDesignerUI.stageState.layer_event);
		if (layer_piece) {
			MapDesignerUI.element.ctrl.map.view.layer_show.prop('checked', true);
			MapDesignerUI.element.map.item_shop.piece_img.removeAttr('_sd');
			MapDesignerUI.element.map.item_shop.piece_anim.removeAttr('_sd');
		} else {
			MapDesignerUI.element.ctrl.map.view.layer_hide.prop('checked', true);
			MapDesignerUI.element.map.item_shop.piece_img.attr('_sd', '');
			MapDesignerUI.element.map.item_shop.piece_anim.attr('_sd', '');
		}
	} else if (Ty.isNum(layer_piece)) {
		modMapLayerState(layer_piece, MapDesignerUI.stageState.layer_event);
		MapDesignerUI.element.ctrl.map.view.layer_focus.prop('checked', true);
		MapDesignerUI.element.ctrl.map.view_show.layer.text(((layer_piece > 0) ? '+' : '') + layer_piece.toString());
		MapDesignerUI.element.ctrl.map.view.layer.val(layer_piece);
		MapDesignerUI.element.map.item_shop.piece_img.removeAttr('_sd');
		MapDesignerUI.element.map.item_shop.piece_anim.removeAttr('_sd');
		mdui.updateSliders(MapDesignerUI.element.ctrl.map.view.layer.parent());
	}
	if (Ty.isBool(layer_event)) {
		modMapLayerState(MapDesignerUI.stageState.layer_piece, layer_event);
		if (layer_event) {
			MapDesignerUI.element.ctrl.map.view.layer_event.prop('checked', layer_event);
			MapDesignerUI.element.map.item_shop.event.removeAttr('_sd');
		} else {
			MapDesignerUI.element.ctrl.map.view.layer_event.prop('checked', false);
			MapDesignerUI.element.map.item_shop.event.attr('_sd', '');
		}
	}
}

// 执行UI初始化
$(() => { MapDesignerUI.init(); });