package com.poc.p_couds.activities;

import android.os.Bundle;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.Fragment;

import com.google.android.material.tabs.TabLayout;
import com.poc.p_couds.R;
import com.poc.p_couds.fragments.Fragment_RequestSendAutomated;
import com.poc.p_couds.fragments.ManualFragment;

import java.util.Objects;

public class RequestActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_request);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main_request), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });


        TabLayout tabLayout = findViewById(R.id.button_bar);

        tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                Toast.makeText(RequestActivity.this, tab.getText(), Toast.LENGTH_SHORT).show();
                Fragment fr = null;
                if (Objects.requireNonNull(tab.getText()).toString().equals("Manual")){
                    fr = new ManualFragment();
                }
                else if (Objects.requireNonNull(tab.getText()).toString().equals("Semi-Automated")){
                    fr = new Fragment_RequestSendAutomated();
                }

                assert fr != null;
                getSupportFragmentManager().beginTransaction()
                    .replace(R.id.content_frame, fr)
                    .commit();
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {
            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {
                Toast.makeText(RequestActivity.this, tab.getText(), Toast.LENGTH_SHORT).show();
                Fragment fr = null;
                if (Objects.requireNonNull(tab.getText()).toString().equals("Manual")){
                    fr = new ManualFragment();
                }
                else if (Objects.requireNonNull(tab.getText()).toString().equals("Semi-Automated")){
                    fr = new Fragment_RequestSendAutomated();
                }

                assert fr != null;
                getSupportFragmentManager().beginTransaction()
                        .replace(R.id.content_frame, fr)
                        .commit();
            }
        });
        // Delay tab selection to ensure the layout is fully rendered

        Objects.requireNonNull(tabLayout.getTabAt(0)).select();  // Select the first tab

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        Objects.requireNonNull(getSupportActionBar()).setTitle("Send Requests");

        // Handle toolbar navigation click using OnBackPressedDispatcher
        toolbar.setNavigationOnClickListener(v -> {
            // Check if there are fragments in the back stack
            if (getSupportFragmentManager().getBackStackEntryCount() > 0) {
                // Pop the fragment from the back stack
                getSupportFragmentManager().popBackStack();
            } else {
                // If no fragments in back stack, delegate to the system's default back button behavior
                finish();
            }
        });
    }
}