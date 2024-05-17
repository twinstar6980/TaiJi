
function sleep(delay) {
	for(let t = Date.now(); Date.now() - t <= delay;)
		;
}

class DocUtil {

	static reload() {
		window.location.reload();
	}

	static makeOpt(text, val) {
		let result = $('<option/>').text(text);
		if (Ty.isStr(val))
			result.val(val);
		return result;
	}
	static makeDataList(id, child) {
		let result = $('<datalist/>').attr('id', id);
		if (Ty.isArr(child)) {
			for (let e of child) {
				result.append(e);
			}
		}
		return result;
	}

}

class MDUtil {

	static setColorSettingDialog() {
		$('#dialog-docs-theme').replaceWith($('\
			<div class=\'mdui-dialog\' id=\'dialog-docs-theme\'>\
				<div class=\'mdui-dialog-title\'>Color</div>\
				<div class=\'mdui-dialog-content\'>\
					<p class=\'mdui-typo-title\'>Theme</p>\
					<div class=\'mdui-row-xs-1 mdui-row-sm-2 mdui-row-md-3\'>\
						<div class=\'mdui-col\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-layout\' value=\'\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Light\
							</label>\
						</div>\
						<div class=\'mdui-col\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-layout\' value=\'dark\' checked />\
								<i class=\'mdui-radio-icon\'></i>\
								Dark\
							</label>\
						</div>\
					</div>\
					<p class=\'mdui-typo-title mdui-text-color-theme\'>Primary</p>\
					<form class=\'mdui-row-xs-1 mdui-row-sm-2 mdui-row-md-3\'>\
						<div class=\'mdui-col mdui-text-color-amber\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'amber\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Amber\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-blue\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'blue\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Blue\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-blue-grey\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'blue-grey\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Blue Grey\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-brown\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'brown\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Brown\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-cyan\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'cyan\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Cyan\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-deep-orange\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'deep-orange\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Deep Orange\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-deep-purple\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'deep-purple\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Deep Purple\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-green\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'green\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Green\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-grey\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'grey\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Grey\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-indigo\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'indigo\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Indigo\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-light-blue\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'light-blue\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Light Blue\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-light-green\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'light-green\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Light Green\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-lime\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'lime\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Lime\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-orange\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'orange\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Orange\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-pink\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'pink\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Pink\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-purple\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'purple\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Purple\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-red\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'red\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Red\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-teal\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'teal\' checked />\
								<i class=\'mdui-radio-icon\'></i>\
								Teal\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-yellow\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-primary\' value=\'yellow\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Yellow\
							</label>\
						</div>\
					</form>\
					<p class=\'mdui-typo-title mdui-text-color-theme-accent\'>Accent</p>\
					<form class=\'mdui-row-xs-1 mdui-row-sm-2 mdui-row-md-3\'>\
						<div class=\'mdui-col mdui-text-color-amber\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'amber\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Amber\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-blue\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'blue\' checked />\
								<i class=\'mdui-radio-icon\'></i>\
								Blue\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-cyan\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'cyan\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Cyan\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-deep-orange\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'deep-orange\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Deep Orange\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-deep-purple\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'deep-purple\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Deep Purple\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-green\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'green\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Green\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-indigo\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'indigo\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Indigo\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-light-blue\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'light-blue\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Light Blue\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-light-green\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'light-green\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Light Green\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-lime\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'lime\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Lime\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-orange\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'orange\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Orange\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-pink\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'pink\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Pink\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-purple\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'purple\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Purple\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-red\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'red\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Red\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-teal\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'teal\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Teal\
							</label>\
						</div>\
						<div class=\'mdui-col mdui-text-color-yellow\'>\
							<label class=\'mdui-radio mdui-m-b-1\'>\
								<input type=\'radio\' name=\'doc-theme-accent\' value=\'yellow\' />\
								<i class=\'mdui-radio-icon\'></i>\
								Yellow\
							</label>\
						</div>\
					</form>\
				</div>\
				<div class=\'mdui-divider\'></div>\
				<div class=\'mdui-dialog-actions\'>\
					<button class=\'mdui-btn mdui-ripple mdui-float-left\' mdui-dialog-cancel>Default</button>\
					<button class=\'mdui-btn mdui-ripple\' mdui-dialog-confirm>OK</button>\
				</div>\
			</div>\
		'));
	}
	static setColorSettingEvent() {
		let $$ = mdui.JQ;
	
		let DEFAULT_PRIMARY = 'teal';
		let DEFAULT_ACCENT = 'blue';
		let DEFAULT_LAYOUT = 'dark';

		// 设置 cookie
		let setCookie = function (key, value) {
			// cookie 有效期为 1 年
			let date = new Date();
			date.setTime(date.getTime() + 365 * 24 * 3600 * 1000);
			document.cookie = key + '=' + value + '; expires=' + date.toGMTString() + '; path=/';
		};

		let setDocsTheme = function (theme) {
			if (typeof theme.primary === 'undefined') {
				theme.primary = false;
			}
			if (typeof theme.accent === 'undefined') {
				theme.accent = false;
			}
			if (typeof theme.layout === 'undefined') {
				theme.layout = false;
			}

			let i, len;
			let $body = $$('body');

			let classStr = $body.attr('class');
			let classs = classStr.split(' ');

			// 设置主色
			if (theme.primary !== false) {
				for (i = 0, len = classs.length; i < len; i++) {
					if (classs[i].indexOf('mdui-theme-primary-') === 0) {
						$body.removeClass(classs[i])
					}
				}
				$body.addClass('mdui-theme-primary-' + theme.primary);
				setCookie('docs-theme-primary', theme.primary);
				$$('input[name="doc-theme-primary"][value="' + theme.primary + '"]').prop('checked', true);
			}

			// 设置强调色
			if (theme.accent !== false) {
				for (i = 0, len = classs.length; i < len; i++) {
					if (classs[i].indexOf('mdui-theme-accent-') === 0) {
						$body.removeClass(classs[i]);
					}
				}
				$body.addClass('mdui-theme-accent-' + theme.accent);
				setCookie('docs-theme-accent', theme.accent);
				$$('input[name="doc-theme-accent"][value="' + theme.accent + '"]').prop('checked', true);
			}

			// 设置主题色
			if (theme.layout !== false) {
				for (i = 0, len = classs.length; i < len; i++) {
					if (classs[i].indexOf('mdui-theme-layout-') === 0) {
						$body.removeClass(classs[i]);
					}
				}
				if (theme.layout !== '') {
					$body.addClass('mdui-theme-layout-' + theme.layout);
				}
				setCookie('docs-theme-layout', theme.layout);
				$$('input[name="doc-theme-layout"][value="' + theme.layout + '"]').prop('checked', true);
			}
		};

		// 切换主色
		$$(document).on('change', 'input[name="doc-theme-primary"]', function () {
			setDocsTheme({
				primary: $$(this).val()
			});
		});

		// 切换强调色
		$$(document).on('change', 'input[name="doc-theme-accent"]', function () {
			setDocsTheme({
				accent: $$(this).val()
			});
		});

		// 切换主题色
		$$(document).on('change', 'input[name="doc-theme-layout"]', function () {
			setDocsTheme({
				layout: $$(this).val()
			});
		});

		// 恢复默认主题
		$$(document).on('cancel.mdui.dialog', '#dialog-docs-theme', function () {
			setDocsTheme({
				primary: DEFAULT_PRIMARY,
				accent: DEFAULT_ACCENT,
				layout: DEFAULT_LAYOUT
			});
		});
	};

	static getWindowHeightNoAppbar() {
		return $(window).height() - $('.mdui-appbar').outerHeight();
	}

	static makeIc(icon) {
		return $('<i/>').addClass('mdui-icon material-icons').text(icon);
	}
	static makeBtn(msg, click) {
		return $('<button/>').addClass('mdui-btn mdui-ripple').text(msg).bind('click', click);
	}
	static makeDenseBtn(msg, click) {
		return this.makeBtn(msg, click).addClass('mdui-btn-dense');
	}
	static makeIcBtn(icon, click) {
		return $('<button/>').addClass('mdui-btn mdui-btn-icon mdui-ripple').bind('click', click).append(this.makeIc(icon));
	}
	static makeDenseIcBtn(icon, click) {
		return this.makeIcBtn(icon, click).addClass('mdui-btn-dense');
	}

	static makeCheckBox(attr, click, state) {
		let input = $('<input/>').attr('type', 'checkbox').prop('checked', state === true);
		if (Ty.isObj(attr)) {
			input.attr(attr);
		}
		if (click !== null) {
			input.bind('click', click);
		}
		return $('<label/>').addClass('mdui-checkbox')
			.append(input)
			.append($('<i/>').addClass('mdui-checkbox-icon'));
	}
	static makeRadio(attr, click, name, state) {
		let input = $('<input/>').attr('type', 'radio').attr('name', name).prop('checked', state === true);
		if (Ty.isObj(attr)) {
			input.attr(attr);
		}
		if (click !== null) {
			input.bind('click', click);
		}
		return $('<label/>').addClass('mdui-radio')
			.append(input)
			.append($('<i/>').addClass('mdui-radio-icon'));
	}

	static showEasyDialog(title, content) {
		mdui.dialog({
			title: title,
			content: content,
			buttons: [
				{
					text: "Close",
					close: true
				}
			]
		});
	}
	static showTopSnackbar(msg, timeout, closeOnOutsideClick) {
		mdui.snackbar({
			message: msg,
			position: 'top',
			timeout: timeout,
			closeOnOutsideClick: closeOnOutsideClick
		});
	}
	static showTopSnackbarForError(msg) {
		this.showTopSnackbar(msg, 1000000, false);
	}
	
}

class FS {

	static fs = require('fs');

	static select(option) {
		return require('electron').remote.dialog.showOpenDialogSync({ properties: option });
	}
	static selectSingleFile() {
		let list = this.select(['openFile', 'showHiddenFiles']);
		if (list === undefined)
			return null;
		return list[0];
	}
	static selectMultiFile() {
		let list = this.select(['openFile', 'showHiddenFiles', 'multiSelections']);
		if (list === undefined)
			return null;
		return list;
	}

	static exist(path) {
		return this.fs.existsSync(path);
	}
	static read(path) {
		if (!this.exist(path))
			return undefined;
		return this.fs.readFileSync(path, 'utf8').toString();
	}
	static readAsJSON(path) {
		let data = this.read(path);
		if (data === undefined)
			return undefined;
		return JSON.parse(data);
	}

	static write(path, string) {
		this.fs.writeFileSync(path, string);
	}
	static writeJSON(path, json) {
		this.fs.writeFileSync(path, JSON.stringify(json));
	}

	static listDir(path) {
		let r = [];
		let list = this.fs.readdirSync(path, { withFileTypes: true });
		for (let e of list) {
			if (e.isDirectory())
				r.push(e.name);
		}
		return r;
	}

	static saveFile(str, type, name) {
		saveAs(new Blob([str], { type: type }), name);
	}
	static saveJSONFile(json, name) {
		saveAs(new Blob([JSON.stringify(json, null, '\t')], { type: "application/json" }), name + ".json");
	}
	
}

function isBtwRange(v, range) {
	return v >= range[0] && v <= range[1];
}

function cntChAtStr(s, c) {
	let cnt = 0;
	for (let ch of s)
		if (ch == c)
			++cnt;
	return cnt;
}

function getStrIfExist(v) {
	return (Ty.isUndef(v) || Ty.isNull(v)) ? "" : v;
}

function createNode(type, attrList, listenerList, innerText, childList) {
	let node = document.createElement(type);
	if (isObj(attrList))
		for (let key in attrList)
			if (!isUndef(attrList[key]))
				node.setAttribute(key, attrList[key]);
	if (isObj(listenerList))
		for (let key in listenerList)
			node.addEventListener(key, listenerList[key]);
	if (!Ty.isNull(innerText))
		node.innerText = innerText;
	if (isArr(childList))
		for (let child of childList)
			node.appendChild(child);
	return node;
}

function addNodeMaybeExist(parentID, idIfExist, node) {
	if (Ty.isNull(getHEleByID(idIfExist)))
		getHEleByID(parentID).appendChild(node);
	else
		getHEleByID(parentID).replaceChild(node, getHEleByID(idIfExist));
}
function removeNodeMaybeExist(parentID, id) {
	if (!Ty.isNull(getHEleByID(id)))
		getHEleByID(parentID).removeChild(getHEleByID(id));
}

function setDisableFromParent(parent, disableOrNot) {
	for (let node of parent.getElementsByTagName("input"))
		node.disabled = disableOrNot;
	for (let node of parent.getElementsByTagName("button"))
		node.disabled = disableOrNot;
	for (let node of parent.getElementsByTagName("select"))
		node.disabled = disableOrNot;
}

$(function () {
	MDUtil.setColorSettingDialog();
	MDUtil.setColorSettingEvent();
});
