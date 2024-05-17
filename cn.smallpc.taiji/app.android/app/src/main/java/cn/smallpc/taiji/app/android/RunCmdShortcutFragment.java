package cn.smallpc.taiji.app.android;

import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import java.util.LinkedHashMap;
import java.util.List;

import cn.smallpc.taiji.app.android.activity.Entry;

public class RunCmdShortcutFragment extends Fragment {
	
	private static class ShortcutItem {
		String desc;
		List<String> cmd;
	}
	
	private LinkedHashMap<String, ShortcutItem> mShortcut;
	private ListView mViewList;
	
	private void init() {
		mShortcut = new Gson().fromJson(Util.FS.readString(Util.TJ_SHORTCUT_FILE), new TypeToken<LinkedHashMap<String, ShortcutItem>>() {}.getType());
		
		ShortcutAdapter adapter = new ShortcutAdapter(getContext());
		adapter.notifyDataSetChanged();
		mViewList.setAdapter(adapter);
		
	}
	
	@Override
	public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View view = View.inflate(getContext(), R.layout.fragment_run_cmd_shortcut, null);
		
		mViewList = view.findViewById(R.id.list);
		
		init();
		
		return view;
	}
	
	private String mCurShortcutName;
	
	private class ShortcutAdapter extends BaseAdapter {
		
		private class ShortcutViewHolder {
			TextView mName;
			Button mRun;
			
			public void setListener() {
				mName.setOnClickListener(view -> {
					new MyDialog(getContext())
						.setTitle(R.string.text_shortcut_desc)
						.setContent(mShortcut.get(mName.getText().toString()).desc)
						.setCloseBtn()
						.show();
				});
				mRun.setOnClickListener(view -> {
					mCurShortcutName = mName.getText().toString();
					List<String> cmd = mShortcut.get(mCurShortcutName).cmd;
					StringBuilder str = new StringBuilder();
					for (final String e : cmd)
						str.append(e).append('\n');
					new MyDialog(getContext())
						.setTitle(R.string.tip_run_cmd_or_not)
						.setContent(str.toString())
						.setYesBtn((dialogInterface, i) -> {
							String result = new LibLinker(getContext()).runEntryBySpecialForAndroid(mShortcut.get(mCurShortcutName).cmd);
							LibLinker.showInfoAfterRunScript(getContext(), result);
						})
						.setNoBtn()
						.show();
				});
			}
		}
		
		private final LayoutInflater mInflater;
		
		ShortcutAdapter(Context context) {
			mInflater = LayoutInflater.from(context);
		}
		
		@Override
		public int getCount() {
			return mShortcut.size();
		}
		
		@Override
		public int getItemViewType(int position) {
			return 0;
		}
		
		@Override
		public Object getItem(int position) {
			return mShortcut.get(Util.getMapItemN(mShortcut, position));
		}
		
		@Override
		public long getItemId(int position) {
			return position;
		}
		
		@Override
		public View getView(int pos, View convertView, ViewGroup parent) {
			ShortcutViewHolder holder;
			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.shortcut_item, parent, false);
				holder = new ShortcutViewHolder();
				holder.mName = convertView.findViewById(R.id.name);
				holder.mRun = convertView.findViewById(R.id.run);
				holder.setListener();
				convertView.setTag(holder);
			} else {
				holder = (ShortcutViewHolder)convertView.getTag();
			}
			holder.mName.setText(Util.getMapItemN(mShortcut, pos).getKey());
			return convertView;
		}
	}
	
}
