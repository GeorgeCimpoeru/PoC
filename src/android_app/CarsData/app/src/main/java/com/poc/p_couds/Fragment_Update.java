package com.poc.p_couds;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.os.CountDownTimer;
import android.os.Handler;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_Update#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Fragment_Update extends Fragment implements View.OnClickListener {

    EditText searchnTxt;
    TextView progressTxt, ecuTxt, versionTxt;
    Button startDwBtn, MCUbtn;
    ImageButton searchBtn, continueBtn, pauseBtn, stopBtn;
    Switch upgradeSw;
    ProgressBar progressDw;
    TableLayout tableLayout;
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
    private boolean downloading = false;

    Dialog mDialog;

    String currentVersion = "1.5";

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        initializeElements(view);
        Bundle args = getArguments();
        if (args != null){
            String title = args.getString("FRAGMENT_TITLE","Error");
            ecuTxt.setText(title);
        }

        List<Update> list = Arrays.asList(new Update[]{new Update("Software update 1", "Succeeded", "1 Sept 2024", 23),
                new Update("Software update 2", "Faied", "1 Ian 20243", 12)});
        updateTable(list);

        getCurrentVersions();
        getAvailableVersions();
    }

    private void initializeElements(View view)
    {
        searchnTxt = view.findViewById(R.id.search_txt);
        progressTxt = view.findViewById(R.id.progess_txt);
        ecuTxt = view.findViewById(R.id.ecu_txt);
        versionTxt = view.findViewById(R.id.versio_txt);

        searchBtn = view.findViewById(R.id.search_btn);
        searchBtn.setOnClickListener(this);
        //MCUbtn = view.findViewById(R.id.mcu_btn);
        //MCUbtn.setOnClickListener(this);
        startDwBtn = view.findViewById(R.id.start_download);
        startDwBtn.setOnClickListener(this);
        pauseBtn = view.findViewById(R.id.pause_b);
        pauseBtn.setOnClickListener(this);
        stopBtn = view.findViewById(R.id.stop_b);
        stopBtn.setOnClickListener(this);
        continueBtn = view.findViewById(R.id.continue_b);
        continueBtn.setOnClickListener(this);

        progressDw = view.findViewById(R.id.progressBar2);

        versionFilter = new ArrayList<>();
        spinnerVersions = view.findViewById(R.id.spinner_versions);
        adapterVersion = new ArrayAdapter<>(view.getContext(), android.R.layout.simple_list_item_1,versionFilter);
        spinnerVersions.setAdapter(adapterVersion);

        upgradeSw = view.findViewById(R.id.update_downgrade);
        upgradeSw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                versionFilter.clear();
                versionFilter.addAll(getVersionFiltered(b));
                adapterVersion.notifyDataSetChanged();
            }
        });
        mDialog = new Dialog(view.getContext());

        tableLayout = view.findViewById(R.id.table);
    }

    private void getCurrentVersions()
    {
        versionTxt.setText("V"+currentVersion);
    }


    private void updateTable(List<Update> listUpdates)
    {
        for(Update update: listUpdates)
        {

            TableRow tableRow = new TableRow(getContext());

            tableRow.setPadding(5, 5, 5, 5);  // Add padding

            // Artifact TextView
            TextView textViewArtifact = new TextView(getContext());
            textViewArtifact.setText(update.getArtifact());
            textViewArtifact.setLayoutParams(new TableRow.LayoutParams(0, TableRow.LayoutParams.WRAP_CONTENT, 1f));  // Width 0dp, Weight 1
            tableRow.addView(textViewArtifact);

            // Status TextView
            TextView textViewStatus = new TextView(getContext());
            String status = update.getStatus();
            textViewStatus.setText(status);
            if (status.equals("Succeeded")) {
                textViewStatus.setTextColor(getResources().getColor(R.color.green));
            } else {
                textViewStatus.setTextColor(getResources().getColor(R.color.red));
            }
            textViewStatus.setLayoutParams(new TableRow.LayoutParams(0, TableRow.LayoutParams.WRAP_CONTENT, 1f));
            tableRow.addView(textViewStatus);

            // Start Time TextView
            TextView textViewStartTime = new TextView(getContext());
            textViewStartTime.setText(update.getStartTime());
            textViewStartTime.setLayoutParams(new TableRow.LayoutParams(0, TableRow.LayoutParams.WRAP_CONTENT, 1f));
            tableRow.addView(textViewStartTime);

            // Size TextView
            TextView textViewSize = new TextView(getContext());
            textViewSize.setText(String.valueOf(update.getSize()));
            textViewSize.setLayoutParams(new TableRow.LayoutParams(0, TableRow.LayoutParams.WRAP_CONTENT, 1f));
            tableRow.addView(textViewSize);


            tableRow.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    Toast.makeText(view.getContext(), update.getArtifact(), Toast.LENGTH_SHORT).show();

                    mDialog.setContentView(R.layout.pop_up_logs);
                    TextView textViewPopup = mDialog.findViewById(R.id.title);

                    // Set the text dynamically (e.g., based on the update artifact)
                    textViewPopup.setText(update.getArtifact());
                    mDialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
                    mDialog.show();
                }
            });

            //tableRow.setPadding(5,5,5,5);

            tableLayout.addView(tableRow);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_updates_ota, container, false);
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.start_download)
        {
            if (!downloading){
                // endpoint start download
                totalTimeInMillis = calculateSecondsDw(3) * 1000; //for 30Mb
                startProgressBar(totalTimeInMillis);
                isPaused = false;
                downloading = true;
            }
        } else if (view.getId() == R.id.continue_b) {
            //endpoint continue download
            if(isPaused && downloading)
            {
                startProgressBar((int)timeRemaining);
                isPaused = false;
            }
        } else if (view.getId() == R.id.pause_b) {
            //endpoint pause download
            if(!isPaused && downloading)
            {
                timer.cancel();
                isPaused = true;
            }
        } else if (view.getId() == R.id.stop_b) {
            if(downloading){
                //endpoint stop download
                timer.cancel();
                downloading = false;
                progressDw.setProgress(0);
                progressTxt.setText("");
                timeRemaining = totalTimeInMillis;
            }

        } else if (view.getId() == R.id.search_btn) {
            String version_txt = searchnTxt.getText().toString();
            versionFilter.clear();
            versionFilter.addAll(findVersionEqualTo(version_txt));
            adapterVersion.notifyDataSetChanged();
        }
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
                downloading = false;
                progressDw.setProgress(100);  // Set to 100% when done
                versionTxt.setText(spinnerVersions.getSelectedItem().toString());
                currentVersion = spinnerVersions.getSelectedItem().toString();
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
                if (!Objects.equals(v1Split[i], v2Split[i])) {
                    flag_eq = false;
                    break;
                }
            }
            if (flag_eq)
            {
                list.add(version);
            }
        }return list;
    }

    private void getAvailableVersions()
    {
        //get la api
        versions = Arrays.asList(new String[]{"0.1.1","1.1", "1.2","1.5", "1.7","2.0","5.0"});
        versionFilter.clear();
        versionFilter.addAll(versions);
        adapterVersion.notifyDataSetChanged();
    }
}