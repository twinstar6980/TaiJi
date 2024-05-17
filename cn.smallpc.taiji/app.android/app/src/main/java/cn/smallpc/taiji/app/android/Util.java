package cn.smallpc.taiji.app.android;

import android.Manifest;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;

import androidx.core.app.ActivityCompat;
import androidx.core.content.FileProvider;
import androidx.fragment.app.Fragment;

import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class Util {
	
	public static class EnvTest {
		
		public static String test(final Context context) {
			final StringBuilder builder = new StringBuilder();
			if (!Util.checkStoragePermission(context))
				builder.append(context.getString(R.string.tip_permission_denied)).append('\n');
			if (!LibLinker.checkExist(context))
				builder.append(context.getString(R.string.tip_lib_not_found)).append('\n');
			if (!Util.FS.existFile(Util.TJ_CFG_FILE))
				builder.append(context.getString(R.string.tip_file_not_found)).append(" : ").append(Util.TJ_CFG_FILE).append('\n');
			if (!Util.FS.existFile(Util.TJ_SHORTCUT_FILE))
				builder.append(context.getString(R.string.tip_file_not_found)).append(" : ").append(Util.TJ_SHORTCUT_FILE).append('\n');
			return builder.toString();
		}
		
	}
	
	public static <T> List<T> toggleValue(List<T> list, T value) {
		int index = list.indexOf(value);
		if (index == -1)
			list.add(value);
		else
			list.remove(index);
		return list;
	}
	
	public static class FS {
		
		public static boolean exist(final String path) {
			return (new File(path)).exists();
		}
		public static boolean exist(final String path, boolean isDir) {
			if (isDir)
				return (new File(path)).isDirectory();
			else
				return (new File(path)).isFile();
		}
		public static boolean existFile(final String path) {
			return exist(path, false);
		}
		public static boolean existDir(final String path) {
			return exist(path, true);
		}
		
		public static void copy(final String src, final String dst, final int bufferSize) {
			try {
				final File dstFile = new File(dst);
				final File dstFileParent = dstFile.getParentFile();
				
				if (!dstFileParent.exists())
					dstFileParent.mkdirs();
				
				final InputStream is = new FileInputStream(new File(src));
				final OutputStream os = new FileOutputStream(dstFile);
				final byte[] buffer = new byte[bufferSize];
				
				int len;
				while ((len = is.read(buffer)) > 0) {
					os.write(buffer, 0, len);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		public static void remove(final File file) {
			if (!file.exists())
				return;
			if (file.isFile() || file.list() == null) {
				file.delete();
			} else {
				for (final File e : file.listFiles())
					remove(e);
				file.delete();
			}
		}
		public static void remove(final String path) {
			remove(new File(path));
		}
		public static String readString(final String path)  {
			try {
				final File file = new File(path);
				final byte[] data = new byte[Long.valueOf(file.length()).intValue()];
				final FileInputStream is = new FileInputStream(file);
				is.read(data);
				is.close();
				return new String(data, StandardCharsets.UTF_8);
			} catch (IOException e) {
				log("File NotFound : " + path);
				return null;
			}
		}
		public static void writeString(final String path, final String s) {
			try {
				final File file = new File(path);
				final FileWriter writer = new FileWriter(file);
				writer.write(s);
				writer.close();
			} catch (IOException e) {
				log("File NotFound : " + path);
			}
		}
		
		public static void selectFile(final Activity context, final int requestCode) {
			final Intent intent = new Intent();
			intent.setAction(android.content.Intent.ACTION_GET_CONTENT);
			intent.setType("*/*");
			context.startActivityForResult(intent, requestCode);
		}
		public static void selectFile(final Fragment fragment, final int requestCode) {
			final Intent intent = new Intent();
			intent.setAction(android.content.Intent.ACTION_GET_CONTENT);
			intent.setType("*/*");
			fragment.startActivityForResult(intent, requestCode);
		}
		
		public static void downloadFile(final Context context, final String urlString, final String dst) {
			try{
				log("download: " + urlString);
				final File dstFile = new File(dst);
				final File dstFileParent = dstFile.getParentFile();
				if (!dstFileParent.exists())
					dstFileParent.mkdirs();
				final URL url = new URL(urlString);
				final URLConnection connection = url.openConnection();
				connection.connect();
				final InputStream is = connection.getInputStream();
				final int fileSize = connection.getContentLength();
				if (fileSize <= 0)
					throw new Exception("unknown file size ");
				if (is == null)
					throw new Exception("stream is null");
				final FileOutputStream os = new FileOutputStream(dst);
				final byte buf[] = new byte[1024];
				int downLoadFileSize = 0;
				do {
					final int numread = is.read(buf);
					if (numread == -1)
						break;
					os.write(buf, 0, numread);
					downLoadFileSize += numread;
				} while (true);
				log("download success");
				is.close();
			} catch (Exception ex) {
				log("error: " + ex.getMessage());
			}
		}
		
		public static String uriToPath(final Uri uri) {
			final String scheme = uri.getScheme();
			Util.log(uri.toString());
			if (scheme.equals(ContentResolver.SCHEME_FILE))
				return uri.getPath();
			return uri.getPath();
		}
	}
	
	public static <K, V> Map.Entry<K, V> getMapItemN(final LinkedHashMap<K, V> map, final int pos) {
		final Iterator<Map.Entry<K, V>> it = map.entrySet().iterator();
		Map.Entry<K, V> result = null;
		for (int i = 0; i <= pos; ++i) {
			result = it.next();
		}
		return result;
	}
	
	public static View[] getListItemArray(ListView listView) {
		int n = listView.getChildCount();
		View[] item = new View[n];
		for (int i = 0; i < n; ++i) {
			item[i] = listView.getChildAt(i);
		}
		return item;
	}
	
	public static <T> void log(final T msg) {
		Log.d("cn.smallpc.taiji-log", msg.toString());
	}
	
	public static <T> void showToast(final Context context, final T msg) {
		Toast.makeText(context, msg.toString(), Toast.LENGTH_SHORT).show();
	}
	
	public static boolean checkStoragePermission(final Context context) {
		return ActivityCompat.checkSelfPermission(context, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
			&& ActivityCompat.checkSelfPermission(context, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
	}
	public static boolean checkInstallPermission(final Context context) {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
			return context.getPackageManager().canRequestPackageInstalls();
		} else {
			return true;
		}
	}
	public static boolean isInstalledApp(Context context, String packageName) {
		final PackageManager packageManager = context.getPackageManager();
		List<PackageInfo> pinfo = packageManager.getInstalledPackages(0);
		List<String> pName = new ArrayList<>();
		if (pinfo != null) {
			for (int i = 0; i < pinfo.size(); i++) {
				String pn = pinfo.get(i).packageName;
				pName.add(pn);
			}
		}
		return pName.contains(packageName);
	}
	
	public static int getAppVerCode(final Context context, final String pkg) {
		try {
			final PackageManager packageManager = context.getPackageManager();
			final PackageInfo packageInfo = packageManager.getPackageInfo(pkg, 0);
			return packageInfo.versionCode;
		} catch (Exception e) {
			e.printStackTrace();
			return -1;
		}
	}
	public static String getAppVerName(final Context context, final String pkg) {
		try {
			final PackageManager packageManager = context.getPackageManager();
			final PackageInfo packageInfo = packageManager.getPackageInfo(pkg, 0);
			return packageInfo.versionName;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	public static String getAppExternalFilesDirPath(final Context context, final String pkg, final String subDir) {
		try {
			return context.createPackageContext(pkg, Context.CONTEXT_IGNORE_SECURITY).getExternalFilesDir(subDir).getPath();
		} catch (PackageManager.NameNotFoundException e) {
			return null;
		}
	}
	public static String getAppObbDirPath(final Context context, final String pkg) {
		try {
			return context.createPackageContext(pkg, Context.CONTEXT_IGNORE_SECURITY).getObbDir().getPath();
		} catch (PackageManager.NameNotFoundException e) {
			return null;
		}
	}
	public static String getAppOBBFilePath(final Context context, final String pkg, final String type) {
		return getAppObbDirPath(context, pkg) + "/" + type + "." + (new Integer(getAppVerCode(context, pkg))).toString() + "." + pkg + ".obb";
	}
	public static String getAppOBBFileName(final Context context, final String pkg, final String type) {
		return type + "." + (new Integer(getAppVerCode(context, pkg))).toString() + "." + pkg + ".obb";
	}
	
	public static void installAPK(final Context context, final String path) {
		if (!new File(path).exists())
			return;
		final Intent intent = new Intent(Intent.ACTION_VIEW);
		final Uri uri = FileProvider.getUriForFile(context, "cn.smallpc.taiji.app.android.fileprovider", new File(path));
		intent.setDataAndType(uri, "application/vnd.android.package-archive");
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_GRANT_READ_URI_PERMISSION);
		log(uri.toString());
		context.startActivity(intent);
	}
	
	public static final String EXTERNAL_STORAGE_DIR = Environment.getExternalStorageDirectory().getPath();
	public static final String TJ_DIR = EXTERNAL_STORAGE_DIR + "/android/data/cn.smallpc.taiji.app.android/files/res";
	public static final String TJ_CFG_FILE = TJ_DIR + "/cfg/cfg.json";
	public static final String TJ_LOG_DIR = TJ_DIR + "/log";
	public static final String TJ_SHORTCUT_FILE = TJ_DIR + "/other/android.shortcut.json";
	public static final String TJ_MOD_DIR = TJ_DIR + "/mod";
	public static final String TJ_MOD_ENV_FILE = TJ_MOD_DIR + "/env.json";
	
}
