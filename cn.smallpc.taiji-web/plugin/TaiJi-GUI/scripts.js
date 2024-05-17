class TaiJiGUI {

	static childProcess = require('child_process');

	static path = {
		exe: 'D:\\MyProgram\\cn.smallpc.taiji\\app.windows.loader\\output\\x64-Release\\app.windows.loader.exe',
		cfg: 'D:\\太極\\cfg.json',
		gui_cfg: 'D:\\太極\\gui-cfg.json',
	}

	static tj = {

		process: { killed: true },
		cfg: null,
		gui_cfg: null,

		setCtrl: function (state) {
			$('#console-stdin').prop('disabled', !state);
			$('#console-exit').prop('disabled', !state);
			mdui.updateTextFields($('#console-stdin').parent());
			mdui.mutation();
			return this;
		},
		clearPage: function () {
			$('#console-page').empty();
			return this;
		},

		clean: function () {
			this.kill(); this.clearPage().setCtrl(false);
			return this;
		},
		start: function (cmd) {
			this.clean().setCtrl(true);

			this.process = TaiJiGUI.childProcess.execFile(TaiJiGUI.path.exe, cmd);
			
			this.process.stdout.on('data', (data) => {
				console.log(data);
				$('#console-page').append($('<span/>').addClass('mdui-ripple').text(data).bind('click', function () {
					let s = $(this).text();
				})).scrollTop(2147483647);
			});
			this.process.stderr.on('data', (data) => {
				$('#console-page').append($('<span/>').addClass('mdui-ripple').text('stderr : ' + data)).scrollTop(2147483647);
			});

			this.process.on('close', function (code) {
				$('#console-page').append($('<span/>').addClass('mdui-ripple').text('CLOSE : ' + code)).scrollTop(2147483647);
				TaiJiGUI.tj.setCtrl(false);
			});

			return this;
		},
		kill: function () {
			if (!this.process.killed) {
				this.process.kill();
				while (!this.process.killed)
					;
			}
			return this;
		},
		input: function (s) {
			this.process.stdin.write(s);
			$('#console-page').append($('<span/>').addClass('mdui-ripple').text(s)).scrollTop(2147483647);
			return this;
		}
	}

	static curScript = null;

	static makeSimpleScriptGenUI() {
		$('#simple-script-run').bind('click', function () {
			let script = {
				type: 'call',
				func: TaiJiGUI.tj.cfg.func[$('#select-func').val().toInt()].func,
				parm: {}
			}
			$('#parm-list').children().each(function () {
				let v = null;
				switch ($(this).attr('_t')) {
					default:
						break;
					case 'Bool':
						v = $(this).children().eq(1).children().children().first().prop('checked');
						break;
					case 'Int':
						v = $(this).children().eq(1).children().children().val().toInt();
						break;
					case 'Str':
						v = $(this).children().eq(1).children().children().val();
						break;
					case 'Path':
						v = [$(this).children().eq(1).first().children().children().val()];
						break;
				}
				script.parm[$(this).attr('_k')] = v;
			});
			curScript = script;
			mdui.dialog({
				title: 'Run Script',
				content: JSON.stringify(script, null, 4),
				buttons: [
					{
						text: 'Close'
					},
					{
						text: 'OK',
						onClick: function () {
							TaiJiGUI.tj.start(['*', '#' + TaiJiGUI.path.cfg, '$' + JSON.stringify(curScript)]);
							mdui.Tab('#interface-select').show(0);
						}
					}
				]
			});
		});

		for (let i in this.tj.cfg.func) {
			let fn = '';
			for (let s of this.tj.cfg.func[i].func) {
				fn += ':' + s;
			}
			$('#select-func').append(DocUtil.makeOpt(fn, i));
		}
		$('#select-func').bind('change', function () {
			$('#parm-list').empty();
			let i = $(this).val().toInt();
			let parm = TaiJiGUI.tj.cfg.func[i].parm;
			for (let k in parm) {
				let newLine = $('<li/>').addClass('mdui-list-item mdui-row').attr('_k', k).attr('_t', parm[k]);
				newLine.append($('<div>').addClass('mdui-col-xs-3').append($('<span/>').text(k)));
				switch (parm[k]) {
					default:
						break;
					case 'Bool':
						newLine.append($('<div/>').addClass('mdui-col-xs-9').append(MDUtil.makeCheckBox({}, null, false)));
						break;
					case 'Int':
						newLine.append($('<div/>').addClass('mdui-col-xs-9')
							.append($('<div/>').addClass('mdui-textfield')
								.append($('<input/>').addClass('mdui-textfield-input').attr({ type: 'number', 'min': 0 })
								)
							)
						);
						break;
					case 'Str':
						newLine.append($('<div/>').addClass('mdui-col-xs-9')
							.append($('<div/>').addClass('mdui-textfield')
								.append($('<input/>').addClass('mdui-textfield-input').attr({ type: 'text' })
								)
							)
						);
						break;
					case 'Path':
						newLine.append($('<div/>').addClass('mdui-col-xs-6')
							.append($('<div/>').addClass('mdui-textfield')
								.append($('<input/>').addClass('mdui-textfield-input').attr({ type: 'text' })
								)
							)
						);
						newLine.append($('<div/>').addClass('mdui-col-xs-3')
							.append($('<button/>').addClass('mdui-btn mdui-btn-raised mdui-color-theme-accent mdui-ripple').text('SELECT').bind('click', async function () {
								let pathList = await selectFile();
								if (pathList.length == 1 && pathList[0][0] === 'D') {
									$(this).parent().prev().children().children().val(pathList[0].substr(2, pathList[0].length - 2));
								}
							})
							)
						);
						break;
				}
				$('#parm-list').append(newLine);
			}
		});
		(new mdui.Select('#select-func')).handleUpdate();
		mdui.mutation();
	}

	static init() {

		$('#console-page').css('height', (MDUtil.getWindowHeightNoAppbar() - $('#console-page').next().outerHeight() - 48).toString() + 'px');

		$('#about').bind({
			click: function () {
				(new mdui.Dialog($('<div/>').addClass('mdui-dialog')
					.append($('<div/>').addClass('mdui-dialog-title').text('About'))
					.append($('<div/>').addClass('mdui-dialog-content')
						.append($('<p/>').text('TaiJi-GUI. By Small-PC'))
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
						.append($('<p/>').text(''))
					)
					.append($('<div/>').addClass('mdui-dialog-actions')
						.append($('<button/>').addClass('mdui-btn mdui-ripple').attr('mdui-dialog-close', '').text('close'))
					))).open();
			}
		});
		$('#refresh').bind({
			click: function () {
				window.location.reload();
			}
		});
		$('#clean').bind({
			click: function () {
				TaiJiGUI.cleanView();
			}
		});

		$('#console-stdin').bind('keypress', function (e) {
			if (e.key == 'Enter') {
				TaiJiGUI.tj.input(this.value + '\n');
				this.value = '';
			}
		});
		$('#console-clear').bind('click', function () {
			TaiJiGUI.tj.clearPage();
		});
		$('#console-exit').bind('click', function () {
			TaiJiGUI.tj.kill();
		});

		document.addEventListener('drop', function (e) {
			e.preventDefault();
			e.stopPropagation();

			let cmd = [];
			for (let f of e.dataTransfer.files) {
				cmd.push(f.path);
			}
			console.log(cmd);
			TaiJiGUI.tj.start(cmd);
		});

		document.addEventListener('dragover', (e) => {
			e.preventDefault();
			e.stopPropagation();

		});

		{
			if (!FS.exist(this.path.cfg)) {
				MDUtil.makeTopSnackbar('file not found : ' + this.path.cfg, 0, false);
				return;
			}
			this.tj.cfg = FS.readAsJSON(this.path.cfg);
			this.makeSimpleScriptGenUI();
		}
		{
			if (!FS.exist(this.path.gui_cfg)) {
				MDUtil.makeTopSnackbar('file not found : ' + this.path.gui_cfg, 0, false);
				return;
			}
			this.tj.gui_cfg = FS.readAsJSON(this.path.gui_cfg);
			let importFontStr = '';
			for (let k in this.tj.gui_cfg.importFont) {
				importFontStr += '@font-face { font-family: ' + k + '; src: ' + this.tj.gui_cfg.importFont[k] + '; }\n';
			}
			$('head').append($('<style/>').html(importFontStr));
			$('head').append($('<style/>').html('#console-page>span { font-family: "' + this.tj.gui_cfg.console.font + '"; font-size: ' + this.tj.gui_cfg.console.fontSize + ' }\n'));
		}

		this.tj.clean();

	}

}

$(function () {
	TaiJiGUI.init();
});
