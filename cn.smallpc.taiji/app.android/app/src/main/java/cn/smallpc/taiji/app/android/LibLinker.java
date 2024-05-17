package cn.smallpc.taiji.app.android;

import android.annotation.SuppressLint;
import android.content.Context;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class LibLinker {
    
    private final Context mContext;
    
    @SuppressLint("UnsafeDynamicallyLoadedCode")
    public LibLinker(Context context) throws java.lang.UnsatisfiedLinkError {
        mContext = context;
        System.load(context.getFilesDir() + "/libtaiji.so");
    }
    
    private static native void 太極Test();
    private static native String 太極GetABIName();
    private static native String 太極GetVerName();
    private static native void 太極RunEntryBySpecialForAndroid(String[] arg);

    public void test() {
        太極Test();
    }
    public String getABIName() {
        return 太極GetABIName();
    }
    public String getVerName() {
        return 太極GetVerName();
    }
    
    public String runEntryBySpecialForAndroid(final String[] cmd) {
        try {
            CppUserInput.mContext = mContext;
            
            List<String> arg = new ArrayList<>();
            
            arg.add(Util.EXTERNAL_STORAGE_DIR);
            arg.add(Util.TJ_LOG_DIR);
            arg.add('#' + Util.TJ_CFG_FILE);
            Collections.addAll(arg, cmd);
            
            太極RunEntryBySpecialForAndroid(arg.toArray(new String[0]));
            
            CppUserInput.mContext = null;
            return null;
        } catch (Exception e) {
            return e.getMessage();
        }
    }
    public String runEntryBySpecialForAndroid(final String cmd) {
        return runEntryBySpecialForAndroid(new String[] { cmd });
    }
    public String runEntryBySpecialForAndroid(final List<String> cmd) {
        return runEntryBySpecialForAndroid(cmd.toArray(new String[0]));
    }
    
    public static boolean checkExist(Context context) {
        try {
            LibLinker lib = new LibLinker(context);
            return true;
        } catch (java.lang.UnsatisfiedLinkError e) {
            return false;
        }
    }
    public static String getVer(Context context) {
        try {
            LibLinker lib = new LibLinker(context);
            return lib.getVerName();
        } catch (java.lang.UnsatisfiedLinkError e) {
            return null;
        }
    }
    
    public static void showInfoAfterRunScript(Context context, String msg) {
        if (msg == null) {
            new MyDialog(context)
                .setTitle(R.string.tip_run_script_finish)
                .setCloseBtn()
                .show();
        } else {
            new MyDialog(context)
                .setTitle(R.string.tip_run_script_failed)
                .setContent(msg)
                .setCloseBtn()
                .show();
        }
    }
    
}
