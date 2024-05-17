package cn.smallpc.taiji.app.android;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

public class RunCmdScriptFragment extends Fragment {
	
	private EditText mEditScriptPath;
	private String mCurScriptPath;
	
	private final int REQUESTCODE_SELECTFILE = 0;
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (resultCode == Activity.RESULT_OK) {
			if (requestCode == REQUESTCODE_SELECTFILE) {
				mEditScriptPath.setText(Util.FS.uriToPath(data.getData()));
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
	
	@Override
	public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View view = View.inflate(getContext(), R.layout.fragment_run_cmd_script, null);
		
		mEditScriptPath = view.findViewById(R.id.path);
		
		view.findViewById(R.id.path_select).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				Util.FS.selectFile(RunCmdScriptFragment.this, REQUESTCODE_SELECTFILE);
			}
		});
		
		view.findViewById(R.id.run).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				mCurScriptPath = mEditScriptPath.getText().toString();
				
				if (!Util.FS.existFile(mCurScriptPath)) {
					new MyDialog(getContext())
						.setTitle(R.string.string_error_upper)
						.setContent(getString(R.string.tip_file_not_found) + " : " + mCurScriptPath)
						.setCloseBtn()
						.show();
					return;
				}
				
				new MyDialog(getContext())
					.setTitle(R.string.tip_run_cmd_or_not)
					.setContent(mCurScriptPath)
					.setYesBtn((dialogInterface, i) -> {
						String result = new LibLinker(getContext()).runEntryBySpecialForAndroid("#" + mCurScriptPath);
						LibLinker.showInfoAfterRunScript(getContext(), result);
					})
					.setNoBtn()
					.show();
			}
		});
		
		return view;
	}
	
}
