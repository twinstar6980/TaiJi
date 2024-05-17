// pvz2 animation converter
// css-json by taiji

class PvZ2Anim {

	static getFrameRange(from, range, forceSingleFrame) {
		let beg, end;
		if (Ty.isArr(range)) {
			beg = range[0];
			end = range[1];
		} else if (from.label.hasOwnProperty(range)) {
			beg = from.label[range][0];
			end = from.label[range][1];
		} else {
			beg = 0;
			end = from.frameSum;
		}
		if (forceSingleFrame) {
			end = beg;
		}
		return [beg, end];
	}

	static imgDir = "";

	static commonStyle = `\
[_pvz2-anim]>img {
position: absolute;
animation-timing-function: step-end;
animation-iteration-count: infinite;
}
`;

	static simplifyAnimName(s) {
		return s.replace(new RegExp('/', 'g'), '_');
	}

	static makeLayerID(list) {
		let s = '';
		for (let i in list) {
			if (i == list.length - 1)
				break;
			if (i > 0)
				s += '-';
			s += list[i].toString();
		}
		return s;
	}
	static cmpLayerID(a, b) {
		for (let i = 0; (i < a.length - 1) && (i < b.length - 1); ++i) {
			if (a[i] > b[i])
				return true;
			if (a[i] < b[i])
				break;
		}
		return false;
	}
	static serLayer(t) {
		for (let i = 0; i < t.length; ++i)
			for (let j = i + 1; j < t.length; ++j)
				if (this.cmpLayerID(t[i].id, t[j].id))
					[t[i], t[j]] = [t[j], t[i]];
		return t;
	}

	static makeImgPath(src) {
		return (this.imgDir + src + ".png").replace(new RegExp('\\\\', 'g'), '/');
	}

	static makeImgElement(imgID, layerID) {
		return `<img _i='${imgID}' _l='${layerID}'>\n`;
	}
	static makeImgStyle(animName, id, sz, origin, src) {
		return `\
[_pvz2-anim=${animName}]>img[_i='${id}'] {
width: ${sz[0]}px; height: ${sz[1]}px;
transform-origin: ${-origin[0]}px ${-origin[1]}px;
content: url('${this.makeImgPath(src)}');
}
`;
	}

	static makeKeyFrame(percent, opacity, matrix) {
		return `\
${percent.toFixed(6)}% {
opacity: ${opacity};
transform: matrix(${matrix[0]}, ${matrix[1]}, ${matrix[2]}, ${matrix[3]}, ${matrix[4]}, ${matrix[5]});
}
`;
	}
	static makeLayerStyle(animName, id, frameTotal, frameList, range, globelRange) {
		let keyframeID = this.simplifyAnimName(animName) + '-' + id;
		let s = `\
[_pvz2-anim=${animName}]>img[_l='${id}'] {
animation-name: ${keyframeID};
}
@keyframes ${keyframeID} {
`;
		let diffCnt = range[0] - globelRange[0], diffCntRev = globelRange[1] - range[1];
		if (diffCnt > 0) {
			s += `\
${Number(0.0).toFixed(6)}% {
opacity: 0.0;
}
`;
		}
		if (diffCnt > 1) {
			s += `\
${(diffCnt / frameTotal * 100).toFixed(6)}% {
opacity: 0.0;
}
`;
		}
		let i = 0;
		for (i = (diffCnt < 0) ? -diffCnt : 0; (range[0] + i) <= globelRange[1] && (range[0] + i) <= range[1]; ++i)
			s += this.makeKeyFrame((diffCnt + i) / frameTotal * 100, frameList[i].opacity, frameList[i].matrix);
		if (diffCntRev > 0) {
			s += `\
${((diffCnt + i) / frameTotal * 100).toFixed(6)}% {
opacity: 0.0;
}
`;
		}
		s += `\
${Number(100.0).toFixed(6)}% {
opacity: 0.0;
}
`;
		s += "}\n";
		return s;
	}

	static makeAnimStyle(name, duration) {
		return `\
[_pvz2-anim=${name}]>img {
animation-duration: ${duration}s;
}
`;
	}
	static makeAnim(name, imgList, layerList, range, parentFilter, frameRate) {
		let frameTotal = range[1] - range[0] + 1;
		let animStyle = this.makeAnimStyle(name, frameTotal / frameRate);
		let imgStyle = '', layerStyle = '';
		let imgElement = '';
		for (let i in imgList) {
			imgStyle += this.makeImgStyle(name, i, imgList[i].sz, imgList[i].origin, imgList[i].src);
		}
		for (let layer of layerList) {
			let layerID = this.makeLayerID(layer.id);
			let imgID = layer.id[layer.id.length - 1];
			let shouldNext = false;
			for (let e of layer.parent) {
				if (parentFilter.indexOf(e) != -1) {
					shouldNext = true;
					break;
				}
			}
			if (!shouldNext && !(layer.range[0] > range[1] || layer.range[1] < range[0])) {
				layerStyle += this.makeLayerStyle(name, layerID, frameTotal, layer.frame, layer.range, range);
				imgElement += this.makeImgElement(imgID, layerID);
			}
		}
		return [animStyle + imgStyle + layerStyle, imgElement];
	}
	static makeAnim2(name, data, range, parentFilter, imgDir) {
		this.imgDir = imgDir;
		return this.makeAnim(name, data.img, data.layer, range === null ? [0, data.frameSum - 1] : range, parentFilter, data.frameRate);
	}

	static makeHTML(anim, divState) {
		let head = `\
<style>
#stage {
left: ${divState.pos_x}px;
top: ${divState.pos_y}px;
transform: scale(${divState.scale});
}
[_pvz2-anim] {
position: absolute;
}
${this.commonStyle}${anim[0]}</style>\
`;
		let body = `<div id='stage' _pvz2-anim='main'>\n${anim[1]}</div>`;
		return makeHTML(head, body);
	}
	static makeAnimHTML2(name, data, range, parentFilter, imgDir, divState) {
		return this.makeHTML(this.makeAnim2(name, data, range, parentFilter, imgDir), divState);
	}

}

function makeHTML(head, body) {
	return `\
<!DOCTYPE html>
<html>
<head>
${head}
</head>
<body>
${body}
</body>
</html>\
`;
}
