package cn.smallpc.taiji.app.android;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.google.gson.JsonObject;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

public class RunCmdQEFragment extends Fragment {
	
	private final LinkedHashMap<String, Set<String>> mOptionMap = new LinkedHashMap<>();
	private final LinkedHashMap<String, Set<String>> mOptionForBatchMap = new LinkedHashMap<>();
	
	private CheckBox mCheckIsBatch;
	private Spinner mSpinnerMainOption;
	private Spinner mSpinnerSubOption;
	
	private EditText mEditQEPath;
	
	private String mCurCmd;
	
	private final int REQUESTCODE_SELECTFILE = 0;
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (resultCode == Activity.RESULT_OK) {
			if (requestCode == REQUESTCODE_SELECTFILE) {
				mEditQEPath.setText(Util.FS.uriToPath(data.getData()));
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
	
	private void createOptionSpinner() {
		boolean isBatch = mCheckIsBatch.isChecked();
		
		ArrayAdapter<String> adapter = new ArrayAdapter<>(getContext(), R.layout.support_simple_spinner_dropdown_item);
		if (isBatch) {
			for (String e : mOptionForBatchMap.keySet())
				adapter.add(e);
		} else {
			for (String e : mOptionMap.keySet())
				adapter.add(e);
		}
		
		mSpinnerMainOption.setAdapter(adapter);
	}
	private void createSubOptionSpinner() {
		boolean isBatch = mCheckIsBatch.isChecked();
		String mainOption = mSpinnerMainOption.getSelectedItem().toString();
		
		ArrayAdapter<String> adapter = new ArrayAdapter<>(getContext(), R.layout.support_simple_spinner_dropdown_item);
		adapter.addAll((isBatch ? mOptionForBatchMap : mOptionMap).get(mainOption));
		
		mSpinnerSubOption.setAdapter(adapter);
	}
	
	private void init() {
		Config.QuickEnterConfig quickEnterConfig = Config.load().quickEnter;
		
		mOptionMap.clear();
		for (Map.Entry<String, LinkedHashMap<String, JsonObject>> e : quickEnterConfig.option.entrySet()) {
			mOptionMap.put(e.getKey(), e.getValue().keySet());
		}
		
		mOptionForBatchMap.clear();
		for (Map.Entry<String, LinkedHashMap<String, JsonObject>> e : quickEnterConfig.optionForBatch.entrySet()) {
			mOptionForBatchMap.put(e.getKey(), e.getValue().keySet());
		}
		
		createOptionSpinner();
	}
	
	@Override
	public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View view = View.inflate(getContext(), R.layout.fragment_run_cmd_qe, null);
		
		mEditQEPath = view.findViewById(R.id.path);
		
		mCheckIsBatch = view.findViewById(R.id.is_batch);
		mSpinnerMainOption = view.findViewById(R.id.option_main);
		mSpinnerSubOption = view.findViewById(R.id.option_sub);
		
		mCheckIsBatch.setOnCheckedChangeListener((compoundButton, b) -> createOptionSpinner());
		mSpinnerMainOption.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
			@Override
			public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
				createSubOptionSpinner();
			}
			@Override
			public void onNothingSelected(AdapterView<?> adapterView) {
			}
		});
		
		view.findViewById(R.id.path_select).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				Util.FS.selectFile(RunCmdQEFragment.this, REQUESTCODE_SELECTFILE);
			}
		});
		
		view.findViewById(R.id.run).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				String qePath = mEditQEPath.getText().toString();
				
				mCurCmd = qePath;
				
				if (!Util.FS.exist(qePath)) {
					new MyDialog(getContext())
						.setTitle(R.string.string_error_upper)
						.setContent(getString(R.string.tip_path_not_found) + " : " + qePath)
						.setOKBtn()
						.show();
					return;
				}
				
				new MyDialog(getContext())
					.setTitle(R.string.tip_run_cmd_or_not)
					.setContent(mCurCmd)
					.setYesBtn((dialogInterface, i) -> {
						String result = new LibLinker(getContext()).runEntryBySpecialForAndroid("<" + mCurCmd);
						LibLinker.showInfoAfterRunScript(getContext(), result);
					})
					.setNoBtn()
					.show();
			}
		});
		
		view.findViewById(R.id.run_with_path).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				String qePath = mEditQEPath.getText().toString();
				
				mCurCmd = qePath + "?";
				
				if (mCheckIsBatch.isChecked()) {
					if (!Util.FS.existDir(qePath)) {
						new MyDialog(getContext())
							.setTitle(R.string.string_error_upper)
							.setContent(getString(R.string.tip_dir_not_found) + " : " + qePath)
							.setOKBtn()
							.show();
						return;
					}
					mCurCmd += '%';
				} else {
					if (!Util.FS.exist(qePath)) {
						new MyDialog(getContext())
							.setTitle(R.string.string_error_upper)
							.setContent(getString(R.string.tip_path_not_found) + " : " + qePath)
							.setOKBtn()
							.show();
						return;
					}
				}
				
				mCurCmd += mSpinnerMainOption.getSelectedItem().toString() + ';' + mSpinnerSubOption.getSelectedItem().toString();
				
				new MyDialog(getContext())
					.setTitle(R.string.tip_run_cmd_or_not)
					.setContent(mCurCmd)
					.setYesBtn((dialogInterface, i) -> {
						String result = new LibLinker(getContext()).runEntryBySpecialForAndroid("<" + mCurCmd);
						LibLinker.showInfoAfterRunScript(getContext(), result);
					})
					.setNoBtn()
					.show();
			}
		});
		
		init();
		
		return view;
	}
	
}
