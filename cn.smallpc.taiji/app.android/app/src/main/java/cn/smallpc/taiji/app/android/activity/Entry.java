package cn.smallpc.taiji.app.android.activity;

import cn.smallpc.taiji.app.android.*;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Build;
import android.os.Bundle;
import android.content.Intent;

public class Entry extends AppCompatActivity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_entry);
		setTitle(getString(R.string.title_entry));
		
		{
			String abi = Build.SUPPORTED_ABIS[0];
			if (!abi.equals("arm64-v8a")) {
				new MyDialog(this)
					.setTitle(R.string.title_abi_unsupport)
					.setContent(R.string.tip_abi_unsupport_desc + '\n' + abi)
					.setCloseBtn((dialogInterface, i) -> {
						android.os.Process.killProcess(android.os.Process.myPid());
					})
					.show();
			}
		}
		
		findViewById(R.id.about).setOnClickListener(view -> {
			startActivity(new Intent(Entry.this, About.class));
		});
		findViewById(R.id.setting).setOnClickListener(view -> {
			startActivity(new Intent(Entry.this, SettingActivity.class));
		});
		findViewById(R.id.run_cmd).setOnClickListener(view -> {
			String s = Util.EnvTest.test(Entry.this);
			if (!s.isEmpty()) {
				new MyDialog(Entry.this)
					.setTitle(R.string.string_error_upper)
					.setContent(s)
					.setOKBtn()
					.show();
			} else {
				startActivity(new Intent(Entry.this, RunCmdActivity.class));
			}
		});
		findViewById(R.id.mod_manager).setOnClickListener(view -> {
			String s = Util.EnvTest.test(Entry.this);
			if (!s.isEmpty()) {
				new MyDialog(Entry.this)
					.setTitle(R.string.string_error_upper)
					.setContent(s)
					.setOKBtn()
					.show();
			} else {
				startActivity(new Intent(Entry.this, ModManagerActivity.class));
			}
		});
	}

}
