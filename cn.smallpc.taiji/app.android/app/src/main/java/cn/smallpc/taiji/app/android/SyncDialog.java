package cn.smallpc.taiji.app.android;

import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.View;

import androidx.appcompat.app.AlertDialog;

public class SyncDialog {
	
	protected final Context mContext;
	
	protected final AlertDialog.Builder mAlertBuilder;
	protected AlertDialog mDialog;
	protected Handler mHandler;
	
	public SyncDialog(Context context) {
		mContext = context;
		mAlertBuilder = new AlertDialog.Builder(context);
	}
	
	public SyncDialog setTitle(String title) {
		mAlertBuilder.setTitle(title);
		return this;
	}
	public SyncDialog setContent(View content) {
		mAlertBuilder.setView(content);
		return this;
	}
	public SyncDialog setOKEvent(DialogInterface.OnClickListener event) {
		mAlertBuilder.setPositiveButton(mAlertBuilder.getContext().getString(R.string.string_ok_upper), event);
		return this;
	}
	
	public void show() {
		mDialog = mAlertBuilder.create();
		mDialog.setCanceledOnTouchOutside(false);
		mDialog.show();
		try {
			mHandler = new SyncHandler();
			Looper.getMainLooper();
			Looper.loop();
		} catch (Exception e) {
		}
	}
	
	static class SyncHandler extends Handler {
		@Override
		public void handleMessage(Message msg) {
			throw new RuntimeException();
		}
	}
	
}
