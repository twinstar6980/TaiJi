"use strict"

// 地图数据处理类
class MapDataUtil {

	// piece-layer 取值范围
	static layerRange = { min: -10, max: 10 };

	// piece-parallax 取值范围
	static parallaxRange = { min: -4, max: 10 };
	
	// piece-parallax 速率
	static parallaxSpeed = {
		'-4': 1.4,
		'-3': 1.3,
		'-2': 1.2,
		'-1': 1.1,
		'0': 1.0,
		'1': 0.9,
		'2': 0.8,
		'3': 0.7,
		'4': 0.6,
		'5': 0.5,
		'6': 0.4,
		'7': 0.3,
		'8': 0.2,
		'9': 0.1,
		'10': 0.0
	};
	
	// 包装原始地图数据为完整PvZ2JSON
	static makeStdPvZ2JSON(data) {
		return {
			'###': 'create by TaiJi : Map-Designer',
			version: 1,
			objects: [
				{
					objclass: 'WorldData',
					objdata: data
				}
			]
		};
	}

	// 生成空地图
	static makeEmptyMap() {
		return {
			id: 0,
			name: '',
			pos_x: 0.0,
			pos_y: 0.0,
			size_w: 0.0,
			size_h: 0.0,
			res_group_id: 0,
			version: 0,
			creation_time: 0,
			piece: [],
			event: []
		};
	}

	// 转换标准地图数据为原始地图数据
	static makeMapProto(src) {
		// 复制基本信息
		let mapProto = {
			m_worldName: src.name,
			m_boundingRect: {
				mX: src.pos_x,
				mY: src.pos_y,
				mWidth: src.size_w,
				mHeight: src.size_h
			},
			m_worldId: src.id,
			m_resGroupID: src.res_group_id,
			m_version: src.version,
			m_creationTime: src.creation_time,
			m_mapPieces: [],
			m_eventList: []
		};
		// 排序event
		for (let i = 0; i < src.event.length; ++i)
			for (let j = i + 1; j < src.event.length; ++j)
				if (src.event[i].eventID > src.event[j].eventID)
					[src.event[i], src.event[j]] = [src.event[j], src.event[i]];

		// piece
		for (let e of src.piece) {
			mapProto.m_mapPieces.push({
				m_eventType: 'island',
				m_imageID: ((e.ref_type == 'img') ? (e.ref_idx - 1) : ((e.ref_type == 'anim') ? (e.ref_idx + 99) : e.ref_idx)),
				m_parallaxLayer: e.parallax,
				m_drawLayer: e.layer,
				m_position: {
					x: e.pos_x,
					y: e.pos_y
				},
				m_scaleX: e.scale_x,
				m_scaleY: e.scale_y,
				m_rotationAngle: e.rotate_angle,
				m_isArtFlipped: e.rotate_flip,
				m_rotationRate: e.rotate_rate
			});
		}
		// event
		for (let e of src.event) {
			let obj = {
				m_position: {
					x: e.pos_x,
					y: e.pos_y
				},
				m_eventId: e.id,
				m_eventType: e.type,
				m_name: e.name,
				m_parentEvent: e.parent,
				m_unlockedFrom: e.unlocked_from,
				m_visibleFrom: e.visible_from,
				m_autoVisible: e.auto_visible,
				m_isChallengeType: e.is_challenge
			};
			// 处理不同类型event
			switch (e.type) {
				case 'none':
				case 'doodad':
				case 'path_node': break;
				case 'star_gate':
				case 'key_gate':
					obj['m_cost'] = e._gate_cost;
					break;
				case 'powerup':
					break;
				case 'upgrade':
					obj['m_dataString'] = e._upgrade_type;
					break;
				case 'plantbox':
				case 'plant':
					obj['m_dataString'] = e._plant_type;
					break;
				case 'giftbox': break;
				case 'pinata': break;
				case 'level':
					obj['m_dataString'] = e._level_data;
					obj['m_levelNodeType'] = e._level_type;
					obj['m_displayText'] = e._level_text;
					obj['m_unlockedNarrationID'] = e._level_narr_unlocked;
					obj['m_completedNarrationID'] = e._level_narr_completed;
					obj['m_worldMapTutorial'] = e._level_tutorial;
					obj['m_worldMapTutorialVisibleWhen'] = e._level_tutorial_when;
					obj['m_toggleName'] = e._level_toggle;
					break;
			}
			mapProto.m_eventList.push(obj);
		}
		return mapProto;
	}

	// 转换原始地图数据为标准地图数据
	static loadMapProto(src) {
		// 复制基本信息
		let map = {
			id: src.m_worldId,
			name: src.m_worldName,
			pos_x: src.m_boundingRect.mX,
			pos_y: src.m_boundingRect.mY,
			size_w: src.m_boundingRect.mWidth,
			size_h: src.m_boundingRect.mHeight,
			res_group_id: src.m_resGroupID,
			version: src.m_version,
			creation_time: src.m_creationTime,
			piece: [],
			event: []
		};
		// piece
		for (let e of src.m_mapPieces) {
			// 视为island型event
			map.piece.push({
				ref_type: isBtwRange(e.m_imageID, [0, 99]) ? 'img' : (isBtwRange(e.m_imageID, [100, 199]) ? 'anim' : 'undef'),
				ref_idx: isBtwRange(e.m_imageID, [0, 99]) ? (e.m_imageID + 1) : (isBtwRange(e.m_imageID, [100, 199]) ? (e.m_imageID + 1 - 100) : e.m_imageID),
				layer: e.m_drawLayer,
				parallax: e.hasOwnProperty('m_parallaxLayer') ? e.m_parallaxLayer : 0,
				pos_x: e.m_position.x,
				pos_y: e.m_position.y,
				scale_x: e.hasOwnProperty('m_scaleX') ? e.m_scaleX : 1.0,
				scale_y: e.hasOwnProperty('m_scaleY') ? e.m_scaleY : 1.0,
				rotate_angle: e.m_rotationAngle,
				rotate_flip: e.m_isArtFlipped,
				rotate_rate: e.m_rotationRate
			});
		}
		// ？？？
		{
			for (let i = 0; i < map.piece.length; ++i) {
				for (let j = i + 1; j < map.piece.length; ++j) {
					/*if (map.piece[i].ref_type === map.piece[j].ref_type && map.piece[i].ref_idx < map.piece[j].ref_idx) {
						console.log('x');
						[map.piece[j], map.piece[i]] = [map.piece[i], map.piece[j]];
					}/*
					if (map.piece[i].pos_x > map.piece[j].pos_x) {
						console.log('x');
						[map.piece[j], map.piece[i]] = [map.piece[i], map.piece[j]];
					}*/
				}
			}
		}
		// event
		for (let e of src.m_eventList) {
			let obj = {
				pos_x: e.m_position.x,
				pos_y: e.m_position.y,
				id: e.m_eventId,
				type: e.m_eventType,
				name: '',
				parent: '',
				unlocked_from: '',
				visible_from: '',
				auto_visible: false,
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
				//,data: e.m_dataString,
			};
			// 处理可选基本数据
			if (e.hasOwnProperty('m_name'))
				obj.name = e.m_name;
			if (e.hasOwnProperty('m_parentEvent'))
				obj.parent = e.m_parentEvent;
			if (e.hasOwnProperty('m_unlockedFrom'))
				obj.unlocked_from = e.m_unlockedFrom;
			if (e.hasOwnProperty('m_visibleFrom'))
				obj.visible_from = e.m_visibleFrom;
			if (e.hasOwnProperty('m_autoVisible'))
				obj.auto_visible = e.m_autoVisible;
			if (e.hasOwnProperty('m_isChallengeType'))
				obj.is_challenge = e.m_isChallengeType;
			// 处理不同event
			switch (e.m_eventType) {
				case 'none':
				case 'doodad':
				case 'path_node': break;
				case 'star_gate':
				case 'key_gate':
					if (e.hasOwnProperty('m_cost'))
						obj._gate_cost = e.m_cost;
					break;
				case 'powerup':
					break;
				case 'upgrade':
					if (e.hasOwnProperty('m_dataString'))
						obj._upgrade_type = e.m_dataString;
					break;
				case 'plantbox':
				case 'plant':
					if (e.hasOwnProperty('m_dataString'))
						obj._plant_type = e.m_dataString;
					break;
				case 'giftbox': break;
				case 'pinata': break;
				case 'level':
					if (e.hasOwnProperty('m_dataString'))
						obj._level_data = e.m_dataString;
					if (e.hasOwnProperty('m_levelNodeType')) {
						if (obj._level_data.search('dangerroom') != -1)
							obj._level_type = 'danger';
						else {
							switch (e.m_levelNodeType) {
								default:
									console.warn('unknown level-node-type : ' + e.m_levelNodeType);
									//obj._level_type = 'none';
									break;
								case 'normal':
								case 'minigame':
								case 'miniboss':
								case 'nonfinalboss':
								case 'boss':
									obj._level_type = e.m_levelNodeType;
									break;
							}
						}
					}
					if (e.hasOwnProperty('m_displayText'))
						obj._level_text = e.m_displayText;
					if (e.hasOwnProperty('m_unlockedNarrationID'))
						obj._level_narr_unlocked = e.m_unlockedNarrationID;
					if (e.hasOwnProperty('m_completedNarrationID'))
						obj._level_narr_completed = e.m_completedNarrationID;
					if (e.hasOwnProperty('m_worldMapTutorial'))
						obj._level_tutorial = e.m_worldMapTutorial;
					if (e.hasOwnProperty('m_worldMapTutorialVisibleWhen'))
						obj._level_tutorial_when = e.m_worldMapTutorialVisibleWhen;
					if (e.hasOwnProperty('m_toggleName'))
						obj._level_toggle = e.m_toggleName;
					break;
			}
			map.event.push(obj);
		}
		return map;
	}

	// 获取event-id表
	static getEventIDList(list) {
		let r = [];
		for (let e of list)
			r.push(e.id);
		return r;
	}
	// 计算可用空闲event-id
	static getFreeEventID(list) {
		let idList = getEventIDList(list);
		let i = 0;
		while (idList.indexOf(i) !== -1)
			++i;
		return i;
	}

	// 获取event-name表
	static getEventNameList(list) {
		let r = [];
		for (let e of list)
			if (Ty.isStr(e.name) && e.name != '')
				r.push(e.name);
		return r;
	}
	// 计算event-list中name为指定值的数目
	static getEventNameCount(list, name) {
		if (name === '')
			return 0;
		let n = 0;
		for (let e of list)
			if (e.name === name)
				++n;
		return n;
	}

}

// 矩形类，用以构造范围选择框
class Rect {

	x; y; w; h;

	constructor(x, y, w, h) {
		this.x = x;
		this.y = y;
		this.w = w;
		this.h = h;
	}

	// 对位置进行偏移
	offset(x, y) {
		this.x += x;
		this.y += y;
	}

	// 判断所给坐标是否位于矩形内
	btw(x, y) {
		return isBtwRange(x, [this.x, this.x + this.w]) && isBtwRange(y, [this.y, this.y + this.h])
	}

}

// 地图设计者UI
class MapDesignerUI {

	// 文件路径，常量
	static path = {
		main_dir: 'C:\\ProgramData\\cn.smallpc\\taiji\\web\\Map-Designer\\',
		res_dir: 'C:\\ProgramData\\cn.smallpc\\taiji\\web\\Map-Designer\\res\\',
		cfg_file: 'C:\\ProgramData\\cn.smallpc\\taiji\\web\\Map-Designer\\cfg.json',
		font_file: 'C:\\ProgramData\\cn.smallpc\\taiji\\web\\Map-Designer\\font.ttf'
	};

	// 配置，从文件中读取
	static cfg = null;

	// HTML元素
	static element = {
		// 抽屉
		drawer: {
			left: null,
			right: null
		},
		// 记录
		record: {
			_shell: null,
			clear: null,
			record: null,
			upload_raw: null,
			download_raw: null,
			upload_ripe: null,
			download_ripe: null,
			list: null
		},
		// 地图（当前记录）
		map: {
			// 基本信息
			info: {
				_all: null,
				id: null,
				name: null,
				pos_x: null,
				pos_y: null,
				size_w: null,
				size_h: null,
				res_group_id: null,
				version: null,
				creation_time: null
			},
			// 仿真视图
			view: {
				// 舞台
				stage: {
					_shell: null,
					bg: null,
					layer_shell: null,
					guideline_shell: null,
					multi_selector_rect: null
				},
				// 舞台控制
				ctrl: {
					_all: null,
					pos_x: null,
					pos_y: null,
					scale: null,
					layer: null,
					layer_focus: null,
					layer_show: null,
					layer_hide: null,
					layer_event: null
				},
				// 舞台控制>文本显示
				ctrl_text: {
					_all: null,
					scale: null,
					pos_x: null,
					pos_y: null,
					layer: null
				}
			},
			// 元件信息（左侧抽屉）
			item_info: {
				_shell: null,
				// 三种类别：piece & event(单选) 以及 multi(多选)
				// ctrl 为元件控制(复制、删除、等)，prop为元件信息
				piece: {
					_shell: null,
					ctrl: {
						idx: null,
						delete: null,
						copy: null,
					},
					prop: {
						ref_type: null,
						ref_idx: null,
						parallax: null,
						layer: null,
						pos_x: null,
						pos_y: null,
						scale_x: null,
						scale_y: null,
						rotate_angle: null,
						rotate_rate: null,
						rotate_flip: null
					}
				},
				event: {
					_shell: null,
					ctrl: {
						idx: null,
						delete: null,
						copy: null
					},
					prop: {
						pos_x: null,
						pos_y: null,
						type: null,
						id: null,
						name: null,
						parent: null,
						unlocked_from: null,
						visible_from: null,
						auto_visible: null,
						is_challenge: null,
						_gate_cost: null,
						_upgrade_type: null,
						_plant_type: null,
						_level_data: null,
						_level_type: null,
						_level_text: null,
						_level_narr_unlocked: null,
						_level_narr_completed: null,
						_level_tutorial: null,
						_level_tutorial_when: null,
						_level_toggle: null
					}
				},
				multi: {
					_shell: null,
					ctrl: {
						cnt: null,
						delete: null,
						copy: null,
						content: null
					}
				}
			},
			// 元件商店（右侧抽屉）
			item_shop: {
				_shell: null,
				// page
				page: {
					// 页面选择器
					_selector: null,
					// 三类元件
					piece_img: null,
					piece_anim: null,
					event: null
				}
			},
	
			// 舞台 - 地图元件
			item: {
				piece: [],
				event: [],
	
				appendPiece: function (e) {
					this.piece.push(e);
					return this;
				},
				removePiece: function (i) {
					this.piece.splice(i, 1)[0].remove();
					for (let j = i; j < this.piece.length; ++j)
						this.piece[j].attr('_i', j);
					return this;
				},
				appendEvent: function (e) {
					this.event.push(e);
					return this;
				},
				removeEvent: function (i) {
					this.event.splice(i, 1)[0].remove();
					for (let j = i; j < this.event.length; ++j)
						this.event[j].attr('_i', j);
					return this;
				}
			},
			// 舞台 - 地图层次
			layer: {
				// piece layer & event layer
				piece: null,
				event: null,
	
				// 获取map-stage上的元件元素到自身属性中
				getNodeMap: function () {
					this.piece = $('.map_layer[_t="0"]');
					this.event = $('.map_layer[_t="1"]');
					return this;
				},

				// 根据parallax获取piece集
				getPieceP: function (parallax) {
					return this.piece.filter(makeIntAttrSelector('_p', parallax));
				},
				// 根据layer获取piece集
				getPieceL: function (layer) {
					return this.piece.filter(makeIntAttrSelector('_l', layer));
				},
				// 根据parallax & layer获取piece集
				getPiecePL: function (parallax, layer) {
					return this.piece.filter(makeIntAttrSelector('_p', parallax) + makeIntAttrSelector('_l', layer));
				},
				// 根据parallax & layer & idx获取piece
				getPiecePLI: function (parallax, layer, idx) {
					return this.piece.filter(makeIntAttrSelector('_p', parallax) + makeIntAttrSelector('_l', layer)).children(makeIntAttrSelector('_i', idx));
				},
				// 根据idx获取piece
				getPiece: function (idx) {
					return this.piece.children(makeIntAttrSelector('_i', idx));
				},
				// 根据parallax获取event
				getEvent: function (idx) {
					return this.event.children(makeIntAttrSelector('_i', idx));
				}
			}
	
		},
	
		ctrl: {
			upload: null,
			download: null,
			linear_map: null,
			guide_line: null,
			res_select: null,
			drawer_toggle: {
				left: null,
				right: null
			},
			record: {
				show: null
			},
		},
	
		datalist: {
			_all: null,
			event_name: null,
			event_upgrade: null,
			event_plant: null,
			event_plant_only_cur_world: null,
			event_level_data_option: null,
			event_level_narr: null,
		},
	
		setEnumerable: function (o) {
			o = o || this;
			for (let k in o) {
				if ((Ty.isObj(o[k]) || Ty.isArr(o[k])) && o[k] != this.map.item_info.piece.prop && o[k] != this.map.item_info.event.prop)
					this.setEnumerable(o[k]);
				if (k[0] === '_')
					Object.defineProperty(o, k, { enumerable: false });
			}
		},
	
		bind: function () {
	
			this.setEnumerable();
			
			// drawer
			{
				this.drawer.left = $('#left-drawer');
				this.drawer.right = $('#right-drawer');
			}
			// record
			{
				this.record._shell = $('#record-shell');
	
				for (let k in this.record)
					this.record[k] = this.record._shell.find(`[_component='${k}']`);
			}
			// map
			// map-info
			{
				this.map.info._all = $('#map-info-shell [_prop]');
	
				for (let k in this.map.info)
					this.map.info[k] = this.map.info._all.filter(`[_prop='${k}']`);
			}
			// map-view stage
			{
				this.map.view.stage._shell = $('#map-view-stage-shell');
	
				for (let k in this.map.view.stage)
					this.map.view.stage[k] = this.map.view.stage._shell.find(`[_component='${k}']`);
			}
			// map-view ctrl
			{
				this.map.view.ctrl._all = $('#map-view-ctrl-shell [_prop]');
				
				for (let k in this.map.view.ctrl)
					this.map.view.ctrl[k] = this.map.view.ctrl._all.filter(`[_prop='${k}']`);
			}
			// map-view ctrl-text
			{
				this.map.view.ctrl_text._all = $('#map-view-ctrl-shell [_text]');
				
				for (let k in this.map.view.ctrl_text)
					this.map.view.ctrl_text[k] = this.map.view.ctrl_text._all.filter(`[_text='${k}']`);
			}
	
			// map item-info
			{
				this.map.item_info._shell = $('#map-iteminfo-shell');
	
				// piece, event, multi > ctrl, prop
				for (let k in this.map.item_info) {
					this.map.item_info[k]._shell = this.map.item_info._shell.find(`[_type=${k}]`);
	
					for (let kk in this.map.item_info[k])
						for (let kkk in this.map.item_info[k][kk])
							this.map.item_info[k][kk][kkk] = this.map.item_info[k]._shell.find(`[_${kk}=${kkk}]`);
				}
			}
			// map item-shop
			{
				this.map.item_shop._shell = $('#map-itemshop-shell');
	
				this.map.item_shop.page._selector = this.map.item_shop._shell.find('[_page-selector]');
	
				for (let k in this.map.item_shop.page)
					this.map.item_shop.page[k] = this.map.item_shop._shell.find(`[_page='${k}']`);
			}
	
			/*
			this.ctrl.upload = $('#ctrl-upload');
			this.ctrl.download = $('#ctrl-download');
			this.ctrl.about = $('#ctrl-about');
			this.ctrl.refresh = $('#ctrl-refresh');
			this.ctrl.clean = $('#ctrl-clean');
			this.ctrl.linear_map = $('#ctrl-linear_map');
			this.ctrl.guide_line = $('#ctrl-guide_line');
			this.ctrl.res_select = $('#ctrl-res_select');
			this.ctrl.drawer_toggle.left = $('#ctrl-drawer_toggle-left');
			this.ctrl.drawer_toggle.right = $('#ctrl-drawer_toggle-right');
	
			this._datalist = $('datalist[id^=_datalist_]');
			this.datalist.event_name = this._datalist.filter('#_datalist_event_name');
			this.datalist.event_upgrade = this._datalist.filter('#_datalist_event_upgrade');
			this.datalist.event_plant = this._datalist.filter('#_datalist_event_plant');
			this.datalist.event_plant_only_cur_world = this._datalist.filter('#_datalist_event_plant_only_cur_world');
			this.datalist.event_level_data_option = this._datalist.filter('#_datalist_event_level_data_option');
			this.datalist.event_level_narr = this._datalist.filter('#_datalist_event_level_narr');
			*/
		},
		bindEvent: function () {
	
			// record
			{
				this.record.clear.bind('click', function (e) {
					mdui.dialog({
						title: 'clear all record ?',
						buttons: [
							{ text: 'no' },
							{
								text: 'yes',
								onClick: function () { MapDesignerUI.record.clear(); }
							}
						]
					});
				});
				this.record.record.bind('click', function (e) {
					MapDesignerUI.record.record(undefined, undefined, undefined);
				});
				
				$([this.record.upload_raw[0], this.record.upload_ripe[0]]).bind('click', function (e) {
					let path = FS.selectSingleFile();
					if (path === null) {
						MDUtil.showTopSnackbar('please select map file', 4000, true);
						return;
					}
					let json = FS.readAsJSON(path);
					let name = 'upload ' + getTimeStr();
					let type = $(this).attr('_component').match(/(?=[_]).*$/)[0].substring(1);
					let data = null;
					switch (type) {
						case 'raw':
							data = json;
							break;
						case 'ripe':
							data = MapDataUtil.loadMapProto(json.objects[0].objdata);
							break;
					}
					MapDesignerUI.record.record(data, name, null).load(name);
					MDUtil.showTopSnackbar('load map finish', 4000, true);
				});
				$([this.record.download_raw[0], this.record.download_ripe[0]]).bind('click', function (e) {
					let type = $(this).attr('_component').match(/(?=[_]).*$/)[0].substring(1);
					saveResultToFile(type);
				});
	
				this.record.list.css('height', (MDUtil.getWindowHeightNoAppbar() - 118).toPxString());
			}
			// map
			// map-info
			{
				for (let k in this.map.info) {
					let dst = this.map.info[k];
					let func = null;
					switch (dst.attr('type')) {
						default: break;
						case 'text':
							func = function () {
								$M[$(this).attr('_d')] = this.value;
							}
							break;
						case 'number':
							func = function () {
								$M[$(this).attr('_d')] = this.value.toFloat();
							}
							break;
					}
					dst.bind('change', func);
				}
			}
			// map-view stage
			{
				this.map.view.stage._shell.css('height', (MDUtil.getWindowHeightNoAppbar() - this.map.view.stage._shell.next().outerHeight() - 156).toPxString());
			}
			// map-view ctrl
			{
				this.map.view.ctrl.pos_x.bind('change', function (e) {
					modMapDivState(null, this.value / 100 * $M.size_w, null, null, null);
				});
				this.map.view.ctrl.pos_y.bind('change', function (e) {
					modMapDivState(null, null, this.value / 100 * $M.size_h, null, null);
				});
				this.map.view.ctrl.scale.bind('change', function (e) {
					modMapDivState(this.value.toFloat(), null, null, null, null);
				});
				this.map.view.ctrl.layer.bind('change', function (e) {
					modMapDivState(null, null, null, this.value.toInt(), null);
				});
				this.map.view.ctrl.layer_focus.bind('click', function (e) {
					modMapDivState(null, null, null, MapDesignerUI.element.map.view.ctrl.layer.val().toInt(), null);
				});
				this.map.view.ctrl.layer_show.bind('click', function (e) {
					modMapDivState(null, null, null, true, null);
				});
				this.map.view.ctrl.layer_hide.bind('click', function (e) {
					modMapDivState(null, null, null, false, null);
				});
				this.map.view.ctrl.layer_event.bind('click', function (e) {
					modMapDivState(null, null, null, null, this.checked);
				});
				
				this.map.view.ctrl._all.filter('input').bind('input', function (e) { $(this).trigger('change'); });
			}
			// map item-info
			{
				// piece
				this.map.item_info.piece.ctrl.delete.bind('click', function (e) {
					MapDesignerUI.stageState.item_select.deleteTarget(MapDesignerUI.stageState.item_select.target[0][0], MapDesignerUI.stageState.item_select.target[0][1]);
				});
				this.map.item_info.piece.ctrl.copy.bind('mousedown', function (e) {
					e.preventDefault();
					let idx = $M.piece.length;
					let ofs = getRectPointAtLayerShell(e);
					let data = jsonCpy($M.piece[MapDesignerUI.stageState.item_select.target[0][1]]);
					data.pos_x = $M.pos_x + MapDesignerUI.stageState.pos_x * MapDataUtil.parallaxSpeed[data.parallax] + ofs[0];
					data.pos_y = $M.pos_y + MapDesignerUI.stageState.pos_y + ofs[1];
					$M.piece.push(data);
					appendItem(0, idx);
					MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(false).appendTarget(0, idx).setMove();
				});
	
				for (let k of ['ref_type', 'ref_idx', 'parallax', 'layer', 'pos_x', 'pos_y', 'scale_x', 'scale_y', 'rotate_rate', 'rotate_flip', 'rotate_angle']) {
					let modFunc = function (k, v) {
						if (!MapDesignerUI.stageState.item_select.isSelect()) return;
						let prop = {}; prop[k] = v;
						modPieceState(MapDesignerUI.stageState.item_select.target[0][1], prop, false, true);
					};
					let dst = this.map.item_info.piece.prop[k];
					if (dst.is('select')) {
						dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value); });
						dst.bind('input', function (e) { $(this).trigger('change'); });
					} else {
						switch (dst.attr('type')) {
							default: break;
							case 'checkbox':
								dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.checked); });
								break;
							case 'text':
								dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value); });
								dst.bind('input', function (e) { $(this).trigger('change'); });
								break;
							case 'number':
								if (dst.attr('step') == '1') {
									dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value.toInt()); });
								} else {
									dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value.toFloat()); });
								}
								dst.bind('input', function (e) { $(this).trigger('change'); });
								break;
						}
					}
				}
	
				// event
	
				this.map.item_info.event.ctrl.delete.bind('click', function (e) {
					MapDesignerUI.stageState.item_select.deleteTarget(MapDesignerUI.stageState.item_select.target[0][0], MapDesignerUI.stageState.item_select.target[0][1]);
				});
				this.map.item_info.event.ctrl.copy.bind('mousedown', function (e) {
					e.preventDefault();
					let idx = $M.event.length;
					let ofs = getRectPointAtLayerShell(e);
					let data = jsonCpy($M.event[MapDesignerUI.stageState.item_select.target[0][1]]);
					data.pos_x = $M.pos_x + MapDesignerUI.stageState.pos_x + ofs[0];
					data.pos_y = $M.pos_y + MapDesignerUI.stageState.pos_y + ofs[1];
					$M.event.push(data);
					appendItem(1, idx);
					MapDesignerUI.stageState.item_select.freeNodeSelect().free().setSelect(false).appendTarget(1, idx).setMove();
				});
		
				for (let k of ['pos_x', 'pos_y', 'id', 'type', 'name', 'parent', 'unlocked_from', 'visible_from', 'auto_visible', 'is_challenge', '_gate_cost', '_upgrade_type', '_plant_type', '_level_data', '_level_type', '_level_text', '_level_narr_unlocked', '_level_narr_completed', '_level_tutorial', '_level_tutorial_when', '_level_toggle']) {
					let modFunc = function (k, v) {
						if (!MapDesignerUI.stageState.item_select.isSelect()) return;
						let prop = {}; prop[k] = v;
						modEventState(MapDesignerUI.stageState.item_select.target[0][1], prop, false, true);
					};
					let dst = this.map.item_info.event.prop[k];
					if (dst.is('select')) {
						dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value); });
						dst.bind('input', function (e) { $(this).trigger('change'); });
					} else {
						switch (dst.attr('type')) {
							default: break;
							case 'checkbox':
								dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.checked); });
								break;
							case 'text':
								dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value); });
								dst.bind('input', function (e) { $(this).trigger('change'); });
								break;
							case 'number':
								if (dst.attr('step') == '1') {
									dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value.toInt()); });
								} else {
									dst.bind('change', function (e) { modFunc($(this).attr('_prop'), this.value.toFloat()); });
								}
								dst.bind('input', function (e) { $(this).trigger('change'); });
								break;
						}
					}
				}
	
				this.map.item_info.event.prop.id.bind('keydown', function (key) {
					if (key.ctrlKey && key.code == 'KeyQ') {
						$(this).val(MapDataUtil.getFreeEventID($M.event)).trigger('change');
					}
				});
				this.map.item_info.event.prop.name.bind('keydown', function (key) {
					if (key.ctrlKey && key.code == 'KeyQ') {
						let data = $M.event[MapDesignerUI.stageState.item_select.target[0][1]];
						let lastText = '';
						if (data.type === 'level') {
							lastText = data._level_text;
						} else if (data.type === 'plant' || data.type === 'plantbox') {
							lastText = data._plant_type;
						} else if (data.type === 'upgrade') {
							lastText = data._upgrade_type;
						}
						$(this).val($M.name + '/' + data.type + '/' + lastText).trigger('change');
					}
				});
				this.map.item_info.event.prop._plant_type.bind('keydown', function (key) {
					if (key.ctrlKey && key.code == 'KeyQ') {
						$(this).attr('list', (this.map.item_info.event.prop._plant_type.attr('list') == 'datalist-event-plant') ? 'datalist-event-plant-only-cur-world' : 'datalist-event-plant');
					}
				});
				this.map.item_info.event.prop._level_data.bind('keydown', function (key) {
					if (key.ctrlKey && key.code == 'KeyQ') {
						$(this).val($M.name + '/').trigger('change');
					}
				});
		
				this.map.item_info.event.prop._level_tutorial.append($('<option/>').text('').val('')).append(createOptionListWithValue(MapDesignerUI.cfg.option.MapTutorialState));
				this.map.item_info.event.prop._level_tutorial_when.append($('<option/>').text('').val('')).append(createOptionListWithValue(MapDesignerUI.cfg.option.WorldMapEventStatus));
		
				mdui.Select(this.map.item_info.event.prop._level_tutorial).handleUpdate();
				mdui.Select(this.map.item_info.event.prop._level_tutorial_when).handleUpdate();
	
				// multi
	
				this.map.item_info.event.ctrl.delete.bind('click', function (e) {
					let pieceTarget = [], eventTarget = [];
					for (let e of MapDesignerUI.stageState.item_select.target) {
						if (e[0] === 0) {
							pieceTarget.push(e[1]);
						} else {
							eventTarget.push(e[1]);
						}
					}
					for (let i = 0; i < pieceTarget.length; ++i) {
						for (let j = i + 1; j < pieceTarget.length; ++j) {
							if (pieceTarget[i] > pieceTarget[j]) {
								let t = pieceTarget[i];
								pieceTarget[i] = pieceTarget[j];
								pieceTarget[j] = t;
							}
						}
					}
					for (let i = 0; i < eventTarget.length; ++i) {
						for (let j = i + 1; j < eventTarget.length; ++j) {
							if (eventTarget[i] > eventTarget[j]) {
								let t = eventTarget[i];
								eventTarget[i] = eventTarget[j];
								eventTarget[j] = t;
							}
						}
					}
					for (let i = pieceTarget.length; i > 0; --i) {
						removeItem(0, pieceTarget[i - 1]);
					}
					for (let i = eventTarget.length; i > 0; --i) {
						removeItem(1, eventTarget[i - 1]);
					}
					MapDesignerUI.stageState.item_select.freeInfoDrawer().free();
				});
				this.map.item_info.event.ctrl.copy.bind('click', function (e) {
					MapDesignerUI.stageState.item_select.copyTargetJustCopy();
				});
		
			}
	
			mdui.updateTextFields();
			mdui.updateSliders();
			mdui.mutation();
	
			return;
	
			MapDesignerUI.element.ctrl.linear_map.bind({
				change: function () {
					gIsLinearMap = this.checked;
				}
			});
			MapDesignerUI.element.ctrl.guide_line.bind({
				change: function () {
					MapDesignerUI.element.map.guideline_shell[this.checked ? 'show' : 'hide']();
				}
			});
			MapDesignerUI.element.ctrl.res_select.bind({
				change: function () {
					gCurResID = this.value.toInt();
				}
			});
	
		}
	};
	
	// 地图记录
	static record = {

		// 记录的数据
		data: {},
		// 当前选定的记录ID
		selected_name: null,

		// 增加
		record: function (data, name, desc) {
			// 未提供data，则以当前map作为data
			data = data || $M;
			// 未提供name，则以当前时间作为name
			name = name || `auto ${getTimeStr()}`;
			// 未提供desc，则设为null
			desc = desc || null;
			// 添加记录到data中
			this.data[name] = { desc: desc, data: jsonCpy(data) };
			// 在UI中添加记录项
			// li的_n属性用以存储对应记录的ID
			MapDesignerUI.element.record.list.append($('<li/>').attr('_n', name).addClass('mdui-list-item mdui-row')
				// ID编辑
				.append($('<div/>').addClass('mdui-col-xs-5 mdui-textfield').append($('<input/>').addClass('mdui-textfield-input').attr('type', 'text').val(name).bind('change', function () {
					// 改变记录的ID
					let oldName = $(this).parent().parent().attr('_n');
					let newName = this.value;
					MapDesignerUI.record.data[newName] = MapDesignerUI.record.data[oldName];
					delete MapDesignerUI.record.data[oldName];
					$(this).parent().parent().attr('_n', newName);
				})))
				// 说明编辑
				.append($('<div/>').addClass('mdui-col-xs-6 mdui-textfield').append($('<input/>').addClass('mdui-textfield-input').attr('type', 'text').val(desc).bind('change', function () {
					MapDesignerUI.record.data[$(this).parent().parent().attr('_n')].desc = this.value;
				})))
				.append($('<div/>').addClass('mdui-col-xs-1')
					// 删除
					.append($('<button/>').addClass('mdui-btn mdui-btn-icon').append(MDUtil.makeIc('clear')).attr('mdui-tooltip', '{ content: "delete" }')
						.bind('click', function () {
							MapDesignerUI.record.selected_name = $(this).parent().parent().attr('_n');
							mdui.dialog({
								title: 'delete this record ?',
								buttons: [
									{ text: 'No' },
									{
										text: 'Yes',
										onClick: function () { MapDesignerUI.record.remove(MapDesignerUI.record.selected_name); }
									}
								]
							});
						})
					)
					// 加载
					.append($('<button/>').addClass('mdui-btn mdui-btn-icon').append(MDUtil.makeIc('replay')).attr('mdui-tooltip', '{ content: "load" }')
						.bind('click', function () {
							MapDesignerUI.record.selected_name = $(this).parent().parent().attr('_n');
							mdui.dialog({
								title: 'load this record ?',
								buttons: [
									{ text: 'No' },
									{
										text: 'Yes',
										onClick: function () { console.log(this); MapDesignerUI.record.load(MapDesignerUI.record.selected_name); }
									}
								]
							});
						})
					))
			);
			return this;
		},
		// 移除
		remove: function (name) {
			MapDesignerUI.element.record.list.children(`[_n='${name}']`).remove();
			delete this.data[name];
			return this;
		},
		// 加载
		load: function (name) {
			createMap(this.data[name].data);
			return this;
		},
		// 清空
		clear: function () {
			for (let k in this.data)
				this.remove(k);
			return this;
		}

	};


	static cleanMap() {

		this.element.map.info._all.val('').prop('disabled', true);

		this.element.map.view.ctrl._all.val('').prop('disabled', true);
		this.element.map.view.ctrl_text._all.text('');

		this.element.map.view.stage.layer_shell.empty();

		this.element.map.item_info._shell.hide();

		this.element.map.item_shop._shell.hide();
		this.element.map.item_shop.page.piece_img.empty();
		this.element.map.item_shop.page.piece_anim.empty();
		this.element.map.item_shop.page.event.empty();

		$('datalist').empty();
	
		$M = MapDataUtil.makeEmptyMap();
		MapDesignerUI.element.map.item.piece = [];
		MapDesignerUI.element.map.item.event = [];
		MapDesignerUI.element.map.layer.piece = [];
		MapDesignerUI.element.map.layer.event = [];
		MapDesignerUI.stageState.item_select.free();
	
		mdui.updateTextFields();
		mdui.updateSliders();
	};
	
	// 舞台状态
	static stageState = {

		// 位移、缩放
		pos_x: 0.0,
		pos_y: 0.0,
		scale: 1.0,

		// 视层控制
		layer: true,
		layer_event: true,

		// 元件选择
		item_select: {

			// 以itset表示状态
			// 0 : select 是否处于选中状态
			// 1 : move 是否处于移动状态
			// 2 : target 选中目标是单个还是多个
			state: 0,
			// 选定的元件
			target: [],

			// 处于选中状态
			isSelect: function () {
				return this.state & 0b001;
			},
			// 处于移动状态
			isMove: function () {
				return this.isSelect() && this.state & 0b010;
			},
			// 处于多选状态
			isMulti: function () {
				return this.isSelect() && this.state & 0b100;
			},
			// 处于单选状态
			isSingle: function () {
				return this.isSelect() && !this.isMulti();
			},

			// 释放选定元件
			free: function () {
				this.state = 0b000;
				this.target = [];
				return this;
			},
			// 启用选定状态，type表示是否为多选
			setSelect: function (isMulti) {
				this.state = 0b001;
				if (isMulti)
					this.state |= 0b100;
				return this;
			},

			// 启用移动状态
			setMove: function () {
				this.state |= 0b010;
				// 为选中的元件添加表示移动状态的_m类
				for (let e of this.target)
					MapDesignerUI.element.map.item[(e[0] === 0) ? 'piece' : 'event'][e[1]].attr('_m', '');
				return this;
			},
			// 清除移动状态
			freeMove: function () {
				this.state &= 0b101;
				// 为选中的元件清除表示移动状态的_m类
				for (let e of this.target)
					MapDesignerUI.element.map.item[(e[0] === 0) ? 'piece' : 'event'][e[1]].removeAttr('_m');
				return this;
			},

			// 加载左侧信息栏
			loadInfoDrawer: function () {
				// 先清空信息栏
				this.freeInfoDrawer();
				// 处于选中状态
				if (this.isSelect()) {
					if (this.isMulti()) {
						// 多选，显示所选元件表
						MapDesignerUI.element.map.item_info.multi._shell.show();
						MapDesignerUI.element.map.item_info.multi.ctrl.cnt.text(this.target.length.toString());
						MapDesignerUI.element.map.item_info.multi.ctrl.content.empty();
						for (let e of this.target)
							MapDesignerUI.element.map.item_info.multi.ctrl.content.append(createMultiInfoItem(e[0], e[1]));

					} else {
						// 单选，显示所选元件属性栏
						if (this.target[0][0] === 0) {
							// 所选元件为piece
							MapDesignerUI.element.map.item_info.piece._shell.show();
							MapDesignerUI.element.map.item_info.piece.ctrl.idx.text(this.target[0][1].toString());
							modPieceState(this.target[0][1], $M.piece[this.target[0][1]], false, true);
						} else {
							// 所选元件为event
							MapDesignerUI.element.map.item_info.event._shell.show();
							MapDesignerUI.element.map.item_info.event.ctrl.idx.text(this.target[0][1].toString());
							modEventState(this.target[0][1], $M.event[this.target[0][1]], false, true);
						}
					}
				}
				return this;
			},
			// 清空(隐藏)左侧信息栏
			freeInfoDrawer: function () {
				MapDesignerUI.element.map.item_info.piece._shell.hide();
				MapDesignerUI.element.map.item_info.event._shell.hide();
				MapDesignerUI.element.map.item_info.multi._shell.hide();
				return this;
			},

			// 所选元件中是否存在给定的元件，应在多选状态下使用
			existTarget: function (type, idx) {
				if (this.isMulti()) {
					for (let e of this.target) {
						if (e[0] === type && e[1] === idx)
							return true;
					}
				}
				return false;
			},
			// 在所选表中添加一个元件
			appendTarget: function (type, idx) {
				if (this.isSelect()) {
					if (this.isMulti()) {
						// 多选状态，则在target中添加
						if (!this.existTarget(type, idx)) {
							// 所给元件不存在于target中
							this.target.push([type, idx]);
							MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].attr('_s', '');
							this.loadInfoDrawer();
						}
					} else {
						// 单选状态，则替换target
						this.target = [[type, idx]];
						MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].attr('_s', '');
						this.loadInfoDrawer();
					}
				}
				return this;
			},
			// 在所选表中移除一个元件
			removeTarget: function (type, idx) {
				if (this.isSelect()) {
					if (this.isMulti()) {
						// 多选状态，则从target中移除
						for (let i in this.target) {
							// 查找对应所选元件
							if (this.target[i][0] === type && this.target[i][1] === idx) {
								MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].removeAttr('_s');
								this.target.splice(i, 1);
								// 判断当前选择情况
								if (this.target.length === 0) {
									this.freeInfoDrawer().free();
								} else {
									this.loadInfoDrawer();
								}
								break;
							}
						}
					} else {
						// 单选状态，且所给元件为当前所选元件，则释放所选元件
						if (this.target[0][0] === type && this.target[0][1] === idx) {
							MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].removeAttr('_s');
							this.target = [];
							this.freeInfoDrawer().free();
						}
					}
				}
				return this;
			},
			// 切换多选状态下的元件状态(选中|未选中)
			toggleTarget: function (type, idx) {
				if (this.isMulti()) {
					let exist = false;
					for (let i in this.target) {
						// 若存在，则在target中加入元件
						if (this.target[i][0] === type && this.target[i][1] === idx) {
							exist = true;
							MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].removeAttr('_s');
							this.target.splice(i, 1);
							if (this.target.length === 0) {
								this.freeInfoDrawer().free();
							} else {
								this.loadInfoDrawer();
							}
							break;
						}
					}
					// 否则移除元件
					if (!exist) {
						this.target.push([type, idx]);
						MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].attr('_s', '');
						this.loadInfoDrawer();
					}
				}
				return this;
			},
			// 从map中删除一个元件
			deleteTarget: function (type, idx) {
				if (this.isSelect()) {
					if (this.isMulti()) {
						for (let i in this.target) {
							if (this.target[i][0] === type && this.target[i][1] === idx) {
								MapDesignerUI.element.map.item[(type === 0) ? 'piece' : 'event'][idx].removeAttr('_s');
								this.target.splice(i, 1);
								for (let e of this.target)
									if (e[0] === type && e[1] > idx)
										e[1] -= 1;
								if (this.target.length === 0) {
									this.freeInfoDrawer().free();
								} else {
									this.loadInfoDrawer();
								}
								removeItem(type, idx);
								break;
							}
						}
					} else {
						removeItem(type, idx);
						this.target = [];
						this.freeInfoDrawer().free();
					}
				}
				return this;
			},
			// 设置target元素为selected状态
			setNodeSelect: function () {
				if (this.isSelect()) {
					for (let e of this.target)
						MapDesignerUI.element.map.item[(e[0] === 0) ? 'piece' : 'event'][e[1]].attr('_s', '');
				}
				return this;
			},
			// 清除target元素的selected状态
			freeNodeSelect: function () {
				if (this.isSelect()) {
					for (let e of this.target)
						MapDesignerUI.element.map.item[(e[0] === 0) ? 'piece' : 'event'][e[1]].removeAttr('_s');
				}
				return this;
			},
			// 在map中复制target
			copyTargetJustCopy: function () {
				let oriTarget = jsonCpy(this.target);
				this.freeNodeSelect().free().setSelect(true);
				let pieceIdx = $M.piece.length;
				let eventIdx = $M.event.length;
				for (let e of oriTarget) {
					if (e[0] === 0) {
						$M.piece.push(jsonCpy($M.piece[e[1]]));
						appendItem(0, pieceIdx);
						this.target.push([0, pieceIdx]);
						MapDesignerUI.element.map.item.piece[pieceIdx].attr('_s', '');
						++pieceIdx;
					} else {
						$M.event.push(jsonCpy($M.event[e[1]]));
						appendItem(1, eventIdx);
						this.target.push([1, eventIdx]);
						MapDesignerUI.element.map.item.event[eventIdx].attr('_s', '');
						++eventIdx;
					}
				}
				this.loadInfoDrawer();
			}
		}

	};

	// 是否构建范围选择框
	static isBuildRect = false;

	// 范围选择框的起点与终点
	static rectPoint = {

		beg: null, end: null,

		// 生成矩形
		toRect: function () {
			let x, y, w, h;
			let fp, sp; // 第一坐标（左&上），第二坐标（右|下）
			// 水平坐标
			[fp, sp] = (this.beg[0] < this.end[0]) ? [this.beg, this.end] : [this.end, this.beg];
			[x, w] = [fp[0], sp[0] - fp[0]]
			// 垂直坐标
			[fp, sp] = (this.beg[1] < this.end[1]) ? [this.beg, this.end] : [this.end, this.beg];
			[x, w] = [fp[1], sp[1] - fp[1]]
			return new Rect(x, y, w, h);
		}
	};

	// 获取相对于Layer外壳的坐标点
	static getRectPointAtLayerShell(e) {
		return [e.clientX - MapDesignerUI.element.map.layer_shell.offset().left, e.clientY - MapDesignerUI.element.map.layer_shell.offset().top];
	};

	// 步长对应表，TODO
	static stepMap = {
		angle: { little: 1, much: 10 },
		pos: { little: 1.0, much: 10.0 },
		scale: { little: 0.1, much: 1.0 },
	};

	// 显示关于对话框
	static showAboutDialog() {
		(new mdui.Dialog($('<div/>').addClass('mdui-dialog')
			.append($('<div/>').addClass('mdui-dialog-title').text('About'))
			.append($('<div/>').addClass('mdui-dialog-content')
				.append($('<p/>').text('Map-Designer. by smallpc'))
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
				.append($('<p/>').text('鸣谢: '))
				.append($('<div/>').addClass('mdui-typo')
					.append($('<a/>').attr('href', 'javascript:;').text('Justin Gdnz'))
					.append($('<span/>').text(' : 在组件位置属性的实现上给予帮助'))
				)
				.append($('<div/>').addClass('mdui-typo')
					.append($('<a/>').attr('target', '_blank').attr('href', 'https://space.bilibili.com/35521848').text('MF'))
					.append($('<span/>').text(' : 提供部分元件属性的作用、提供PvZ2风格字体'))
				)
				.append($('<div/>').addClass('mdui-typo')
					.append($('<a/>').attr('target', '_blank').attr('href', 'https://space.bilibili.com/183409126').text('CZ'))
					.append($('<span/>').text(' : 提供部分元件属性的作用'))
				)
				.append($('<div/>').addClass('mdui-typo')
					.append($('<a/>').attr('target', '_blank').attr('href', 'https://space.bilibili.com/210379574').text('Beast'))
					.append($('<span/>').text(' : 提供PvZ2风格字体'))
				)
				.append($('<p/>').text('使用的框架: '))
				.append($('<div/>').addClass('mdui-typo')
					.append($('<a/>').attr('target', '_blank').attr('href', 'https://www.mdui.org/').text('MDUI'))
					.append($('<span/>').text(' : 一套用于开发 Material Design 网页的前端框架'))
				)
			)
			.append($('<div/>').addClass('mdui-dialog-actions')
				.append($('<button/>').addClass('mdui-btn mdui-ripple').attr('mdui-dialog-close', '').text('close'))
			))).open();
	}
	
	// UI初始化
	static init() {
		
		$('#record-shell').css('height', (MDUtil.getWindowHeightNoAppbar() - $('#record-shell').next().outerHeight() - 156).toPxString());

		// 关于
		$('#about').bind('click', function () { showAboutDialog(); });
		// 刷新
		$('#refresh').bind('click', function () { DocUtil.reload(); });
		// 清除
		$('#clean').bind('click', function () {
			cleanMap();
			$M = MapDataUtil.makeEmptyMap();
		});

		// 鼠标按下
		window.onmousedown = function (e) {
			if (e.button === 2) {
				// 点击右键：释放所选元件与左侧信息栏
				MapDesignerUI.stageState.item_select.freeInfoDrawer().freeNodeSelect().free();
			} else if (e.ctrlKey && $(e.target).attr('_a') === 'map' && $(e.target).attr('_b') === 'layer_shell') {
				// CTRL且位于MAP视图中
				// 处于构建范围框状态
				MapDesignerUI.isBuildRect = true;
				// 点击右键：释放所选元件
				MapDesignerUI.stageState.item_select.freeNodeSelect().free();
				// 获取起点坐标
				MapDesignerUI.rectPoint.beg = MapDesignerUI.getRectPointAtLayerShell(e);
			}
		}
		// 鼠标松开
		window.onmouseup = function (e) {
			if (MapDesignerUI.isBuildRect) {
				// 取消构建范围框状态
				MapDesignerUI.isBuildRect = false;
				// 获取终点坐标
				MapDesignerUI.rectPoint.end = MapDesignerUI.getRectPointAtLayerShell(e);
				// 构建矩形
				let rect = MapDesignerUI.rectPoint.toRect();
				// 偏移至实际MAP坐标
				rect.offset(MapDesignerUI.stageState.pos_x + $M.pos_x, MapDesignerUI.stageState.pos_y + $M.pos_y);
				// 隐藏并重置范围选择框
				MapDesignerUI.element.map.select_rect.removeAttr('_s');
				MapDesignerUI.element.map.select_rect.css({ left: 0, top: 0, width: 0, height: 0 });
				// ？？？
				MapDesignerUI.stageState.item_select.setSelect(true);
				if (MapDesignerUI.stageState.layer_piece === true) {
					// 所有元件层启用
					// 选择所有位于矩形范围内的元件
					for (let i in $M.piece) {
						if ($M.piece[i].parallax == 0 && rect.btw($M.piece[i].pos_x, $M.piece[i].pos_y)) {
							MapDesignerUI.stageState.item_select.target.push([0, i.toInt()]);
						}
					}
				} else if (Ty.isNum(MapDesignerUI.stageState.layer_piece)) {
					// 单独元件层启用
					// 选择所有位于矩形范围内的元件
					for (let i in $M.piece) {
						if ($M.piece[i].parallax == 0 && $M.piece[i].layer === MapDesignerUI.stageState.layer_piece && rect.btw($M.piece[i].pos_x, $M.piece[i].pos_y)) {
							MapDesignerUI.stageState.item_select.target.push([0, i.toInt()]);
						}
					}
				}
				if (MapDesignerUI.stageState.layer_event) {
					// 事件层启用
					// 选择所有位于矩形范围内的事件
					for (let i in $M.event) {
						if (rect.btw($M.event[i].pos_x, $M.event[i].pos_y)) {
							MapDesignerUI.stageState.item_select.target.push([1, i.toInt()]);
						}
					}
				}
				// 若范围内未选择到有效元件，则重置选择信息
				if (MapDesignerUI.stageState.item_select.target.length === 0)
					MapDesignerUI.stageState.item_select.free();
				else // 否则，选中对应元件node，并显示信息栏
					MapDesignerUI.stageState.item_select.setNodeSelect().loadInfoDrawer();
			}
		}
		// 鼠标移动
		window.onmousemove = function (e) {
			if (MapDesignerUI.stageState.item_select.isMove()) {
				let isReSetInfoDrawer = !MapDesignerUI.stageState.item_select.isMulti();
				let prop = {
					pos_x: e.movementX / MapDesignerUI.stageState.scale,
					pos_y: e.movementY / MapDesignerUI.stageState.scale
				};
				for (let e of MapDesignerUI.stageState.item_select.target) {
					if (e[0] === 0) {
						modPieceState(e[1], prop, true, isReSetInfoDrawer);
					} else if (e[0] === 1) {
						modEventState(e[1], prop, true, isReSetInfoDrawer);
					}
				}
			}
			if (MapDesignerUI.isBuildRect) {
				MapDesignerUI.rectPoint.end = MapDesignerUI.getRectPointAtLayerShell(e);
				let rect = MapDesignerUI.rectPoint.toRect();
				MapDesignerUI.element.map.select_rect.css({
					left: rect.x.toPxString(),
					top: rect.y.toPxString(),
					width: rect.w.toPxString(),
					height: rect.h.toPxString()
				});
				MapDesignerUI.element.map.select_rect.attr('_s', '');
			}
		}
		// 鼠标滚轮
		window.onmousewheel = function (e) {
			if ($(e.target).attr('_a') === 'map' && $(e.target).attr('_b') === 'layer_shell') {
				if (e.altKey) {
					if (e.ctrlKey) {
						let fresh = MapDesignerUI.element.ctrl.map.view.pos_y.val().toFloat() + e.wheelDelta / 120.0 * 2.0;
						if (fresh < 0.0)
							fresh = 0.0;
						else if (fresh > 100.0)
							fresh = 100.0;
						modMapDivState(null, null, fresh / 100 * $M.size_h, null, null);
					} else {
						let fresh = MapDesignerUI.element.ctrl.map.view.pos_x.val().toFloat() + e.wheelDelta / 120.0 * 2.0;
						if (fresh < 0.0)
							fresh = 0.0;
						else if (fresh > 100.0)
							fresh = 100.0;
						modMapDivState(null, fresh / 100 * $M.size_w, null, null, null);
					}
				} else if (e.ctrlKey) {
					let fresh = MapDesignerUI.stageState.scale + e.wheelDelta / 120.0 / 10;
					if (fresh < 0.0)
						fresh = 0.0;
					else if (fresh > 10.0)
						fresh = 10.0;
					modMapDivState(fresh, null, null, null, null);
				}
			}
		}
		// 快捷键设置
		document.onkeydown = function (key) {
			if (document.activeElement.localName != 'input' && document.activeElement.localName != 'select') {
				if (key.ctrlKey) {
					switch (key.code) {
						default: break;
						case 'BracketLeft':
							mdui.Drawer(MapDesignerUI.element.drawer.left).toggle();
							break;
						case 'BracketRight':
							mdui.Drawer(MapDesignerUI.element.drawer.right).toggle();
							break;
						case 'ArrowLeft':
							mdui.Tab('#page_selector').prev();
							break;
						case 'ArrowRight':
							mdui.Tab('#page_selector').next();
							break;
						case 'KeyA':
							mdui.Tab('#_map_item_shop_select').prev();
							break;
						case 'KeyD':
							mdui.Tab('#_map_item_shop_select').next();
							break;
					}
				}
				if (MapDesignerUI.stageState.item_select.isSelect()) {
					if (!MapDesignerUI.stageState.item_select.isMulti() && key.code === 'Tab') {
						let type = MapDesignerUI.stageState.item_select.target[0][0];
						let idx = MapDesignerUI.stageState.item_select.target[0][1];
						if (key.ctrlKey) {
							if (idx > 0)
								MapDesignerUI.stageState.item_select.freeNodeSelect().appendTarget(type, idx - 1);
						} else if (idx < $M[(type === 0) ? 'piece' : 'event'].length) {
							MapDesignerUI.stageState.item_select.freeNodeSelect().appendTarget(type, idx + 1);
						}
						return;
					}
					let ck = null, cv = null;
					switch (key.code) {
						default: break;
						case 'KeyZ': // idx change
							ck = 'ref_idx';
							cv = key.shiftKey ? -1 : +1;
							break;
						case 'KeyA': // pos left
							ck = 'pos_x';
							cv = key.shiftKey ? -10.0 : -1.0;
							break;
						case 'KeyD': // pos right
							ck = 'pos_x';
							cv = key.shiftKey ? +10.0 : +1.0;
							break;
						case 'KeyW': // pos up
							ck = 'pos_y';
							cv = key.shiftKey ? -10.0 : -1.0;
							break;
						case 'KeyS': // pos down
							ck = 'pos_y';
							cv = key.shiftKey ? +10.0 : +1.0;
							break;
						case 'KeyQ': // rotate clockwise or flipped
							if (key.altKey) {
								ck = 'rotate_flip';
								cv = !$M.piece[MapDesignerUI.stageState.item_select.target[0][1]].rotate_flip;
							}
							else {
								ck = 'rotate_angle';
								cv = key.shiftKey ? +10 : +1;
							}
							break;
						case 'KeyE': // rotate counterclockwise or flipped
							if (key.altKey) {
								ck = 'rotate_flip';
								cv = !$M.piece[MapDesignerUI.stageState.item_select.target[0][1]].rotate_flip;
							}
							else {
								ck = 'rotate_angle';
								cv = key.shiftKey ? -10 : -1;
							}
							break;
						case 'KeyX': // scaly-x -
							ck = 'scale_x';
							cv = key.shiftKey ? -1.0 : -0.1;
							break;
						case 'KeyC': // scaly-x +
							ck = 'scale_x';
							cv = key.shiftKey ? +1.0 : +0.1;
							break;
						case 'KeyR': // scaly-y -
							ck = 'scale_y';
							cv = key.shiftKey ? -1.0 : -0.1;
							break;
						case 'KeyF': // scaly-y +
							ck = 'scale_y';
							cv = key.shiftKey ? +1.0 : +0.1;
							break;
						case 'KeyV': // layer down
							ck = key.shiftKey ? 'parallax' : 'layer';
							cv = -1;
							break;
						case 'KeyB': // layer up
							ck = key.shiftKey ? 'parallax' : 'layer';
							cv = +1;
							break;
					}
					if (ck !== null) {
						let cm = {};
						cm[ck] = cv;
						for (let e of MapDesignerUI.stageState.item_select.target) {
							if (e[0] === 0) {
								modPieceState(e[1], cm, ck != 'rotate_flip', true);
							} else {
								if (ck == 'pos_x' || ck == 'pos_y')
									modEventState(e[1], cm, true, true);
							}
						}
					}
				}
			}
		}

		// 加载配置信息
		if (!FS.exist(this.path.cfg_file)) {
			MDUtil.showTopSnackbarForError('config file not found');
			return;
		}
		this.cfg = FS.readAsJSON(this.path.cfg_file);

		{
			// 加载fbUsv8C5eI字体文件
			$('head').append($('<style/>').html(`@font-face { font-family: fbUsv8C5eI; src: url(${this.path.font_file}); }`));

			this.element.bind();
			this.element.bindEvent();

			this.cleanMap();

			for (let e of this.cfg.setting.guideLine) {
				let line = $('<div/>').attr({ _direction: e.direction }).css({ left: '0', top: '0' });
				switch (e.direction) {
					case 'horizontal': line.css('top', e.position); break;
					case 'vertical': line.css('left', e.position); break;
				}
				this.element.map.view.stage.guideline_shell.append(line);
			}
			//this.element.ctrl.linear_map.prop('checked', this.cfg.setting.linearMap).trigger('change');
			//this.element.ctrl.guide_line.prop('checked', this.cfg.setting.showGuideLine).trigger('change');
			//this.element.ctrl.res_select.val(this.cfg.setting.res).trigger('change');
			mdui.Select(this.element.ctrl.res_select).handleUpdate();
			loadAnim(this.cfg.world);
		}

		MDUtil.showTopSnackbar('load config finish', 3000, true);

	}

}


// 直线地图
var gIsLinearMap = true;

// 废弃event属性
var gDeprecatedEventPropMap = { 'is_challenge': '1.7' };

// 特殊event属性所对应类型
let gSpecialEventPropMap = {
	_gate_cost: ['star_gate', 'key_gate'],
	_upgrade_type: ['upgrade'],
	_plant_type: ['plant', 'plantbox'],
	_level_data: ['level'],
	_level_type: ['level'],
	_level_text: ['level'],
	_level_narr_unlocked: ['level'],
	_level_narr_completed: ['level'],
	_level_tutorial: ['level'],
	_level_tutorial_when: ['level'],
	_level_toggle: ['level']
}

function readAnimJSON(path, name, imgDir, range) {
	let fs = require('fs');
	fs.readFile(path, 'utf8', function (err, data) {
		if (err)
			return console.log(err);

		//console.log(name);

		let anim = JSON.parse(data);

		gAnimOriginList[name] = jsonCpy(anim.origin);
		let animData = PvZ2Anim.makeAnim2(name, anim, PvZ2Anim.getFrameRange(anim, range, MapDesignerUI.cfg.setting.forceSingleFrame), [], imgDir);
		gAnimDivInnerList[name] = animData[1];
		$('head').append($('<style/>').html(animData[0]));
	});
}

function loadAnim(worldList) {
	//readAnimJSON(genUrl('item-event-level-node-ripe-json', ['normal']), 'sprout/idle', genUrl('item-event-level-node-img-dir', ['normal']), 'locked_idle');
	readAnimJSON(genUrl('just-prefix', []) + 'worldmap/giftbox_world_map/giftbox_world_map.css.json', 'giftbox/idle', genUrl('just-prefix', []) + 'worldmap/giftbox_world_map/', 'idle');
	readAnimJSON(genUrl('just-prefix', []) + 'worldmap/giftbox_world_map/giftbox_world_map.css.json', 'giftbox/open_idle', genUrl('just-prefix', []) + 'worldmap/giftbox_world_map/', 'open_idle');
	readAnimJSON(genUrl('item-event-level-node-ripe-json', ['normal']), 'level_node/normal', genUrl('item-event-level-node-img-dir', ['normal']), 'locked_idle');
	readAnimJSON(genUrl('item-event-level-node-ripe-json', ['minigame']), 'level_node/minigame', genUrl('item-event-level-node-img-dir', ['minigame']), 'locked_idle');
	readAnimJSON(genUrl('item-event-level-node-ripe-json', ['miniboss']), 'level_node/miniboss', genUrl('item-event-level-node-img-dir', ['miniboss']), 'locked_idle');
	readAnimJSON(genUrl('item-event-level-node-ripe-json', ['miniboss']), 'level_node/nonfinalboss', genUrl('item-event-level-node-img-dir', ['miniboss']), 'locked_idle');
	readAnimJSON(genUrl('item-event-level-node-ripe-json', ['zomboss_hologram']), 'level_node/boss_hologram', genUrl('item-event-level-node-img-dir', ['zomboss_hologram']), 'idle');
	for (let world in worldList) {
		readAnimJSON(genUrl('item-event-level-node-ripe-json', ['boss', world]), 'level_node/boss/' + world, genUrl('item-event-level-node-img-dir', ['boss', world]), [0, 0]);
		readAnimJSON(genUrl('item-event-level-node-ripe-json', ['danger', world]), 'level_node/danger/' + world, genUrl('item-event-level-node-img-dir', ['danger', world]), 'locked_idle');
		for (let anim of worldList[world].anim) {
			readAnimJSON(genUrl('item-piece-anim-ripe-json', [world, anim]), 'world/' + world + '/anim' + anim.toString(), genUrl('item-piece-anim-img-dir', [world, anim]), 'idle');
		}
	}
}