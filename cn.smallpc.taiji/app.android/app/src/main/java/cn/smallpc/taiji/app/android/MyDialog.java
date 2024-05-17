package cn.smallpc.taiji.app.android;

import android.content.Context;
import android.content.DialogInterface;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;

import androidx.appcompat.app.AlertDialog;

public class MyDialog {
	public static class Builder {
		private final AlertDialog.Builder mBuilder;
		
		public Builder(Context context) {
			mBuilder = new AlertDialog.Builder(context);
		}
		
		public MyDialog create() {
			return new MyDialog(mBuilder.create());
		}
		
		public Builder setView(View view) {
			mBuilder.setView(view);
			return this;
		}
		public Builder setMultiChoice(String[] item, boolean[] initState, DialogInterface.OnMultiChoiceClickListener listener) {
			mBuilder.setMultiChoiceItems(item, initState, listener);
			return this;
		}
		
	}
	
	private final AlertDialog mDialog;
	
	public MyDialog(Context context) {
		mDialog = new AlertDialog.Builder(context).create();
	}
	public MyDialog(AlertDialog dialog) {
		mDialog = dialog;
	}
	
	public AlertDialog dialog() {
		return mDialog;
	}
	
	public MyDialog setTitle(String title) {
		mDialog.setTitle(title);
		return this;
	}
	public MyDialog setTitle(int title) {
		return setTitle(mDialog.getContext().getString(title));
	}
	
	public MyDialog setContent(String content) {
		mDialog.setMessage(content);
		return this;
	}
	public MyDialog setContent(int content) {
		return setContent(mDialog.getContext().getString(content));
	}
	
	public MyDialog setContent(View view) {
		mDialog.setView(view);
		return this;
	}
	
	public MyDialog setPosBtn(String msg, DialogInterface.OnClickListener listener) {
		mDialog.setButton(AlertDialog.BUTTON_POSITIVE, msg, listener);
		return this;
	}
	public MyDialog setNegBtn(String msg, DialogInterface.OnClickListener listener) {
		mDialog.setButton(AlertDialog.BUTTON_NEGATIVE, msg, listener);
		return this;
	}
	public MyDialog setNeuBtn(String msg, DialogInterface.OnClickListener listener) {
		mDialog.setButton(AlertDialog.BUTTON_NEUTRAL, msg, listener);
		return this;
	}
	
	public MyDialog setPosBtn(int msg, DialogInterface.OnClickListener listener) {
		return setPosBtn(mDialog.getContext().getString(msg), listener);
	}
	public MyDialog setNegBtn(int msg, DialogInterface.OnClickListener listener) {
		return setNegBtn(mDialog.getContext().getString(msg), listener);
	}
	public MyDialog setNeuBtn(int msg, DialogInterface.OnClickListener listener) {
		return setNeuBtn(mDialog.getContext().getString(msg), listener);
	}
	
	public Button getPosBtn() {
		return mDialog.getButton(DialogInterface.BUTTON_POSITIVE);
	}
	public Button getNegBtn() {
		return mDialog.getButton(DialogInterface.BUTTON_NEGATIVE);
	}
	public Button getNeuBtn() {
		return mDialog.getButton(DialogInterface.BUTTON_NEUTRAL);
	}
	
	public MyDialog setPosBtn(String msg) {
		return setPosBtn(msg, null);
	}
	public MyDialog setNegBtn(String msg) {
		return setNegBtn(msg, null);
	}
	public MyDialog setNeuBtn(String msg) {
		return setNeuBtn(msg, null);
	}
	public MyDialog setPosBtn(int msg) {
		return setPosBtn(mDialog.getContext().getString(msg));
	}
	public MyDialog setNegBtn(int msg) {
		return setNegBtn(mDialog.getContext().getString(msg));
	}
	public MyDialog setNeuBtn(int msg) {
		return setNeuBtn(mDialog.getContext().getString(msg));
	}
	
	public MyDialog setOKBtn(DialogInterface.OnClickListener listener) {
		return setPosBtn(R.string.string_ok_upper, listener);
	}
	public MyDialog setYesBtn(DialogInterface.OnClickListener listener) {
		return setPosBtn(R.string.string_yes_upper, listener);
	}
	public MyDialog setNoBtn(DialogInterface.OnClickListener listener) {
		return setNegBtn(R.string.string_no_upper, listener);
	}
	public MyDialog setCancelBtn(DialogInterface.OnClickListener listener) {
		return setNegBtn(R.string.string_cancel_upper, listener);
	}
	public MyDialog setCloseBtn(DialogInterface.OnClickListener listener) {
		return setNegBtn(R.string.string_close_upper, listener);
	}
	
	public MyDialog setOKBtn() {
		return setOKBtn(null);
	}
	public MyDialog setYesBtn() {
		return setYesBtn(null);
	}
	public MyDialog setNoBtn() {
		return setNoBtn(null);
	}
	public MyDialog setCancelBtn() {
		return setCancelBtn(null);
	}
	public MyDialog setCloseBtn() {
		return setCloseBtn(null);
	}
	
	private MyDialog setTipBtn(Button.OnClickListener listener) {
		getNeuBtn().setOnClickListener(listener);
		return this;
	}
	private MyDialog setTipBtn(String msg) {
		return setTipBtn(view -> new MyDialog(mDialog)
			.setContent(msg)
			.setCloseBtn()
			.show());
	}
	
	public MyDialog setOnShowListener(DialogInterface.OnShowListener listener) {
		mDialog.setOnShowListener(listener);
		return this;
	}
	
	public View[] getListItemArray() {
		return Util.getListItemArray(mDialog.getListView());
	}
	
	public MyDialog show() {
		mDialog.show();
		return this;
	}
	
}
