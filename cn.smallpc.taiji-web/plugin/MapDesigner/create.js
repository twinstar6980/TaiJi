
function makeIntAttrSelector(key, val) {
	return `['${key}'='${val}']`;
}

function mapItemMouseDown(type, idx, state) {
	if (state) {
		if (!MapDesignerUI.stageState.item_select.isMulti()) {
			if (MapDesignerUI.stageState.item_select.isSelect()) {
				let oldType = MapDesignerUI.stageState.item_select.target[0][0];
				let oldIdx = MapDesignerUI.stageState.item_select.target[0][1];
				MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(true).appendTarget(oldType, oldIdx);
			} else {
				MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(true);
			}
		}
		MapDesignerUI.stageState.item_select.toggleTarget(type, idx);
	} else {
		if (!MapDesignerUI.stageState.item_select.isMulti())
			MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(false).appendTarget(type, idx);
		MapDesignerUI.stageState.item_select.setMove();
	}
}
function mapItemMouseUp() {
	if (MapDesignerUI.stageState.item_select.isMove())
		MapDesignerUI.stageState.item_select.freeMove();
}

function createPieceContent(node, piece) {
	node.empty();
	switch (piece.ref_type) {
		default: break;
		case 'undef':
			break;
		case 'img':
			node.attr('_pt', 0).css(genPieceStyle(piece));
			node.append($('<img/>').addClass('mdui-shadow-6 mdui-hoverable').attr('src', genUrl('item-piece-img', [$M.name, piece.ref_idx])));
			break;
		case 'anim':
			if (MapDesignerUI.cfg.world[$M.name].anim.indexOf(piece.ref_idx) == -1) {
				node.attr('_pt', 0).css(genPieceStyle(piece));
				node.append($('<img/>').attr('src', genUrl('missing-img', [])));
			} else {
				let animName = 'world/' + $M.name + '/anim' + piece.ref_idx.toString();
				node.attr('_pt', 1).css(genPieceStyle(piece, gAnimOriginList[animName]));
				node.append($('<div/>').attr('_pvz2-anim', animName).html(gAnimDivInnerList[animName]));
			}
			break;
	}
	return node;
}

function createPiece(piece, idx) {
	let r = $('<div/>').addClass('map_item').attr('_t', 0).attr('_i', idx).bind({
		mousedown: function(e) {
			if (e.button === 2)
				return;
			e.preventDefault();
			mapItemMouseDown(0, $(this).attr('_i').toInt(), e.ctrlKey);
		},
		mouseup: function() {
			mapItemMouseUp();
		}
	});
	createPieceContent(r, piece);
	return r;
}

function createEventContent(node, event) {
	node.empty();
	node.attr('_et', event.type);
	switch (event.type) {
		default: break;
		case 'giftbox':
			node.append($('<div/>').html(gAnimDivInnerList['giftbox/idle']));
			break;
		case 'pinata':
			node.append($('<img/>').attr('src', genUrl('item-event-pinata', [$M.name, event._state])));
			break;
		case 'level': {
			let animName = 'level_node/' + event._level_type;
			let enableText;
			switch (event._level_type) {
				default: break;
				case 'normal':
				case 'minigame':
				case 'miniboss':
				case 'nonfinalboss':
					enableText = true;
					break;
				case 'boss':
				case 'danger':
					animName += '/' + $M.name;
					enableText = false;
					break;
			}
			if (event._level_type === 'boss') {
				node.append($('<div/>').attr('_pvz2-anim', animName).attr('_t', 'danger_node').html(gAnimDivInnerList[animName]));
			} else {
				node.append($('<div/>').attr('_pvz2-anim', animName).attr('_t', 'node').html(gAnimDivInnerList[animName]));
			}
			if (enableText) {
				node.append($('<span/>').attr('_t', 'text').text(event._level_text));
			}
			if (event._level_type == 'boss') {
				node.append($('<div/>').attr('_pvz2-anim', 'level_node/miniboss').attr('_t', 'node').html(gAnimDivInnerList['level_node/miniboss']));
				node.append($('<div/>').attr('_pvz2-anim', 'level_node/boss_hologram').html(gAnimDivInnerList['level_node/boss_hologram']));
			} else if (event._level_type == 'danger') {
				node.append($('<img/>').attr('_t', 'danger_text_bg').attr('src', dir.res + gCurResID.toString() + '/worldmap/danger_level_' + $M.name + '.png'));
				node.append($('<span/>').attr('_t', 'danger_text').text('?'));
			}
			break;
		}
		case 'upgrade':
			node.append($('<img/>').attr('src', genUrl('item-event-upgrade', [event._upgrade_type])));
			break;
		case 'plant':
			node.append($('<img/>').attr('_t', 'bg').attr('src', genUrl('item-event-plant-card', [event._plant_type])));
			node.append($('<img/>').attr('_t', 'fg').attr('src', genUrl('item-event-plant', [event._plant_type])));
			break;
		case 'plantbox':
			break;
	}
	return node;
}
function createEvent(event, idx) {
	let result = $('<div/>').attr('_i', idx).addClass('map_item').attr('_t', 1).bind({
		mousedown: function(e) {
			if (e.button === 2)
				return;
			e.preventDefault();
			mapItemMouseDown(1, $(this).attr('_i').toInt(), e.ctrlKey);
		},
		mouseup: function() {
			mapItemMouseUp();
		}
	}).css('transform', 'translate(' + event.pos_x.toPxString() + ',' + event.pos_y.toPxString() + ')');
	createEventContent(result, event);
	return result;
}

function modPieceState(idx, propList, valIsBaseOn, resetInfoDiv) {
	let dst = $M.piece[idx];
	let modStyle = false, modRef = false, modLayer = false;
	let newParallax = dst.parallax, newLayer = dst.layer;
	for (let key in propList) {
		let val = propList[key];
		if (val === undefined)
			continue;
		let chgNodeType = 0;
		// 0 : normal
		// 1 : checkbox
		// 2 : select
		switch (key) {
			default: break;
			case 'pos_x':
			case 'pos_y':
			case 'scale_x':
			case 'scale_y':
			case 'rotate_angle':
			case 'rotate_rate':
				dst[key] = valIsBaseOn ? (dst[key] + val) : val;
				modStyle = true;
				break;
			case 'rotate_flip':
				dst[key] = Ty.isNull(val) ? !dst[key] : val;
				modStyle = true;
				chgNodeType = 1;
				break;
			case 'ref_type':
				dst[key] = val;
				modRef = true;
				chgNodeType = 2;
				break;
			case 'ref_idx':
				dst[key] = valIsBaseOn ? (dst[key] + val) : val;
				modRef = true;
				break;
			case 'parallax':
				dst[key] = valIsBaseOn ? (dst[key] + val) : val;
				newParallax = dst[key];
				modLayer = true;
				break;
			case 'layer':
				dst[key] = valIsBaseOn ? (dst[key] + val) : val;
				newLayer = dst[key];
				modLayer = true;
				break;
		}		
		if (resetInfoDiv) {
			let node = doce.map.item_info.piece[key];
			if (chgNodeType == 0) {
				node.val(dst[key]);
			} else if (chgNodeType == 1) {
				node.prop('checked', dst[key]);
			} else if (chgNodeType == 2) {
				node.val(dst[key]);
				mdui.Select(node).handleUpdate();
			}
		}
	}
	if (modLayer) {
		let dstLayerNode = doce.map.layer.getPiecePL(newParallax, newLayer);
		let prevIdx = -1;
		for (let e of dstLayerNode.children()) {
			let curIdx = $(e).attr('_i').toInt();
			if (curIdx > idx)
				break;
			prevIdx = curIdx;
		}
		if (prevIdx === -1) {
			dstLayerNode.prepend(doce.map.item.piece[idx].detach());
		} else if (prevIdx !== idx) {
			doce.map.item.piece[prevIdx].after(doce.map.item.piece[idx].detach());
		}
	}
	if (modRef) {
		createPieceContent(doce.map.item.piece[idx], dst);
	} else if (modStyle) {
		doce.map.item.piece[idx].css(genPieceStyle(dst));
	}
	if (resetInfoDiv) {
		if (propList.hasOwnProperty('ref_type') || propList.hasOwnProperty('ref_idx')) {
			$(doce.map.item_info.piece.ref_idx).parent()[(dst.ref_type !== 'undef' && MapDesignerUI.cfg.world[$M.name][dst.ref_type].indexOf(dst.ref_idx) > -1) ? 'removeClass' : 'addClass']('mdui-textfield-invalid');
		}
	}
	if (doce.ctrl.map.view.layer_focus.prop('checked'))
		modMapDivState(null, null, null, dst.layer, null);
	return;
}
function modEventState(idx, propList, valIsBaseOn, resetInfoDiv) {
	let modStyle = false, reCreate = false;
	let dst = $M.event[idx];
	let oriName = dst.name;
	for (let key in propList) {
		let val = propList[key];
		if (val === undefined)
			continue;
		let divType = 0;
		// 0 : normal
		// 1 : checkbox
		// 2 : select
		switch (key) {
			default: break;
			case 'pos_x':
			case 'pos_y':
				modStyle = true;
			case '_gate_cost':
				dst[key] = valIsBaseOn ? (dst[key] + val) : val;
				break;
			case '_upgrade_type':
			case '_plant_type':
			case '_level_data':
			case '_level_text':
				reCreate = true;
			case 'id':
			case 'name':
			case 'parent':
			case 'unlocked_from':
			case 'visible_from':
			case '_level_narr_completed':
			case '_level_narr_unlocked':
			case '_level_toggle':
				dst[key] = val;
				break;
			case 'auto_visible':
			case 'is_challenge':
				dst[key] = Ty.isNull(val) ? !dst[key] : val;
				divType = 1;
				break;
			case 'type':
			case '_level_type':
				reCreate = true;
			case '_level_tutorial':
			case '_level_tutorial_when':
				dst[key] = val;
				divType = 2;
				break;
		}
		if (resetInfoDiv) {
			let node = doce.map.item_info.event[key];
			if (divType == 0) {
				node.val(dst[key]);
			} else if (divType == 1) {
				node.prop('checked', dst[key]);
			} else if (divType == 2) {
				node.val(dst[key]);
				mdui.Select(node).handleUpdate();
			}
		}
	}
	if (reCreate) {
		createEventContent(doce.map.item.event[idx], dst);
	} else if (modStyle) {
		doce.map.item.event[idx].css('transform', 'translate(' + dst.pos_x.toPxString() + ',' + dst.pos_y.toPxString() + ')');
	}
	if (resetInfoDiv) {
		if (propList.hasOwnProperty('name')) {
			$('#_datalist_event_name').empty().append(createOptionList(MapDataUtil.getEventNameList($M.event)));
			if (dst.name !== '' && oriName !== '' && MapDataUtil.getEventNameCount($M.event, dst.name) === 1) {
				for (let e of $M.event) {
					for (let k of ['parent', 'unlocked_from', 'visible_from']) {
						if (e[k] == oriName) {
							e[k] = dst.name;
						}
					}
				}
			}
		}
		for (let e of [ 'id', 'name' ]) {
			if (propList.hasOwnProperty(e)) {
				let isOK = true;
				for (let i = 0; i < $M.event.length; ++i) {
					if (i != idx && $M.event[i][e] == dst[e]) {
						isOK = false;
						break;
					}
				}
				doce.map.item_info.event[e].parent()[isOK ? 'removeClass' : 'addClass']('mdui-textfield-invalid');
			}
		}
		for (let e of [ 'parent', 'unlocked_from', 'visible_from' ]) {
			if (propList.hasOwnProperty(e)) {
				let isOK = dst[e] === '';
				if (!isOK) {
					for (let i = 0; i < $M.event.length; ++i) {
						if (i != idx && $M.event[i].name == dst[e]) {
							isOK = true;
							break;
						}
					}
				}
				doce.map.item_info.event[e].parent()[isOK ? 'removeClass' : 'addClass']('mdui-textfield-invalid');
			}
		}
		if (propList.hasOwnProperty('type')) {
			for (let key in gDeprecatedEventPropMap) {
				doce.map.item_info.event[key].parent().hide();
			}
			for (let key in gSpecialEventPropMap) {
				doce.map.item_info.event[key].parent()[(gSpecialEventPropMap[key].indexOf(dst.type) == -1) ? 'hide' : 'show']();
			}
		}
		if (propList.hasOwnProperty('type') || propList.hasOwnProperty('_plant_type') || propList.hasOwnProperty('_upgrade_type') || propList.hasOwnProperty('_level_data')) {
			switch (dst.type) {
				case 'upgrade':
					break;
				case 'plant':
				case 'plantbox':
					doce.map.item_info.event._upgrade_type.parent()[(MapDesignerUI.cfg.option.Plant.indexOf(dst._plant_type) != -1) ? 'removeClass' : 'addClass']('mdui-textfield-invalid');
					break;
				case 'level':
					doce.map.item_info.event._level_data.parent()[(dst._level_data !== '') ? 'removeClass' : 'addClass']('mdui-textfield-invalid');
					break;
			}
		}
	}
	return;
}

// unused ?
function makeStdPieceImgBySrc(src, pos) {
	let result = jsonCpy(src);
	result.pos_x = pos[0];
	result.pos_y = pos[1];
	return result;
}
function makeStdPieceImg(type, idx, layer, pos) {
	return {
		ref_type: type,
		ref_idx: idx,
		layer: layer,
		parallax: 0,
		pos_x: pos[0],
		pos_y: pos[1],
		scale_x: 1.0,
		scale_y: 1.0,
		rotate_angle: 0,
		rotate_flip: false,
		rotate_rate: 0
	};
}

function makeEventDataFromTmpl(pos, prop) {
	let result = {
		pos_x: pos[0],
		pos_y: pos[1],
		id: 0,
		type: 'none',
		name: '',
		parent: '',
		unlocked_from: '',
		visible_from: '',
		auto_visible: '',
		is_challenge: false,
		_gate_cost: 0,
		_upgrade_type: '',
		_plant_type: '',
		_level_data: '',
		_level_type: 'normal',
		_level_text: '',
		_level_narr_unlocked: '',
		_level_narr_completed: '',
		_level_tutorial: 'none',
		_level_tutorial_when: 'unlocked',
		_level_toggle: ''
	};
	for (let key in prop) {
		result[key] = prop[key];
	}
	return result;
}

function createNewPiece(from, e) {
	e.preventDefault();
	let idx = $M.piece.length;
	let ofs = MapDesignerUI.getRectPointAtLayerShell(e);
	$M.piece.push(makeStdPieceImg($(from).attr('_t'), $(from).attr('_i').toInt(), (MapDesignerUI.stageState.layer_piece === true) ? 0 : MapDesignerUI.stageState.layer_piece, [
		$M.pos_x + MapDesignerUI.stageState.pos_x + ofs[0],
		$M.pos_y + MapDesignerUI.stageState.pos_y + ofs[1]
	]));
	appendItem(0, idx);
	MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(false).appendTarget(0, idx).setMove();
	return;
}
function createNewEvent(from, e) {
	e.preventDefault();
	let idx = $M.event.length;
	let ofs = MapDesignerUI.getRectPointAtLayerShell(e);
	$M.event.push(makeEventDataFromTmpl([
		$M.pos_x + MapDesignerUI.stageState.pos_x + ofs[0],
		$M.pos_y + MapDesignerUI.stageState.pos_y + ofs[1]
	], MapDesignerUI.cfg.ItemShopOfEvent[$(from).attr('_i')]));
	appendItem(1, idx);
	MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(false).appendTarget(1, idx).setMove();
	return;
}

function createShopCard(title, subtitle, content, attrMap, func) {
	let result = $('<div/>').addClass('mdui-col mdui-grid-tile mdui-valign mdui-shadow-2 mdui-hoverable').addClass('map_item_shop_card').attr(attrMap).bind('mousedown', func)
		.append($('<div/>').addClass('mdui-shadow-10').addClass('map_item_shop_card_content').append(content))
		.append($('<div/>').addClass('mdui-grid-tile-actions')
			.append($('<div/>').addClass('mdui-grid-tile-text')
				.append($('<div/>').addClass('mdui-grid-tile-title').text(title))
			)
		);
	if (subtitle !== null) {
		result.find('.mdui-grid-tile-text').append($('<div/>').addClass('mdui-grid-tile-subtitle').text(subtitle));
	}
	return result;
}

function createShopPieceImg(worldName, idxList) {
	let result = $('<div/>').addClass('mdui-p-a-1 mdui-row-xs-2 mdui-grid-list mdui-shadow-6');
	for (let idx of idxList) {
		result.append(createShopCard(idx.toString(), null, $('<img/>').addClass('mdui-shadow-10').attr('src', genUrl('item-piece-img', [worldName, idx])), {
			_t: 'img', _i: idx
		}, function (e) { createNewPiece(this, e); }));
	}
	return result;
}
function createShopPieceAnim(worldName, idxList) {
	let animNamePrefix = 'world/' + worldName + '/anim';
	let result = $('<div/>').addClass('mdui-p-a-1 mdui-row-xs-2 mdui-grid-list mdui-shadow-6');
	for (let idx of idxList) {
		result.append(createShopCard(idx.toString(), null, $('<div/>').attr('_pvz2-anim', animNamePrefix + idx.toString()).html(gAnimDivInnerList[animNamePrefix + idx.toString()]), {
			_t: 'anim', _i: idx
		}, function (e) { createNewPiece(this, e); }));
	}
	return result;
}
function createShopEventItem(title, subtitle, tmplRef, content) {
	return createShopCard(title, subtitle, content, {
		_t: 'event',
		_i: tmplRef
	}, function (e) { createNewEvent(this, e); });
}

function genPieceStyle(piece, other) {
	let result = {};
	if (piece.ref_type == 'img') {
		let transform = '';
		transform += 'translate(' + piece.pos_x.toPxString() + ',' + piece.pos_y.toPxString() + ')';
		transform += 'rotate(' + piece.rotate_angle.toString() + 'deg)';
		if (piece.rotate_flip)
			transform += 'rotateY(180deg)';
		transform += 'scale(' + piece.scale_x.toString() + ',' + piece.scale_y.toString() + ')';
		result['transform'] = transform;
	} else if (piece.ref_type == 'anim') {
		result = {
			top: piece.pos_y.toPxString(),
			left: piece.pos_x.toPxString()
		}
		let origin = jsonCpy((true||Ty.isUndef(other)) ? [ 78.125, 78.125 ] : other);
		if (piece.rotate_flip) {
			[origin[0], origin[1]] = [-origin[0], origin[1]];
		}
		let angle = piece.rotate_angle;
		let transform = '';
		transform += 'translate(' + (-origin[0] * piece.scale_x).toPxString() + ',' + (-origin[1] * piece.scale_y).toPxString() + ')';
		transform += 'rotate(' + angle.toString() + 'deg)';
		if (piece.rotate_flip)
			transform += 'rotateY(180deg)';
		transform += 'scale(' + piece.scale_x.toString() + ',' + piece.scale_y.toString() + ')';
		result['transform'] = transform;

	}
	return result;
}