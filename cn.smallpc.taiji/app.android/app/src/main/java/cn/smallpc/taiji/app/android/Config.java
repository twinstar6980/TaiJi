package cn.smallpc.taiji.app.android;

import java.util.LinkedHashMap;
import java.util.List;

import com.google.gson.Gson;
import com.google.gson.JsonObject;

class Config {
	
	public enum ParmType { UNKNOWN, BOOL, INT, STR, PATH }
	
	public static class ParmItem {
		public String mName;
		public ParmType mType;
		
		public ParmItem(String name, ParmType type) {
			mName = name;
			mType = type;
		}
	}
	
	public static class FuncInfo {
		public List<String> func;
		public LinkedHashMap<String, String> parm;

		public String getFuncName() {
			String r = "";
			for (final String e : func)
				r += ":" + e;
			return r;
		}
	}
	
	public static class QuickEnterConfig {
		public LinkedHashMap<String, LinkedHashMap<String, JsonObject>> option;
		public LinkedHashMap<String, LinkedHashMap<String, JsonObject>> optionForBatch;
	}

	public List<FuncInfo> func;
	public QuickEnterConfig quickEnter;
	
	public static Config load() {
		return new Gson().fromJson(Util.FS.readString(Util.TJ_CFG_FILE), Config.class);
	}
	
}