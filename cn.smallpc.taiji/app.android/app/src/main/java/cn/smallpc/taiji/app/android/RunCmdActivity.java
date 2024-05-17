package cn.smallpc.taiji.app.android;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

import android.os.Bundle;

import com.google.android.material.tabs.TabLayout;
import com.google.android.material.tabs.TabLayoutMediator;

public class RunCmdActivity extends AppCompatActivity {
    
    private final int[] TAB_TITLE_STRING = { R.string.title_run_cmd_tab_shortcut, R.string.title_run_cmd_tab_script, R.string.title_run_cmd_tab_qe, R.string.title_run_cmd_tab_func };
    private final Fragment[] TAB_FRAGMENT = { new RunCmdShortcutFragment(), new RunCmdScriptFragment(), new RunCmdQEFragment(), new RunCmdFuncFragment() };
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_run_cmd);
        setTitle(getString(R.string.title_run_cmd));
        
        TabLayout tab = findViewById(R.id.tab);
        ViewPager2 paper = findViewById(R.id.paper);
        
        paper.setAdapter(new FragmentStateAdapter(this) {
            @NonNull
            @Override
            public Fragment createFragment(int position) {
                return TAB_FRAGMENT[position];
            }
            
            @Override
            public int getItemCount() {
                return TAB_FRAGMENT.length;
            }
        });
        new TabLayoutMediator(tab, paper, true, (tab1, position) -> tab1.setText(getString(TAB_TITLE_STRING[position]))).attach();
        
    }
    
}
