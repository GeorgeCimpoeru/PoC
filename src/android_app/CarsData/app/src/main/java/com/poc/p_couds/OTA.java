package com.poc.p_couds;

import android.app.FragmentTransaction;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.app.Fragment;
import android.view.View;

import com.google.android.material.bottomnavigation.BottomNavigationView;

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

//        bottomNavigationView
//                = findViewById(R.id.bottomNavigationView);
//        bottomNavigationView
//                .setOnItemSelectedListener(this);
//        bottomNavigationView.setSelectedItemId(R.id.mcu);

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setTitle("OTA");
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Fragment fr = new Fragment_Update();

                FragmentTransaction ft = getFragmentManager().beginTransaction();
                ft.replace(R.id.content_frame, fr);
                ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN);
                ft.addToBackStack(null);
                ft.commit();
            }
        });


        Fragment fr = new Fragment_Update();
        openFragment(fr);

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
    private void openFragment(Fragment fr)
    {
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.replace(R.id.content_frame, fr);
        ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN);
        ft.addToBackStack(null);
        ft.commit();
    }
    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if(item.getItemId() == R.id.sendRe_btn)
        {
            Fragment fr = new Fragment_RequestSend();
            openFragment(fr);
        } else if(item.getItemId() == R.id.updates_btn)
        {
            Fragment fr = new Fragment_Update();
            openFragment(fr);
        }
        return super.onOptionsItemSelected(item);
    }
}