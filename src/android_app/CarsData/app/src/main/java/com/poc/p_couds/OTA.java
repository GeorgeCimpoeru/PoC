package com.poc.p_couds;

import android.app.FragmentTransaction;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import androidx.fragment.app.Fragment;
import android.view.View;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;

public class OTA extends AppCompatActivity {

    BottomNavigationView bottomNavigationView;

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.toolbar_menu, menu);
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ota);

        bottomNavigationView
                = findViewById(R.id.bottomNavigationView);
        bottomNavigationView
                .setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
                                               @Override
                                               public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                                                   String title = "";

                                                   if(item.getItemId() == R.id.mcu)
                                                   {
                                                       title = "MCU";
                                                   } else if(item.getItemId() == R.id.battery)
                                                   {
                                                       title = "Battery";
                                                   } else if(item.getItemId() == R.id.engine)
                                                   {
                                                       title = "Engine";
                                                   }else if(item.getItemId() == R.id.door)
                                                   {
                                                       title = "Door";
                                                   }else if(item.getItemId() == R.id.hvc)
                                                   {
                                                       title = "HVC";
                                                   }
                                                   openFragment(new Fragment_Update(),title);
                                                   return true;
                                               }
                                           });
                        bottomNavigationView.setSelectedItemId(R.id.mcu);

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setTitle("OTA");
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Fragment fr = new Fragment_Update();

                getSupportFragmentManager().beginTransaction()
                        .replace(R.id.content_frame,fr)
                        .commit();
            }
        });


        Fragment fr = new Fragment_Update();
        openFragment(fr,"MCU");

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
    private void openFragment(Fragment fr, String title)
    {
        if (!title.equals(""))
        {
            Bundle bundle = new Bundle();
            bundle.putString("FRAGMENT_TITLE", title);
            fr.setArguments(bundle);
        }
        getSupportFragmentManager().beginTransaction()
                .replace(R.id.content_frame,fr)
                .commit();
    }
    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if(item.getItemId() == R.id.sendRe_btn)
        {
            Fragment fr = new Fragment_RequestSend();
            openFragment(fr,"");
        } else if(item.getItemId() == R.id.updates_btn)
        {
            Fragment fr = new Fragment_Update();
            openFragment(fr,"");
        }
        return super.onOptionsItemSelected(item);
    }
}