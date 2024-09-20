package com.poc.p_couds;

import android.app.ActionBar;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

public class OTA extends AppCompatActivity implements NavigationBarView.OnItemSelectedListener, View.OnClickListener {

    BottomNavigationView bottomNavigationView;

    EditText searchnTxt;
    TextView progressTxt;
    Button startDwBtn;
    ImageButton searchBtn, continueBtn, pauseBtn, stopBtn;
    Switch upgradeSw;
    ProgressBar progressDw;
    private int progressStatus = 0;
    private Handler handler = new Handler();

    Spinner spinnerVersions;
    List<String> versions;
    List<String> versionFilter;
    ArrayAdapter<String> adapterVersion;

    private CountDownTimer timer;
    private long timeRemaining;     // Track remaining time when paused
    private int totalTimeInMillis;  // Total time for the progress bar
    private boolean isPaused = false;

    String currentVersion = "1.5";


    // TODO Pop-up Logs when press a line in the table

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //EdgeToEdge.enable(this);
        setContentView(R.layout.activity_ota);
//        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
//            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
//            //v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
//            return insets;
//        });

//        bottomNavigationView
//                = findViewById(R.id.bottomNavigationView);
//        bottomNavigationView
//                .setOnItemSelectedListener(this);
//        bottomNavigationView.setSelectedItemId(R.id.mcu);

        searchnTxt = findViewById(R.id.search_txt);
        progressTxt = findViewById(R.id.progess_txt);

        searchBtn = findViewById(R.id.search_brn);
        searchBtn.setOnClickListener(this);
        startDwBtn = findViewById(R.id.start_download);
        startDwBtn.setOnClickListener(this);
        pauseBtn = findViewById(R.id.pause_b);
        pauseBtn.setOnClickListener(this);
        stopBtn = findViewById(R.id.stop_b);
        stopBtn.setOnClickListener(this);
        continueBtn = findViewById(R.id.continue_b);
        continueBtn.setOnClickListener(this);

        progressDw = findViewById(R.id.progressBar2);

        versionFilter = new ArrayList<>();
        spinnerVersions = findViewById(R.id.spinner_versions);
        adapterVersion = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1,versionFilter);
        spinnerVersions.setAdapter(adapterVersion);

        upgradeSw = findViewById(R.id.update_downgrade);
        upgradeSw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                versionFilter.clear();
                versionFilter.addAll(getVersionFiltered(b));
                adapterVersion.notifyDataSetChanged();
            }
        });

        getVersions();

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // Enable the back button
        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        // Handle toolbar navigation click using OnBackPressedDispatcher
        toolbar.setNavigationOnClickListener(v -> {
            // Use OnBackPressedDispatcher to handle back press
            getOnBackPressedDispatcher().onBackPressed();
        });
    }

    private int calculateSecondsDw(double size)
    {
        double speed_download = 1;  // 1Mb/s
        int time_s = (int) (size / speed_download);
        return time_s;
    }

    private void startProgressBar(int durationInMillis) {
        timeRemaining = durationInMillis;  // Set the time remaining

        timer = new CountDownTimer(timeRemaining, 100) {  // Update every 100ms
            public void onTick(long millisUntilFinished) {
                timeRemaining = millisUntilFinished;  // Update remaining time
                int progress = (int) ((totalTimeInMillis - millisUntilFinished) / (float) totalTimeInMillis * 100);
                progressTxt.setText(progress+"%");
                progressDw.setProgress(progress);
            }

            public void onFinish() {
                progressTxt.setText(100+"%");

                progressDw.setProgress(100);  // Set to 100% when done
            }
        }.start();
    }

    private List<String> getVersionFiltered(boolean upgrade)
    {
        List<String> verFilt = new ArrayList<>();
        if (upgrade)
        {
            for (String v1: versions)
            {
                if (!v1.equals(currentVersion))
                {
                    if (higher(v1, currentVersion))
                    {
                        verFilt.add(v1);
                    }
                }
            }
        } else{ //Downgrade
            for (String v1: versions)
            {
                if (!v1.equals(currentVersion))
                {
                    if (!higher(v1, currentVersion))
                    {
                        verFilt.add(v1);
                    }
                }
            }
        }
        return  verFilt;
    }

    private boolean higher(String v1, String v2)
    {
        String[] v1Split = v1.split("\\.");
        String[] v2Split = v2.split("\\.");

        for (int i = 0; i < v1Split.length; i++)
        {
            if (Integer.parseInt(v1Split[i]) > Integer.parseInt(v2Split[i]))
            {
                return true;
            } else if (Integer.parseInt(v1Split[i]) < Integer.parseInt(v2Split[i])) {
                return false;
            }
        }
        if(v2Split.length > v1Split.length)
        {
            return false;
        }
        return true;
    }

    private List<String> findVersionEqualTo(String version_txt)
    {
        List<String> list = new ArrayList<>();
        String[] v1Split = version_txt.split("\\.");
        for (String version: versions)
        {
            boolean flag_eq = true;
            String[] v2Split = version.split("\\.");
            for (int i = 0; i < v1Split.length; i++)
            {
                if(!Objects.equals(v1Split[i], v2Split[i]))
                {
                    flag_eq = false;
                }
            }
            if (flag_eq)
            {
                list.add(version);
            }
        }return list;
    }

    private void getVersions()
    {
        versions = Arrays.asList(new String[]{"0.1.1","1.1", "1.2","1.5", "1.7","2.0","5.0"});
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        return true;
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.start_download)
        {
            // endpoint start download
            totalTimeInMillis = calculateSecondsDw(3) * 1000; //for 30Mb
            startProgressBar(totalTimeInMillis);
            isPaused = false;

        } else if (view.getId() == R.id.continue_b) {
            //endpoint continue download
            if(isPaused)
            {
                startProgressBar((int)timeRemaining);
                isPaused = false;
            }
        } else if (view.getId() == R.id.pause_b) {
            //endpoint pause download
            if(!isPaused)
            {
                timer.cancel();
                isPaused = true;
            }
        } else if (view.getId() == R.id.stop_b) {
            //endpoint stop download
            timer.cancel();
            progressDw.setProgress(0);
            progressTxt.setText("");
            timeRemaining = totalTimeInMillis;

        } else if (view.getId() == R.id.search_brn) {
            String version_txt = searchnTxt.getText().toString();
            versionFilter.clear();
            versionFilter.addAll(findVersionEqualTo(version_txt));
            adapterVersion.notifyDataSetChanged();
        }
    }
}