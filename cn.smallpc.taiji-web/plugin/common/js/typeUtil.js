
String.prototype.toInt = function () {
	return parseInt(this);
}
String.prototype.format = function () {
	let s = this;
	let result = '';
	let lastIndex = -1;
	for (let i = 0; i < s.length;) {
		let c = s[i++];
		switch (c) {
			default:
				result += c;
				break;
			case '\\':
				result += s[i++];
				break;
			case '{': {
				let index = '';
				for (let j = i; j < s.length; ++j) {
					if (s[j] == '}') {
						i = j + 1;
						break;
					}
					index += s[j];
				}
				if (index.length == 0)
					index = lastIndex + 1;
				lastIndex = index;
				result += arguments[index];
				break;
			}
		}
	}
	return result;
}
String.prototype.toFloat = function () {
	return parseFloat(this);
}
Number.prototype.toPxString = function () {
	return this.toString() + 'px';
}
Number.prototype.btw = function (beg, end) {
	return this.toString() + 'px';
}

class Ty {

	static isUndef(v) {
		return Object.prototype.toString.call(v) == "[object Undefined]";
	}
	static isNull(v) {
		return Object.prototype.toString.call(v) == "[object Null]";
	}
	static isArr(v) {
		return Object.prototype.toString.call(v) == "[object Array]";
	}
	static isObj(v) {
		return Object.prototype.toString.call(v) == "[object Object]";
	}
	static isBool(v) {
		return Object.prototype.toString.call(v) == "[object Boolean]";
	}
	static isNum(v) {
		return Object.prototype.toString.call(v) == "[object Number]";
	}
	static isStr(v) {
		return Object.prototype.toString.call(v) == "[object String]";
	}

}

function getTimeStr() {
	let date = new Date();
	return '{}:{}:{}:{}'.format(date.getHours().toString().padStart(2,'0'), date.getMinutes().toString().padStart(2,'0'), date.getSeconds().toString().padStart(2,'0'), date.getMilliseconds().toString().padStart(3,'0'));
}
