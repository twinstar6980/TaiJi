package cn.smallpc.taiji.app.android;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.CheckResult;
import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.PreferenceManager;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class ModManagerActivity extends AppCompatActivity {
	
	private final Context mContext = this;
	
	private LibLinker mLib;
	
	private String mTargetPkgName;
	
	private final LinkedHashMap<String, Boolean> mModState = new LinkedHashMap<>();
	
	private String mCurScript = "";
	
	private void runModXxtro(String s) {
		JsonObject script = new JsonObject();
		script.addProperty("type", "call");
		JsonArray func = new JsonArray();
		func.add("Expand");
		func.add("PvZ2Mod");
		func.add(s);
		script.add("func", func);
		JsonObject parm = new JsonObject();
		parm.add("EnvFile", new JsonArray());
		parm.getAsJsonArray("EnvFile").add(Util.TJ_MOD_ENV_FILE);
		script.add("parm", parm);
		String result = mLib.runEntryBySpecialForAndroid("$" + script.toString());
		LibLinker.showInfoAfterRunScript(ModManagerActivity.this, result);
	}
	private void runModApply() {
		StringBuilder selectedModListTip = new StringBuilder();
		JsonArray scriptList = new JsonArray();
		for (Map.Entry<String, Boolean> e : mModState.entrySet()) {
			if (e.getValue()) {
				selectedModListTip.append(e.getKey()).append("\n");
				JsonObject script = new JsonObject();
				script.addProperty("type", "call");
				JsonArray func = new JsonArray();
				func.add("Expand");
				func.add("PvZ2Mod");
				func.add("Apply");
				script.add("func", func);
				JsonObject parm = new JsonObject();
				parm.add("EnvFile", new JsonArray());
				parm.getAsJsonArray("EnvFile").add(Util.TJ_MOD_ENV_FILE);
				parm.add("ModDir", new JsonArray());
				parm.getAsJsonArray("ModDir").add(Util.TJ_MOD_DIR + "/" + e.getKey() + ".tjpvz2mod");
				script.add("parm", parm);
				scriptList.add(script);
			}
		}
		mCurScript = scriptList.toString();
		new MyDialog(mContext)
			.setTitle(R.string.text_mod_apply)
			.setContent(selectedModListTip.toString())
			.setYesBtn((dialogInterface, i) -> {
				String result = mLib.runEntryBySpecialForAndroid("$" + mCurScript);
				LibLinker.showInfoAfterRunScript(ModManagerActivity.this, result);
			})
			.setNoBtn()
			.show();
	}
	
	private EditText mEditViewRSBPackMemSz;
	private EditText mEditViewRTONEncodeMemSz;
	
	private enum GameDataType {
		RSB_UNPACK_DIR, NO_BACKUP_DIR, CDN_DIR,
		RSB_FILE, PLAYER_DATA_FILE, SNAPSHOT_1_FILE, SNAPSHOT_2_FILE,
		RSB_BACKUP_FILE, PLAYER_DATA_BACKUP_FILE, SNAPSHOT_1_BACKUP_FILE, SNAPSHOT_2_BACKUP_FILE,
	}
	
	private String getGameDataPath(final GameDataType type) {
		String path = null;
		switch (type) {
			default:
				break;
			case NO_BACKUP_DIR:
				path = Util.getAppExternalFilesDirPath(this, mTargetPkgName, "No_Backup");
				break;
			case SNAPSHOT_1_FILE:
				path = Util.getAppExternalFilesDirPath(this, mTargetPkgName, "No_Backup") + "/snapshot1.dat";
				break;
			case SNAPSHOT_2_FILE:
				path = Util.getAppExternalFilesDirPath(this, mTargetPkgName, "No_Backup") + "/snapshot2.dat";
				break;
			case RSB_FILE:
				path = Util.getAppOBBFilePath(this, mTargetPkgName, "main");
				break;
			case RSB_UNPACK_DIR:
				path = Util.getAppOBBFilePath(this, mTargetPkgName, "main") + ".dir";
				break;
			case PLAYER_DATA_FILE:
				path = Util.getAppExternalFilesDirPath(this, mTargetPkgName, "No_Backup") + "/pp.dat";
				break;
			case CDN_DIR:
				path = Util.getAppExternalFilesDirPath(this, mTargetPkgName, "No_Backup") + "/CDN." + Util.getAppVerName(mContext, mTargetPkgName).substring(0, 3);
				break;
			case RSB_BACKUP_FILE:
				path = getExternalFilesDir("backup") + "/" + mTargetPkgName + "/" + Util.getAppOBBFileName(this, mTargetPkgName, "main");
				break;
			case PLAYER_DATA_BACKUP_FILE:
				path = getExternalFilesDir("backup") + "/" + mTargetPkgName + "/" + "pp.dat";
				break;
			case SNAPSHOT_1_BACKUP_FILE:
				path = getExternalFilesDir("backup") + "/" + mTargetPkgName + "/" + "snapshot1.dat";
				break;
			case SNAPSHOT_2_BACKUP_FILE:
				path = getExternalFilesDir("backup") + "/" + mTargetPkgName + "/" + "snapshot2.dat";
				break;
		}
		return path;
	}
	
	private void createModEnv() {
		JsonObject env = new JsonObject();
		
		JsonObject rsbMain = new JsonObject();
		rsbMain.add("file", new JsonArray());
		rsbMain.getAsJsonArray("file").add(getGameDataPath(GameDataType.RSB_FILE));
		rsbMain.add("dir", new JsonArray());
		rsbMain.getAsJsonArray("dir").add(getGameDataPath(GameDataType.RSB_UNPACK_DIR));
		rsbMain.add("oriInfoFile", new JsonArray());
		rsbMain.getAsJsonArray("oriInfoFile").add("properties");
		rsbMain.getAsJsonArray("oriInfoFile").add("resources.rton");
		
		env.add("rsb", new JsonArray());
		env.getAsJsonArray("rsb").add(rsbMain);
		
		env.add("path", new JsonObject());
		env.getAsJsonObject("path").add("rsb.main", new JsonArray());
		env.getAsJsonObject("path").getAsJsonArray("rsb.main").add(getGameDataPath(GameDataType.RSB_UNPACK_DIR));
		env.getAsJsonObject("path").add("data", new JsonArray());
		env.getAsJsonObject("path").getAsJsonArray("data").add(getGameDataPath(GameDataType.NO_BACKUP_DIR));
		env.getAsJsonObject("path").add("cdn", new JsonArray());
		env.getAsJsonObject("path").getAsJsonArray("cdn").add(getGameDataPath(GameDataType.CDN_DIR));
		
		env.addProperty("memSzWhenRSBPack", Integer.parseInt(mEditViewRSBPackMemSz.getText().toString()));
		env.addProperty("memSzWhenRTONEnCode", Integer.parseInt(mEditViewRTONEncodeMemSz.getText().toString()));
		env.addProperty("enableStrIdxWhenRTONEnCode", false);
		
		Util.FS.writeString(Util.TJ_MOD_ENV_FILE, env.toString());
	}
	
	private String getGameOBBPath() {
		return getGameDataPath(GameDataType.RSB_FILE);
	}
	private String getGamePlayerDataPath() {
		return getGameDataPath(GameDataType.PLAYER_DATA_FILE);
	}
	
	private String testGameData() {
		String obbFile = getGameOBBPath();
		String ppFile = getGamePlayerDataPath();
		String result = "";
		if (!Util.FS.existFile(obbFile)) {
			result += getString(R.string.tip_game_obb_not_found) + " : " + obbFile + "\n";
		}
		if (!Util.FS.existFile(ppFile)) {
			result += getString(R.string.tip_game_pp_not_found) + " : " + ppFile + "\n";
		}
		return result;
	}
	
	private boolean checkGameBackup() {
		getGameDataPath(GameDataType.RSB_BACKUP_FILE);
		getGameDataPath(GameDataType.PLAYER_DATA_BACKUP_FILE);
		String rsbBakFile = getGameOBBPath() + ".bak";
		String ppBakFile = getGamePlayerDataPath() + ".bak";
		return Util.FS.existFile(rsbBakFile) && Util.FS.existFile(ppBakFile);
	}
	
	List<Integer> listSelected = new ArrayList<>();
	
	private LinkedHashMap<String, Pair<Boolean, String>> getDataPathMap() {
		return new LinkedHashMap<String, Pair<Boolean, String>>() {{
			put("RSB",
				new Pair<>(Boolean.FALSE, getGameDataPath(GameDataType.RSB_FILE)));
			put("PlayerData",
				new Pair<>(Boolean.FALSE, getGameDataPath(GameDataType.PLAYER_DATA_FILE)));
			put("Snapshot1",
				new Pair<>(Boolean.FALSE, getGameDataPath(GameDataType.SNAPSHOT_1_FILE)));
			put("Snapshot2",
				new Pair<>(Boolean.FALSE, getGameDataPath(GameDataType.SNAPSHOT_2_FILE)));
			put("CDN",
				new Pair<>(Boolean.TRUE, getGameDataPath(GameDataType.CDN_DIR)));
			put("ModTempDir",
				new Pair<>(Boolean.TRUE, getGameDataPath(GameDataType.RSB_UNPACK_DIR)));
		}};
	}
	private LinkedHashMap<String, Pair<Boolean, Pair<String, String>>> getDataBackupPathMap() {
		return new LinkedHashMap<String, Pair<Boolean, Pair<String, String>>>() {{
			put("RSB",
				new Pair<>(Boolean.FALSE, new Pair<>(getGameDataPath(GameDataType.RSB_FILE), getGameDataPath(GameDataType.RSB_BACKUP_FILE))));
			put("PlayerData",
				new Pair<>(Boolean.FALSE, new Pair<>(getGameDataPath(GameDataType.PLAYER_DATA_FILE), getGameDataPath(GameDataType.PLAYER_DATA_BACKUP_FILE))));
			put("Snapshot1",
				new Pair<>(Boolean.FALSE, new Pair<>(getGameDataPath(GameDataType.SNAPSHOT_1_FILE), getGameDataPath(GameDataType.SNAPSHOT_1_BACKUP_FILE))));
			put("Snapshot2",
				new Pair<>(Boolean.FALSE, new Pair<>(getGameDataPath(GameDataType.SNAPSHOT_2_FILE), getGameDataPath(GameDataType.SNAPSHOT_2_BACKUP_FILE))));
		}};
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_mod_manager);
		setTitle(getString(R.string.title_mod_manager));
		
		mLib = new LibLinker(this);
		
		new File(Util.TJ_MOD_DIR).mkdirs();
		
		mEditViewRSBPackMemSz = findViewById(R.id.mm_rsb_pack_mem_sz);
		mEditViewRTONEncodeMemSz = findViewById(R.id.mm_rton_enc_mem_sz);
		
		{
			SharedPreferences settingPreferences = PreferenceManager.getDefaultSharedPreferences(this);
			String pkgNameList = settingPreferences.getString("mod_target_package_name_list", "");
			
			mEditViewRSBPackMemSz.setText(settingPreferences.getString("mod_rsb_pack_mem_sz", "0"));
			mEditViewRTONEncodeMemSz.setText(settingPreferences.getString("mod_rton_enc_mem_sz", "0"));
			
			if (pkgNameList.isEmpty()) {
				//Util.showDialog(this, );
			}
			
			Spinner pkgSelector = findViewById(R.id.mm_pkg_select);
			
			ArrayAdapter<String> pkgSelectorAdapter = new ArrayAdapter<>(this, R.layout.support_simple_spinner_dropdown_item);
			pkgSelectorAdapter.addAll(pkgNameList.split("\n"));
			
			pkgSelector.setAdapter(pkgSelectorAdapter);
			pkgSelector.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
				@Override
				public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
					String pkg = adapterView.getItemAtPosition(i).toString();
					mTargetPkgName = pkg;
					if (!Util.isInstalledApp(mContext, pkg)) {
						new MyDialog(mContext)
							.setContent("未安装所选包名之对应程序")
							.setCloseBtn()
							.show();
					}
				}
				
				@Override
				public void onNothingSelected(AdapterView<?> adapterView) {
				}
			});
			
		}
		
		findViewById(R.id.mm_data_check).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				final LinkedHashMap<String, Pair<Boolean, String>> dataPath = getDataPathMap();
				final ListView listView = new ListView(mContext);
				listView.setAdapter(new ArrayAdapter<>(mContext, R.layout.support_simple_spinner_dropdown_item, dataPath.keySet().toArray(new String[]{})));
				listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
					@Override
					public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
						new MyDialog(mContext).setContent(Util.getMapItemN(dataPath, i).getValue().second).show();
					}
				});
				new MyDialog(mContext)
					.setContent("点击每行文字即可显示对应路径\n红体字表示对应数据文件/文件夹不存在")
					.setContent(listView)
					.setCloseBtn()
					.setOnShowListener(dialogInterface -> {
						View[] item = Util.getListItemArray(listView);
						int i = 0;
						for (Map.Entry<String, Pair<Boolean, String>> e : dataPath.entrySet()) {
							if (!Util.FS.exist(e.getValue().second, e.getValue().first))
								((TextView)item[i]).setTextColor(Color.RED);
							++i;
						}
					})
					.show();
			}
		});
		findViewById(R.id.mm_data_clean).setOnClickListener(view -> {
			final LinkedHashMap<String, Pair<Boolean, String>> dataPath = getDataPathMap();
			final MyDialog dialog = new MyDialog.Builder(ModManagerActivity.this)
				.setMultiChoice(dataPath.keySet().toArray(new String[]{}),
								new boolean[dataPath.size()],
								(dialogInterface, i, b) -> Util.toggleValue(listSelected, i))
				.create();
			dialog
				.setTitle("选择要清除的数据")
				.setCloseBtn()
				.setOKBtn((dialogInterface, i) -> {
					for (Integer e : listSelected)
						Util.FS.remove(Util.getMapItemN(dataPath, e).getValue().second);
					Util.showToast(mContext, getString(R.string.string_finish_upper));
				})
				.setOnShowListener(dialogInterface -> {
					listSelected.clear();
					View[] item = dialog.getListItemArray();
					int i = 0;
					for (Map.Entry<String, Pair<Boolean, String>> e : dataPath.entrySet()) {
						if (!Util.FS.exist(e.getValue().second, e.getValue().first))
							item[i].setEnabled(false);
						++i;
					}
					for (View e : item)
						e.setClickable(!e.isEnabled());
				})
				.show();
		});
		findViewById(R.id.mm_data_backup).setOnClickListener(view -> {
			final LinkedHashMap<String, Pair<Boolean, Pair<String, String>>> dataPathForBackup = getDataBackupPathMap();
			final MyDialog dialog = new MyDialog.Builder(ModManagerActivity.this)
				.setMultiChoice(dataPathForBackup.keySet().toArray(new String[]{}),
					new boolean[dataPathForBackup.size()],
					(dialogInterface, i, b) -> Util.toggleValue(listSelected, i))
				.create();
			dialog
				.setTitle("选择要备份的数据")
				.setCloseBtn()
				.setOKBtn((dialogInterface, i) -> {
					for (Integer e : listSelected) {
						Pair<String, String> path = Util.getMapItemN(dataPathForBackup, e).getValue().second;
						Util.FS.copy(path.first, path.second, 128 * 1024 * 1024);
					}
					Util.showToast(mContext, getString(R.string.string_finish_upper));
				})
				.setOnShowListener(dialogInterface -> {
					listSelected.clear();
					View[] item = dialog.getListItemArray();
					int i = 0;
					for (Map.Entry<String, Pair<Boolean, Pair<String, String>>> e : dataPathForBackup.entrySet()) {
						if (!Util.FS.exist(e.getValue().second.first, e.getValue().first))
							item[i].setEnabled(false);
						if (Util.FS.exist(e.getValue().second.second, e.getValue().first))
							((TextView)item[i]).setTextColor(Color.RED);
						++i;
					}
					for (View e : item)
						e.setClickable(!e.isEnabled());
				})
				.show();
		});
		findViewById(R.id.mm_data_backup_restore).setOnClickListener(view -> {
			final LinkedHashMap<String, Pair<Boolean, Pair<String, String>>> dataPathForBackup = getDataBackupPathMap();
			final MyDialog dialog = new MyDialog.Builder(ModManagerActivity.this)
				.setMultiChoice(dataPathForBackup.keySet().toArray(new String[]{}),
					new boolean[dataPathForBackup.size()],
					(dialogInterface, i, b) -> Util.toggleValue(listSelected, i))
				.create();
			dialog
				.setTitle("选择要恢复的备份")
				.setCloseBtn()
				.setOKBtn((dialogInterface, i) -> {
					for (Integer e : listSelected) {
						Pair<String, String> path = Util.getMapItemN(dataPathForBackup, e).getValue().second;
						Util.FS.copy(path.second, path.first, 128 * 1024 * 1024);
					}
					Util.showToast(mContext, getString(R.string.string_finish_upper));
				})
				.setOnShowListener(dialogInterface -> {
					listSelected.clear();
					View[] item = dialog.getListItemArray();
					int i = 0;
					for (Map.Entry<String, Pair<Boolean, Pair<String, String>>> e : dataPathForBackup.entrySet()) {
						if (!Util.FS.exist(e.getValue().second.second, e.getValue().first))
							item[i].setEnabled(false);
						if (Util.FS.exist(e.getValue().second.first, e.getValue().first))
							((TextView)item[i]).setTextColor(Color.RED);
						++i;
					}
					for (View e : item)
						e.setClickable(!e.isEnabled());
				})
				.show();
		});
		findViewById(R.id.mm_data_backup_clean).setOnClickListener(view -> {
			final LinkedHashMap<String, Pair<Boolean, Pair<String, String>>> dataPathForBackup = getDataBackupPathMap();
			final MyDialog dialog = new MyDialog.Builder(ModManagerActivity.this)
				.setMultiChoice(dataPathForBackup.keySet().toArray(new String[]{}),
					new boolean[dataPathForBackup.size()],
					(dialogInterface, i, b) -> Util.toggleValue(listSelected, i))
				.create();
			dialog
				.setTitle("选择要恢复的备份")
				.setCloseBtn()
				.setOKBtn((dialogInterface, i) -> {
					for (Integer e : listSelected) {
						Pair<String, String> path = Util.getMapItemN(dataPathForBackup, e).getValue().second;
						Util.FS.remove(path.second);
					}
					Util.showToast(mContext, getString(R.string.string_finish_upper));
				})
				.setOnShowListener(dialogInterface -> {
					listSelected.clear();
					View[] item = dialog.getListItemArray();
					int i = 0;
					for (Map.Entry<String, Pair<Boolean, Pair<String, String>>> e : dataPathForBackup.entrySet()) {
						if (!Util.FS.exist(e.getValue().second.second, e.getValue().first))
							item[i].setEnabled(false);
						if (!Util.FS.exist(e.getValue().second.first, e.getValue().first))
							((TextView)item[i]).setTextColor(Color.RED);
						++i;
					}
					for (View e : item)
						e.setClickable(!e.isEnabled());
				})
				.show();
		});
		
		findViewById(R.id.mm_rsb_unpack).setOnClickListener(view -> {
			if (!Util.FS.existFile(getGameDataPath(GameDataType.RSB_FILE))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_error_upper)
					.setContent("不存在RSB文件")
					.setCloseBtn()
					.show();
				return;
			}
			if (!Util.FS.existFile(getGameDataPath(GameDataType.RSB_BACKUP_FILE))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_warning_upper)
					.setContent("不存在RSB备份文件")
					.setPosBtn("备份并解包", (dialogInterface, i) -> {
						Util.FS.copy(getGameDataPath(GameDataType.RSB_FILE), getGameDataPath(GameDataType.RSB_BACKUP_FILE), 128 * 1024 * 1024);
						createModEnv();
						runModXxtro("Intro");
					})
					.setCloseBtn()
					.show();
				return;
			}
			if (Util.FS.existDir(getGameDataPath(GameDataType.RSB_UNPACK_DIR))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_warning_upper)
					.setContent("已存在RSB解包文件夹")
					.setPosBtn("删除并解包", (dialogInterface, i) -> {
						Util.FS.remove(getGameDataPath(GameDataType.RSB_UNPACK_DIR));
						createModEnv();
						runModXxtro("Intro");
					})
					.setCloseBtn()
					.show();
				return;
			}
			new MyDialog(mContext)
				.setContent("是否执行RSB解包？")
				.setYesBtn((dialogInterface, i) -> {
					createModEnv();
					runModXxtro("Intro");
				})
				.setNoBtn()
				.show();
		});
		findViewById(R.id.mm_rsb_pack).setOnClickListener(view -> {
			if (!Util.FS.existDir(getGameDataPath(GameDataType.RSB_UNPACK_DIR))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_error_upper)
					.setContent("不存在RSB解包文件夹")
					.setCloseBtn()
					.show();
				return;
			}
			if (!Util.FS.existFile(getGameDataPath(GameDataType.RSB_BACKUP_FILE))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_warning_upper)
					.setContent("不存在RSB备份文件")
					.setPosBtn("仍然打包", (dialogInterface, i) -> {
						Util.FS.remove(getGameDataPath(GameDataType.RSB_FILE));
						createModEnv();
						runModXxtro("Outro");
					})
					.setCloseBtn()
					.show();
				return;
			}
			if (Util.FS.existFile(getGameDataPath(GameDataType.RSB_FILE))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_warning_upper)
					.setContent("已存在RSB文件")
					.setPosBtn("仍然打包，覆盖原有RSB", (dialogInterface, i) -> {
						Util.FS.remove(getGameDataPath(GameDataType.RSB_FILE));
						createModEnv();
						runModXxtro("Outro");
					})
					.setCloseBtn()
					.show();
				return;
			}
			new MyDialog(mContext)
				.setContent("是否执行RSB打包？")
				.setYesBtn((dialogInterface, i) -> {
					createModEnv();
					runModXxtro("Outro");
				})
				.setNoBtn()
				.show();
		});
		findViewById(R.id.mm_mod_apply).setOnClickListener(view -> {
			String testResult = testGameData();
			if (!testResult.isEmpty()) {
				new MyDialog(mContext)
					.setTitle(R.string.string_error_upper)
					.setContent(testResult)
					.setCloseBtn()
					.show();
				return;
			}
			if (!checkGameBackup()) {
				new MyDialog(mContext)
					.setTitle(R.string.string_error_upper)
					.setContent(R.string.error_game_backup_not_found_when_mod)
					.setCloseBtn()
					.show();
				return;
			}
			if (!Util.FS.existDir(getGameDataPath(GameDataType.RSB_UNPACK_DIR))) {
				new MyDialog(mContext)
					.setTitle(R.string.string_warning_upper)
					.setContent(R.string.warning_game_unpack_data_not_found_when_apply)
					.setYesBtn((dialogInterface, i) -> {
						createModEnv();
						runModApply();
					})
					.setNoBtn()
					.show();
			} else {
				createModEnv();
				runModApply();
			}
		});
		
		loadModList();
		showModList();
		
	}

	private void loadModList() {
		File[] fileList = new File(Util.TJ_MOD_DIR).listFiles();
		List<String> modNameList = new ArrayList<>();
		for (File file : fileList) {
			if (file.isDirectory() &&
				file.getName().endsWith(".tjpvz2mod")) {
				modNameList.add(file.getName().substring(0, file.getName().length() - 10));
			}
		}
		Collections.sort(modNameList);
		for (String e : modNameList)
			mModState.put(e, false);
		
	}

	private ListView mListView;
	
	private class ModAdapter extends BaseAdapter {
		
		private class ModViewHolder {
			CheckBox mCheckBox;
			TextView mTextView;
			
			public void setListener() {
				mCheckBox.setOnCheckedChangeListener((buttonView, isChecked) -> mModState.replace(mTextView.getText().toString(), isChecked));
				mTextView.setOnClickListener(view -> mCheckBox.performClick());
			}
		}
		
		LayoutInflater mInflater;

		ModAdapter(Context context) {
			mInflater = LayoutInflater.from(context);
		}

		@Override
		public int getCount() {
			return mModState.size();
		}

		@Override
		public int getItemViewType(int position) {
			return 0;
		}

		@Override
		public Object getItem(int position) {
			return Util.getMapItemN(mModState, position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int pos, View convertView, ViewGroup parent) {
			ModViewHolder holder;
			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.mod_item, parent, false);
				holder = new ModViewHolder();
				holder.mCheckBox = convertView.findViewById(R.id.mod_item_checkbox);
				holder.mTextView = convertView.findViewById(R.id.mod_item_textview);
				holder.setListener();
				convertView.setTag(holder);
			} else {
				holder = (ModViewHolder)convertView.getTag();
			}
			Map.Entry<String, Boolean> modItem = Util.getMapItemN(mModState, pos);
			holder.mTextView.setText(modItem.getKey());
			holder.mCheckBox.setChecked(modItem.getValue());
			return convertView;
		}
	}

	private void showModList() {
		mListView = this.findViewById(R.id.mm_mod_list);
		ModAdapter adapter = new ModAdapter(this);
		adapter.notifyDataSetChanged();
		mListView.setAdapter(adapter);
	}

}
