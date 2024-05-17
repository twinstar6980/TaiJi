package cn.smallpc.taiji.app.android;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import cn.smallpc.taiji.app.android.Config.ParmType;
import cn.smallpc.taiji.app.android.Config.ParmItem;

public class RunCmdFuncFragment extends Fragment {
	
	private String mCurScriptString = "";
	
	private EditText mCurEditText;
	private Spinner mSpinnerFunc;
	
	private ListView mViewParmList;
	private ArrayList mParmList;
	
	private final int REQUESTCODE_SELECTFILE = 0;
	
	private void selectFile() {
		Util.FS.selectFile(this, REQUESTCODE_SELECTFILE);
	}
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (resultCode == Activity.RESULT_OK) {
			if (requestCode == REQUESTCODE_SELECTFILE) {
				mCurEditText.setText(Util.FS.uriToPath(data.getData()));
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
	
	private List<Config.FuncInfo> mFuncInfoList;
	
	private void init() {
		mFuncInfoList = Config.load().func;
		
		ArrayAdapter<String> adapter = new ArrayAdapter<>(getContext(), R.layout.support_simple_spinner_dropdown_item);
		for (Config.FuncInfo e : mFuncInfoList)
			adapter.add(e.getFuncName());
		
		mSpinnerFunc.setAdapter(adapter);
	}
	
	@Override
	public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View view = View.inflate(getContext(), R.layout.fragment_run_cmd_func, null);
		
		mSpinnerFunc = view.findViewById(R.id.func);
		mSpinnerFunc.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
			@Override
			public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
				mParmList = genParmItemList(mFuncInfoList.get(i));
				createParmList();
			}
			@Override
			public void onNothingSelected(AdapterView<?> adapterView) {
			}
		});
		
		mViewParmList = view.findViewById(R.id.parm);
		
		view.findViewById(R.id.run).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				{
					int curFuncIdx = mSpinnerFunc.getSelectedItemPosition();
					
					JsonObject scriptObject = new JsonObject();
					
					scriptObject.addProperty("type", "call");
					{
						JsonArray funcNameArray = new JsonArray();
						for (String funcNameSub : mFuncInfoList.get(curFuncIdx).func)
							funcNameArray.add(funcNameSub);
						scriptObject.add("func", funcNameArray);
					}
					{
						JsonObject parmObject = new JsonObject();
						int i = 0;
						for (Map.Entry<String, String> entry : mFuncInfoList.get(curFuncIdx).parm.entrySet()) {
							switch (entry.getValue()) {
								default: break;
								case "Bool":
									parmObject.addProperty(entry.getKey(), ((ParmAdapter.BoolParmViewHolder)mViewParmList.getChildAt(i).getTag()).mVal.isChecked());
									break;
								case "Int":
									parmObject.addProperty(entry.getKey(), Long.parseLong(((ParmAdapter.IntParmViewHolder)mViewParmList.getChildAt(i).getTag()).mVal.getText().toString()));
									break;
								case "Str":
									parmObject.addProperty(entry.getKey(), ((ParmAdapter.StrParmViewHolder)mViewParmList.getChildAt(i).getTag()).mVal.getText().toString());
									break;
								case "Path": {
									JsonArray pathArray = new JsonArray();
									pathArray.add(((ParmAdapter.PathParmViewHolder)mViewParmList.getChildAt(i).getTag()).mVal.getText().toString());
									parmObject.add(entry.getKey(), pathArray);
									break;
								}
							}
							++i;
						}
						scriptObject.add("parm", parmObject);
					}
					mCurScriptString = new Gson().toJson(scriptObject);
				}
				new MyDialog(getContext())
					.setTitle(R.string.tip_run_cmd_or_not)
					.setContent(mCurScriptString)
					.setYesBtn((dialogInterface, i) -> {
						String result = new LibLinker(getContext()).runEntryBySpecialForAndroid("$" + mCurScriptString);
						LibLinker.showInfoAfterRunScript(getContext(), result);
					})
					.setNoBtn()
					.show();
			}
		});
		
		init();
		
		return view;
	}
	
	private void createParmList() {
		ParmAdapter adapter = new ParmAdapter(getContext());
		adapter.notifyDataSetChanged();
		mViewParmList.setAdapter(adapter);
	}
	
	private ArrayList<?> genParmItemList(Config.FuncInfo func) {
		ArrayList<ParmItem> parmItemList = new ArrayList<>();
		for (Map.Entry<String, String> e : func.parm.entrySet()) {
			ParmType ctrlType;
			switch (e.getValue()) {
				default: ctrlType = ParmType.UNKNOWN; break;
				case "Bool": ctrlType = ParmType.BOOL; break;
				case "Int": ctrlType = ParmType.INT; break;
				case "Str": ctrlType = ParmType.STR; break;
				case "Path": ctrlType = ParmType.PATH; break;
			}
			parmItemList.add(new Config.ParmItem(e.getKey(), ctrlType));
		}
		return parmItemList;
	}
	
	private class ParmAdapter extends BaseAdapter {
		
		private class BoolParmViewHolder {
			public TextView mKey;
			public CheckBox mVal;
		}
		private class IntParmViewHolder {
			public TextView mKey;
			public EditText mVal;
		}
		private class StrParmViewHolder {
			public TextView mKey;
			public EditText mVal;
		}
		private class PathParmViewHolder {
			public TextView mKey;
			public EditText mVal;
			public Button mSelect;
			
			public void initSelect() {
				mSelect.setOnClickListener(view -> {
					mCurEditText = mVal;
					selectFile();
				});
			}
		}
		
		LayoutInflater mInflater;
		
		ParmAdapter(Context context) {
			mInflater = LayoutInflater.from(context);
		}
		
		@Override
		public int getCount() {
			return mParmList.size();
		}
		
		@Override
		public int getItemViewType(int position) {
			return ((ParmItem) mParmList.get(position)).mType.ordinal();
		}
		
		@Override
		public Object getItem(int position) {
			return mParmList.get(position);
		}
		
		@Override
		public long getItemId(int position) {
			return position;
		}
		
		@Override
		public View getView(int pos, View convertView, ViewGroup parent) {
			BoolParmViewHolder holder_bool = null;
			IntParmViewHolder holder_int = null;
			StrParmViewHolder holder_string = null;
			PathParmViewHolder holder_path = null;
			ParmType type = ParmType.values()[getItemViewType(pos)];
			if (convertView == null) {
				switch (type) {
					case BOOL:
						convertView = mInflater.inflate(R.layout.parmslot_bool, parent, false);
						holder_bool = new BoolParmViewHolder();
						holder_bool.mKey = convertView.findViewById(R.id.parmslot_bool_key);
						holder_bool.mVal = convertView.findViewById(R.id.parmslot_bool_val);
						holder_bool.mKey.setText(((ParmItem) getItem(pos)).mName);
						holder_bool.mVal.setChecked(false);
						convertView.setTag(holder_bool);
						break;
					case INT:
						convertView = mInflater.inflate(R.layout.parmslot_int, parent, false);
						holder_int = new IntParmViewHolder();
						holder_int.mKey = convertView.findViewById(R.id.parmslot_int_key);
						holder_int.mVal = convertView.findViewById(R.id.parmslot_int_val);
						holder_int.mKey.setText(((ParmItem) getItem(pos)).mName);
						holder_int.mVal.setText("0");
						convertView.setTag(holder_int);
						break;
					case STR:
						convertView = mInflater.inflate(R.layout.parmslot_str, parent, false);
						holder_string = new StrParmViewHolder();
						holder_string.mKey = convertView.findViewById(R.id.parmslot_str_key);
						holder_string.mVal = convertView.findViewById(R.id.parmslot_str_val);
						holder_string.mKey.setText(((ParmItem)getItem(pos)).mName);
						holder_string.mVal.setText("");
						convertView.setTag(holder_string);
						break;
					case PATH:
						convertView = mInflater.inflate(R.layout.parmslot_path, parent, false);
						holder_path = new PathParmViewHolder();
						holder_path.mKey = convertView.findViewById(R.id.parmslot_path_key);
						holder_path.mVal = convertView.findViewById(R.id.parmslot_path_val);
						holder_path.mSelect = convertView.findViewById(R.id.parmslot_path_select);
						holder_path.mKey.setText(((ParmItem) getItem(pos)).mName);
						holder_path.mVal.setText("");
						holder_path.initSelect();
						convertView.setTag(holder_path);
						break;
				}
			} else {
				switch (type) {
					case BOOL:
						holder_bool = (BoolParmViewHolder)convertView.getTag();
						break;
					case INT:
						holder_int = (IntParmViewHolder)convertView.getTag();
						break;
					case STR:
						holder_string = (StrParmViewHolder)convertView.getTag();
						break;
					case PATH:
						holder_path = (PathParmViewHolder)convertView.getTag();
						break;
				}
			}
			return convertView;
		}
		
	}
	
}
