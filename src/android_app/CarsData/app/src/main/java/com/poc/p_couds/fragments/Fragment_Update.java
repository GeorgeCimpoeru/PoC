package com.poc.p_couds.fragments;

import android.app.Dialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.os.CountDownTimer;
import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.poc.p_couds.APIClient;
import com.poc.p_couds.IApiService;
import com.poc.p_couds.R;
import com.poc.p_couds.pojo.FileNode;
import com.poc.p_couds.pojo.UpdateHistory;
import com.poc.p_couds.pojo.UpdateV;
import com.poc.p_couds.pojo.UpdateVResponse;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_Update#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Fragment_Update extends Fragment implements View.OnClickListener {

    // Constants
    private final String TAG = "PoC";

    // Elements in view
    private EditText searchnTxt;
    private TextView progressTxt, ecuTxt, versionTxt;
    private Button startDwBtn;
    private ImageButton searchBtn, continueBtn, pauseBtn, stopBtn;
    private Switch upgradeSw;
    private ProgressBar progressDw;
    private TableLayout tableLayout;
    private Dialog mDialog;

    // Spinner and it's adapter
    private Spinner spinnerVersions;
    private List<Pair<String, String>> versions;
    private List<String> versionFilter;
    private ArrayAdapter<String> adapterVersion;

    // Variables
    private CountDownTimer timer;
    private long timeRemaining;     // Track remaining time when paused
    private int totalTimeInMillis;  // Total time for the progress bar
    private boolean isPaused = false;
    private boolean downloading = false;
    private IApiService apiInterface;
    private List<UpdateHistory> listHistoryUpdates = new ArrayList<>();

    // String variables
    private String folderNameVersions = "";
    private String idEcu;
    private String currentVersion = "1.5";


    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        // Initialize elements in the view
        initializeElements(view);

        // Get arguments from OTA page(ecu title,idEcu and location in the driver of the updates)
        Bundle args = getArguments();
        if (args != null){
            String title = args.getString("FRAGMENT_TITLE","Error");
            idEcu = args.getString("id","Error");
            folderNameVersions = args.getString("folderName", "Error");
            ecuTxt.setText(title);
            Toast.makeText(getActivity(), "ID: "+idEcu, Toast.LENGTH_SHORT).show();
        }

        // Get current version from API
        // getCurrentVersionFor(idEcu);
        displayCurrentVersions();

        // Get list of the versions from the API
        //getListOfVersionsAPI();

        // Get history of updates from endpoint and update table base on teh info received
        //getListOfUpdatesHistory(idEcu);
        
    }

    private void initializeElements(View view)
    {
        searchnTxt = view.findViewById(R.id.search_txt);
        progressTxt = view.findViewById(R.id.progess_txt);
        ecuTxt = view.findViewById(R.id.ecu_txt);
        versionTxt = view.findViewById(R.id.versio_txt);

        searchBtn = view.findViewById(R.id.search_btn);
        searchBtn.setOnClickListener(this);
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

    private void displayCurrentVersions()
    {
        versionTxt.setText("V"+currentVersion);
    }


    private void updateTable(List<UpdateHistory> listUpdates)
    {
        for(UpdateHistory update: listUpdates)
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
                int size = getSize();
                updateVersion();

                totalTimeInMillis = calculateMiliSecDw(size);
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

    private int getSize()
    {
        String currentVersionSelected = spinnerVersions.getSelectedItem().toString();
        int size = Integer.parseInt(versions.stream()
                .filter(pair -> pair.first.equals(currentVersionSelected))
                .map(pair -> pair.second)
                .findFirst()
                .orElse("0"));
        return size;
    }

    private int calculateMiliSecDw(double size)
    {
        double speed_download = 1000;  // 1b/ms
        return (int) (size / speed_download);
    }

    private void startProgressBar(int durationInMillis) {
        timeRemaining = durationInMillis;  // Set the time remaining
        progressDw.setMax(100);
        timer = new CountDownTimer(timeRemaining, 100) {  // Update every 100ms
            public void onTick(long millisUntilFinished) {
                timeRemaining = millisUntilFinished;  // Update remaining time
                int progress = (int) ((totalTimeInMillis - millisUntilFinished) / (float) totalTimeInMillis * 100);
                progressTxt.setText(progress+"%");
                progressDw.setProgress(progress);
            }

            public void onFinish() {
            }
        }.start();
    }

    private List<String> getVersionFiltered(boolean upgrade)
    {
        List<String> verFilt = new ArrayList<>();
        if (upgrade)
        {
            for (Pair<String,String> v1: versions)
            {
                if (!v1.equals(currentVersion))
                {
                    if (higher(v1.first, currentVersion))
                    {
                        verFilt.add(v1.first);
                    }
                }
            }
        } else{ //Downgrade
            for (Pair<String,String> v1: versions)
            {
                if (!v1.equals(currentVersion))
                {
                    if (!higher(v1.first, currentVersion))
                    {
                        verFilt.add(v1.first);
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
        return v2Split.length <= v1Split.length;
    }

    private List<String> findVersionEqualTo(String version_txt)
    {
        List<String> list = new ArrayList<>();
        String[] v1Split = version_txt.split("\\.");
        for (Pair<String,String> version: versions)
        {
            boolean flag_eq = true;
            String[] v2Split = version.first.split("\\.");
            for (int i = 0; i < v1Split.length; i++)
            {
                if (!Objects.equals(v1Split[i], v2Split[i])) {
                    flag_eq = false;
                    break;
                }
            }
            if (flag_eq)
            {
                list.add(version.first);
            }
        }return list;
    }

    private void updateVersion()
    {
        String version = spinnerVersions.getSelectedItem().toString();
        UpdateV updateV = new UpdateV(version, "zip",idEcu);
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<UpdateVResponse> call = apiInterface.updateVersion(updateV);
        call.enqueue(new Callback<UpdateVResponse>() {
            @Override
            public void onResponse(@NonNull Call<UpdateVResponse> call, @NonNull Response<UpdateVResponse> response) {
                UpdateVResponse updateVResponse = response.body();
                if (response.isSuccessful() && updateVResponse !=null)
                {
                    if (updateVResponse.getErrors().equals("No errors."))
                    {
                        Log.i(TAG, "Download completed");

                        progressTxt.setText(100+"%");
                        downloading = false;
                        progressDw.setProgress(100);  // Set to 100% when done
                        versionTxt.setText(spinnerVersions.getSelectedItem().toString());
                        currentVersion = spinnerVersions.getSelectedItem().toString();

                        Toast.makeText(Fragment_Update.this.getContext(), "Download completed", Toast.LENGTH_SHORT).show();
                    } else {
                        Log.e(TAG, updateVResponse.getErrors());
                        errorDownload();
                        Toast.makeText(Fragment_Update.this.getContext(), updateVResponse.getErrors(), Toast.LENGTH_SHORT).show();
                    }
                } else {
                    // Response was not successful or body is null
                    Log.e(TAG, "Error: Unsuccessful response or empty body. Code: " + response.code());
                    if (response.errorBody() != null) {
                        try {
                            // Optionally log the error body
                            Log.e("TAG", "Error body: " + response.errorBody().string());
                        } catch (IOException e) {
                            Log.e("TAG", "Error body: " + e.toString());
                        }
                    }
                    errorDownload();
                }
            }

            @Override
            public void onFailure(@NonNull Call<UpdateVResponse> call, @NonNull Throwable t) {
                Toast.makeText(Fragment_Update.this.getContext(), "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
                Toast.makeText(Fragment_Update.this.getContext(), call.request().url().toString(), Toast.LENGTH_LONG).show();
                errorDownload();
                call.cancel();
            }
        });

    }
    private void errorDownload()
    {
        downloading = false;
        versionTxt.setText("Error to download");
        Toast.makeText(Fragment_Update.this.getContext(), "Staus download error", Toast.LENGTH_SHORT).show();
    }

    private void getListOfVersionsAPI()
    {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<FileNode> call = apiInterface.requestListOfVersions();
        call.enqueue(new Callback<FileNode>() {
            @Override
            public void onResponse(@NonNull Call<FileNode> call, @NonNull Response<FileNode> response) {
                FileNode body = response.body();
                Log.i(TAG, response.code() + "");

                if (response.isSuccessful() && body != null)
                {
                    List<FileNode> children = body.getChildren();
                    versions = extractVersion(body,folderNameVersions);
                    if (versions != null) {
                        versionFilter.clear(); // Clear existing items in the filter
                        for (Pair<String, String> pair : versions) {
                            versionFilter.add(pair.first); // Add only the 'key' (first element) to the filter
                        }
                        adapterVersion.notifyDataSetChanged(); // Notify adapter about data changes
                    }
                    Log.i(TAG,"All good");
                }
            }

            @Override
            public void onFailure(@NonNull Call<FileNode> call, @NonNull Throwable t) {
                Toast.makeText(Fragment_Update.this.getContext(), "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
                Toast.makeText(Fragment_Update.this.getContext(), call.request().url().toString(), Toast.LENGTH_LONG).show();
                Log.e(TAG, "ERROR");
            }
        });
    }

    private List<Pair<String,String>> extractVersion(FileNode swVersions, String ecuFolderName)
    {
        if (!Objects.equals(ecuFolderName, "") || !Objects.equals(ecuFolderName, "Error"))
        {
            FileNode targetFolder = null;
            for (FileNode folder : swVersions.getChildren())
            {
                if (Objects.equals(folder.getName(), ecuFolderName))
                {
                    targetFolder = folder;
                    break;
                }
            }
            if (targetFolder != null)
            {
                List<Pair<String,String>> versions = new ArrayList<>();
                for (FileNode swVersion: targetFolder.getChildren())
                {
                    if (!Objects.equals(swVersion.getType(), "folder"))
                    {
                        versions.add(new Pair<>(swVersion.getSwVersion(), swVersion.getSize()));
                    }
                }
                return versions;
            }
        }
        return null;
    }

    private void getListOfUpdatesHistory(String idEcu)
    {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<List<UpdateHistory>> call = apiInterface.requestListOfUpdatesHistory();
        call.enqueue(new Callback<List<UpdateHistory>>() {
            @Override
            public void onResponse(@NonNull Call<List<UpdateHistory>> call, @NonNull Response<List<UpdateHistory>> response) {
                if (response.isSuccessful() && response.body() != null)
                {
                    listHistoryUpdates = response.body();
                    updateTable(listHistoryUpdates);
                    Log.i(TAG, "All good");
                }
            }

            @Override
            public void onFailure(@NonNull Call<List<UpdateHistory>> call, @NonNull Throwable t) {
                Toast.makeText(Fragment_Update.this.getContext(), "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
                Toast.makeText(Fragment_Update.this.getContext(), call.request().url().toString(), Toast.LENGTH_LONG).show();
                Log.e(TAG, "ERROR");
                call.cancel();
            }
        });

        // TODO !!!IMPORTANT!!! Delete when call to endpoint is implemented
//        listHistoryUpdates = Arrays.asList(new UpdateHistory[]{new UpdateHistory("Software update 1", "Succeeded", "1 Sept 2024", "23"),
//                new UpdateHistory("Software update 2", "Failed", "1 Ian 20243", "12")});
    }
}