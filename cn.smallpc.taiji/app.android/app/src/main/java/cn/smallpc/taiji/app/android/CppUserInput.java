package cn.smallpc.taiji.app.android;

import android.content.Context;

import cn.smallpc.taiji.app.android.Config.ParmType;

public class CppUserInput {
	
	static Context mContext;
	
	static void pause(String msg) {
		new CppUserInputDialog(mContext, msg, ParmType.UNKNOWN);
	}
	static boolean inputBool(String msg) {
		new CppUserInputDialog(mContext, msg, ParmType.BOOL);
		Util.log("data : " + CppUserInputDialog.mData);
		return Boolean.parseBoolean(CppUserInputDialog.mData);
	}
	static int inputInt(String msg) {
		new CppUserInputDialog(mContext, msg, ParmType.INT);
		Util.log("data : " + CppUserInputDialog.mData);
		return Integer.parseInt(CppUserInputDialog.mData);
	}
	static String inputStr(String msg) {
		new CppUserInputDialog(mContext, msg, ParmType.STR);
		Util.log("data : " + CppUserInputDialog.mData);
		return CppUserInputDialog.mData;
	}
	static String inputPath(String msg) {
		new CppUserInputDialog(mContext, msg, ParmType.PATH);
		Util.log("data : " + CppUserInputDialog.mData);
		return CppUserInputDialog.mData;
	}
	
}
