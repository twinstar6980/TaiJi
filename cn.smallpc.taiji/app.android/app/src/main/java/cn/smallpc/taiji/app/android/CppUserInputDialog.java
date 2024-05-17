package cn.smallpc.taiji.app.android;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.widget.CheckBox;
import android.widget.EditText;

import java.io.File;
import java.util.Arrays;

import cn.smallpc.taiji.app.android.Config.ParmType;

public class CppUserInputDialog extends SyncDialog {
	
	static protected String mData;
	private View mDataView;
	
	private String getLastLog() {
		String fileName = Util.TJ_LOG_DIR + "/";
		File path = new File(fileName);
		if (path.exists()) {
			File[] files = path.listFiles();
			Arrays.sort(files, (file1, file2) -> (int) (file2.lastModified() - file1.lastModified()));
			fileName += files[0].getName();
		}
		return Util.FS.readString(fileName);
	}
	
	public CppUserInputDialog(Context context, String title, ParmType type) {
		super(context);
		setTitle(title == null ? "null" : title);
		mData = "";
		switch (type) {
			default:
				break;
			case UNKNOWN:
				mDataView = null;
				setContent(mDataView);
				setOKEvent((dialogInterface, i) -> {
					mDialog.dismiss();
					Message msg = mHandler.obtainMessage();
					mHandler.sendMessage(msg);
				});
				break;
			case BOOL:
				mDataView = new CheckBox(context);
				setContent(mDataView);
				setOKEvent((dialogInterface, i) -> {
					mData = mData.concat(Boolean.toString(((CheckBox)mDataView).isChecked()));
					mDialog.dismiss();
					Message msg = mHandler.obtainMessage();
					mHandler.sendMessage(msg);
				});
				break;
			case INT:
			case STR:
			case PATH:
				mDataView = new EditText(context);
				setContent(mDataView);
				setOKEvent((dialogInterface, i) -> {
					mData = ((EditText)mDataView).getText().toString();
					mDialog.dismiss();
					Message msg = mHandler.obtainMessage();
					mHandler.sendMessage(msg);
				});
				break;
		}
		show();
	}
	
	@Override
	public void show() {
		mAlertBuilder.setNeutralButton("LOG", null);
		mDialog = mAlertBuilder.create();
		mDialog.setCanceledOnTouchOutside(false);
		mDialog.show();
		mDialog.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(view -> {
			new MyDialog(mContext)
				.setTitle("LOG")
				.setContent(getLastLog())
				.setCloseBtn()
				.show();
		});
		try {
			mHandler = new SyncHandler();
			Looper.getMainLooper();
			Looper.loop();
		} catch (Exception e) {
		}
	}
	
}
