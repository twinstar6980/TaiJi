package cn.smallpc.taiji.app.android.activity;

import android.os.Bundle;
import android.os.Environment;
import android.os.StrictMode;
import android.view.View;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import cn.smallpc.taiji.app.android.LibLinker;
import cn.smallpc.taiji.app.android.MyDialog;
import cn.smallpc.taiji.app.android.R;
import cn.smallpc.taiji.app.android.Util;

public class About extends AppCompatActivity {
	
	private TextView mTextAppVer;
	private TextView mTextLibABI;
	private TextView mTextLibVer;
	
	private void setInfoText() {
		mTextAppVer.setText("App-Ver : " + Util.getAppVerName(this, getPackageName()) + "(" + Util.getAppVerCode(this, getPackageName()) + ")");
		try {
			LibLinker lib = new LibLinker(this);
			mTextLibABI.setText("Lib-ABI : " + lib.getABIName());
			mTextLibVer.setText("Lib-Ver : " + lib.getVerName());
		} catch (java.lang.UnsatisfiedLinkError e) {
			mTextLibABI.setText("Lib-ABI : " + "unknown");
			mTextLibVer.setText("Lib-Ver : " + "unknown");
		}
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_about);
		setTitle(getString(R.string.title_about));
		
		mTextAppVer = findViewById(R.id.app_ver);
		mTextLibABI = findViewById(R.id.lib_abi);
		mTextLibVer = findViewById(R.id.lib_ver);
		
		setInfoText();
		
		findViewById(R.id.update).setOnClickListener(view -> {
			String s = new Updater().update();
			new MyDialog(About.this)
				.setTitle(R.string.string_finish_upper)
				.setContent(s)
				.setCloseBtn()
				.show();
			setInfoText();
		});
		
		StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
		StrictMode.setThreadPolicy(policy);
	}
	
	private class Updater {
		
		private final String UPDATER_VERSION = "2";
		
		private final String URL_DOWNLOAD_DIR = "https://smallpc.cn/taiji/download/";
		private final String URL_RES_DIR = URL_DOWNLOAD_DIR + "res/";
		private final String URL_INFO_FILE = URL_DOWNLOAD_DIR + "update/android.txt";
		
		private final String DOWNLOAD_DIR = About.this.getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS).toString() + "/";
		private final String DOWNLOAD_INFO_FILE_PATH = DOWNLOAD_DIR + "tmp/update.json";
		private final String DOWNLOAD_APP_FILE_PATH = DOWNLOAD_DIR + "tmp/app.apk";
		private final String LIB_FILE_PATH = About.this.getFilesDir() + "/libtaiji.so";
		private final String DOWNLOAD_RES_DIR = About.this.getExternalFilesDir("res").toString() + "/";
		
		private void cleanData() {
			Util.FS.remove(DOWNLOAD_INFO_FILE_PATH);
			Util.FS.remove(DOWNLOAD_APP_FILE_PATH);
		}
		
		private String update() {
			if (!Util.checkStoragePermission(About.this) ||
				!Util.checkInstallPermission(About.this))
				return getString(R.string.tip_update_need_permission);
			try {
				cleanData();
				Util.FS.downloadFile(About.this, URL_INFO_FILE, DOWNLOAD_INFO_FILE_PATH);
				BufferedReader reader = new BufferedReader(new FileReader(DOWNLOAD_INFO_FILE_PATH));
				{
					// check updater version
					String updaterVer = reader.readLine();
					if (!updaterVer.equals("android-updater-version=" + UPDATER_VERSION)) {
						return getString(R.string.tip_update_error_ver);
					}
				}
				{
					// update app
					String appVer = reader.readLine();
					String appURL = reader.readLine();
					if (!appVer.equals(Util.getAppVerName(About.this, getPackageName()))) {
						Util.FS.downloadFile(About.this, URL_RES_DIR + appURL, DOWNLOAD_APP_FILE_PATH);
						Util.installAPK(About.this, DOWNLOAD_APP_FILE_PATH);
						return getString(R.string.tip_update_app_finish);
					}
				}
				String result = "";
				{
					// update lib
					String libVer = reader.readLine();
					String libURL = reader.readLine();
					if (!libVer.equals(LibLinker.getVer(About.this))) {
						Util.FS.downloadFile(About.this, URL_RES_DIR + libURL, LIB_FILE_PATH);
						result += getString(R.string.tip_update_lib_finish) + '\n';
					}
				}
				{
					// update files
					String path;
					while ((path = reader.readLine()) != null) {
						Util.FS.downloadFile(About.this, URL_RES_DIR + path, DOWNLOAD_RES_DIR + path);
					}
					result += getString(R.string.tip_update_file_finish);
				}
				return result;
			} catch (IOException e) {
				e.printStackTrace();
			}
			return null;
		}
		
	}
	
}
