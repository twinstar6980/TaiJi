"use strict"

// 动画查看器UI名空间
class AnimationViewerUI {

	// HTML元素
	static doce = {

		// HTML下载按钮
		downloadHTML: null,
		// 左抽屉内Sub-Animation列表

		subList: null,
		// 右抽屉内Label列表
		labelList: null,
		// 舞台CSS

		stageStyle: null,
		// 舞台
		stage: null,
		// 舞台状态控制组件的外壳
		stageCtrlShell: null,

		// 绑定
		bind() {
			this.downloadHTML = $('#download-html');
			this.subList = $('#sub-list');
			this.labelList = $('#label-list');
			this.stageStyle = $('#stage-style');
			this.stage = $('#stage');
			this.stageCtrlShell = $('#stage-ctrl-shell');
		}
		
	};

	// CSS-Info数据
	static cssInfo = null;

	// 舞台数据，Array { style, div }
	static stageData = null;

	// 舞台状态
	static stageState = {

		// 左右位移、缩放
		pos_x: 0, pos_y: 0, scale: 1.0,

		// 转换为transform属性值
		makeString() {
			return 'translate({}px, {}px) scale({})'.format(this.pos_x, this.pos_y, this.scale);
		}

	};

	// 创建Sub-Animation列表
	static createSubList(list) {
		// 清空原有内容
		this.doce.subList.empty();
		// 首行全选|全不选按键
		this.doce.subList.append($('<li/>').addClass('mdui-list-item')
			.append(MDUtil.makeDenseBtn('select all', function () { $(this).parent().siblings().find('input').prop('checked', true) }).attr('mdui-tooltip', '{ content: "select all" }'))
			.append(MDUtil.makeDenseBtn('clear all', function () { $(this).parent().siblings().find('input').prop('checked', false) }).attr('mdui-tooltip', '{ content: "clear all" }'))
		);
		// Sub-Animation
		for (let i in list) {
			this.doce.subList.append($('<label/>').addClass('mdui-list-item mdui-ripple').attr('mdui-tooltip', '{ content: "' + list[i] + '" }')
				.append(MDUtil.makeCheckBox({ _i: i }, null, true))
				.append($('<span/>').text('{} | {}'.format(i, list[i])))
			);
		}
	}
	// 创建Label列表
	static createLabelList(list) {
		// 清空原有内容
		this.doce.labelList.empty();
		// 首行播放动画整体
		
		this.doce.labelList.append($('<div/>').addClass('mdui-list-item mdui-ripple').attr('mdui-tooltip', '{ content: "' + '[ 0, {} ]'.format(this.cssInfo.frameSum - 1) + '" }')
			.append(MDUtil.makeIcBtn('play_arrow', function () { AnimationViewerUI.createStage([0, AnimationViewerUI.cssInfo.frameSum - 1]); }))
			.append($('<p/>').text('<all>'))
		);
		// Label
		for (let e in list) {
			this.doce.labelList.append($('<div/>').addClass('mdui-list-item mdui-ripple').attr('mdui-tooltip', '{ content: "' + '[ {}, {} ]'.format(this.cssInfo.label[e][0], this.cssInfo.label[e][1]) + '" }')
				.append(MDUtil.makeIcBtn('play_arrow', function () { AnimationViewerUI.createStage(AnimationViewerUI.cssInfo.label[$(this).attr('_l')]); }).attr('_l', e))
				.append($('<p/>').text(e))
			);
		}
	}

	// 构建舞台
	static createStage(range) {
		if (range === undefined) {
			// 无参数，清空舞台
			this.stageData = null;
			this.doce.stageStyle.empty();
			this.doce.stage.empty();
			this.doce.downloadHTML.prop('disabled', true);
		} else {
			// 生成过滤sub序号表
			let filter = [];
			for (let e of this.doce.subList.find('input')) {
				if (!e.checked)
					filter.push($(e).attr('_i').toInt());
			}
			// 生成舞台数据，填充舞台，启用下载按键
			this.stageData = PvZ2Anim.makeAnim2('main', this.cssInfo, range, filter, $('#res-path').val() + '/');
			this.doce.stageStyle.html(this.stageData[0]);
			this.doce.stage.html(this.stageData[1]);
			this.doce.downloadHTML.prop('disabled', false);
		}
		// 调整舞台状态
		this.modStageState();
	}

	// 调整舞台状态
	static modStageState(prop) {
		prop = prop || {};
		for (let k in prop) {
			let v = prop[k];
			this.stageState[k] = v;
			switch (k) {
				default: break;
				case 'pos_x':
					$('#stage-ctrl-show-pos-x').text(v.toFixed(0));
					$('#stage-ctrl-pos-x').val(v);
					mdui.updateSliders($('#stage-ctrl-pos-x').parent());
					break;
				case 'pos_y':
					$('#stage-ctrl-show-pos-y').text(v.toFixed(0));
					$('#stage-ctrl-pos-y').val(v);
					mdui.updateSliders($('#stage-ctrl-pos-y').parent());
					break;
				case 'scale':
					$('#stage-ctrl-show-scale').text(v.toFixed(2));
					break;
			}
		}
		this.doce.stage.css('transform', this.stageState.makeString());
	}

	// 清除视图
	static cleanView() {
		// 禁用舞台状态控制组件
		this.doce.stageCtrlShell.find('input').prop('disabled', true);
		this.doce.stageCtrlShell.find('span').text('');
		mdui.updateSliders();
		// 禁用下载按键
		this.doce.downloadHTML.prop('disabled', true);
		// 清空Sub-Animation与Label列表
		this.doce.subList.empty();
		this.doce.labelList.empty();
		// 清空舞台
		this.doce.stageStyle.empty();
		this.doce.stage.empty();
		// 清除CSS-Info及与之相联的舞台数据
		this.cssInfo = null;
		this.stageData = null;
	}
	// 创建视图
	static createView(cssInfo) {
		this.cleanView();
		// 启用舞台状态控制组件
		this.doce.stageCtrlShell.find('input').prop('disabled', false);
		mdui.updateSliders();
		// 赋值CSS-Info，并排序层级
		this.cssInfo = cssInfo;
		PvZ2Anim.serLayer(this.cssInfo.layer);
		// 创建Sub-Animation与Label列表
		this.createSubList(this.cssInfo.sub);
		this.createLabelList(this.cssInfo.label);
		// 创建舞台
		this.createStage();
	}

	// UI初始化
	static init() {

		// 为舞台外壳元素设置高度
		$('#stage-shell').css('height', (MDUtil.getWindowHeightNoAppbar() - $('#stage-shell').next().outerHeight() - 52).toPxString());

		// 刷新
		$('#refresh').bind('click', function () { DocUtil.reload(); });

		// 关于
		$('#about').bind('click', function () {
			(new mdui.Dialog($('<div/>').addClass('mdui-dialog')
				.append($('<div/>').addClass('mdui-dialog-title').text('About'))
				.append($('<div/>').addClass('mdui-dialog-content')
					.append($('<p/>').text('Animation-Viewer. by smallpc'))
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
					.append($('<p/>').text('PAM 文件格式研究由 本人 与 时漪、PAK向日葵、伊特 等人 一同完成。'))
					.append($('<p/>').text('使用方法'))
					.append($('<p/>').text('1. 点击上传按钮，上传 .CSS.JSON (使用 TaiJi 解码 PAM 生成)'))
					.append($('<p/>').text('2. 在上端的文本框中输入分解图所在文件夹的路径'))
					.append($('<p/>').text('3. 在右栏选择想要播放的动画范围，点击 Play 按钮即可播放'))
				)
				.append($('<div/>').addClass('mdui-dialog-actions')
					.append($('<button/>').addClass('mdui-btn mdui-ripple').attr('mdui-dialog-close', '').text('close'))
				))).open();
		});

		// 清除视图
		$('#clean-view').bind('click', function () { AnimationViewerUI.cleanView(); });

		// 上传CSS-Info文件
		$('#upload-cssinfo').bind('click', function () {
			let path = FS.selectSingleFile();
			if (path === null) {
				MDUtil.showTopSnackbar('please select one file', 4000, true);
			} else {
				let json = FS.readAsJSON(path);
				// TODO : JSON有效性校验
				AnimationViewerUI.createView(json);
				MDUtil.showTopSnackbar('load css-info finish', 4000, true);
			}
		});
		// 下载当前舞台中动画的HTML文档
		$('#download-html').bind('click', function () { FS.saveFile(PvZ2Anim.makeHTML(AnimationViewerUI.stageData, AnimationViewerUI.stageState), 'text/html', 'anim.html'); });

		// 图像文件夹路径
		$('#res-path').bind('change', function () { $(this).parent()[FS.exist(this.value) ? 'removeClass' : 'addClass']('mdui-textfield-invalid'); });

		// 舞台状态控制：水平位移
		$('#stage-ctrl-pos-x').bind('change', function () { AnimationViewerUI.modStageState({ pos_x: this.value.toInt() }); });
		// 舞台状态控制：垂直位移
		$('#stage-ctrl-pos-y').bind('change', function () { AnimationViewerUI.modStageState({ pos_y: this.value.toInt() }); });
		$('#stage-ctrl-scale-out, #stage-ctrl-scale-in').bind('change', function () { AnimationViewerUI.modStageState({ scale: this.value.toFloat() }); },
);

		// 将input事件同步到change事件
		$('#res-path, input[id^="stage-ctrl-"]').bind('input', function () { $(this).trigger('change'); });

		// 绑定HTML元素
		this.doce.bind();

		// 清除视图
		$('#clean-view').click();

	}

}

// 执行UI初始化
$(() => { AnimationViewerUI.init(); });