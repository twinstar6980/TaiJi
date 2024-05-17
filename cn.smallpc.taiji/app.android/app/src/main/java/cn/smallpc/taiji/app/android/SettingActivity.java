package cn.smallpc.taiji.app.android;

import android.Manifest;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.SwitchPreference;

public class SettingActivity extends AppCompatActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_setting);
		setTitle(this.getString(R.string.title_setting));
		getSupportFragmentManager()
				.beginTransaction()
				.replace(R.id.setting, new SettingsFragment())
				.commit();
	}
	
	public static class SettingsFragment extends PreferenceFragmentCompat {
		
		private SwitchPreference mPerfStoragePermission;
		private SwitchPreference mPerfInstallPermission;
		
		private boolean checkStoragePermission() {
			return Util.checkStoragePermission(getActivity());
		}
		private boolean checkInstallPermission() {
			return Util.checkInstallPermission(getActivity());
		}
		
		private final int REQUESTCODE_REQUESTPERMISSION_STORAGE = 1;
		private final int REQUESTCODE_REQUESTPERMISSION_INSTALL = 2;
		
		public void getStoragePermission() {
			if (!checkStoragePermission()) {
				this.requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUESTCODE_REQUESTPERMISSION_STORAGE);
			}
		}
		public void getInstallPermission() {
			if (!checkInstallPermission()) {
				Uri packageURI = Uri.parse("package:" + getActivity().getPackageName());
				Intent intent = new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES, packageURI);
				startActivityForResult(intent, REQUESTCODE_REQUESTPERMISSION_INSTALL);
			}
		}
		
		@Override
		public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
			if (requestCode == REQUESTCODE_REQUESTPERMISSION_STORAGE) {
				mPerfStoragePermission.setChecked(checkStoragePermission());
				Util.showToast(getActivity(), getString(mPerfInstallPermission.isChecked() ? R.string.permission_request_success : R.string.permission_request_failed));
			}
			super.onRequestPermissionsResult(requestCode, permissions, grantResults);
		}
		@Override
		public void onActivityResult(int requestCode, int resultCode, Intent data) {
			super.onActivityResult(requestCode, resultCode, data);
			if (requestCode == REQUESTCODE_REQUESTPERMISSION_INSTALL) {
				mPerfInstallPermission.setChecked(checkInstallPermission());
				Util.showToast(getActivity(), getString(mPerfInstallPermission.isChecked() ? R.string.permission_request_success : R.string.permission_request_failed));
			}
		}
		
		@Override
		public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
			addPreferencesFromResource(R.xml.pref_setting);
			
			mPerfStoragePermission = findPreference("storage_permission");
			mPerfInstallPermission = findPreference("install_permission");
			
			mPerfStoragePermission.setChecked(checkStoragePermission());
			mPerfInstallPermission.setChecked(checkInstallPermission());
			
			mPerfStoragePermission.setOnPreferenceClickListener(preference -> {
				if (((SwitchPreference)preference).isChecked()) {
					getStoragePermission();
				} else {
					((SwitchPreference)preference).setChecked(checkStoragePermission());
				}
				return true;
			});
			mPerfInstallPermission.setOnPreferenceClickListener(preference -> {
				if (((SwitchPreference)preference).isChecked()) {
					getInstallPermission();
				} else {
					((SwitchPreference)preference).setChecked(checkInstallPermission());
				}
				return true;
			});
		}
		
	}
	
}