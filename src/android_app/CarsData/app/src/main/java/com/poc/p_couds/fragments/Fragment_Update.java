package com.poc.p_couds.fragments;

import android.app.Dialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;

<<<<<<< HEAD
import androidx.annotation.NonNull;
=======
import androidx.activity.OnBackPressedCallback;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
>>>>>>> development
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
<<<<<<< HEAD

import com.poc.p_couds.APIClient;
import com.poc.p_couds.IApiService;
=======
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;

import com.poc.p_couds.APIClient;
<<<<<<< HEAD
import com.poc.p_couds.UpdateViewModel;
import com.poc.p_couds.UpdateViewModelFactory;
=======
>>>>>>> Database of history updates (#486)
import com.poc.p_couds.models.DbHelper;
import com.poc.p_couds.models.IApiService;
>>>>>>> development
import com.poc.p_couds.R;
import com.poc.p_couds.pojo.FileNode;
import com.poc.p_couds.pojo.UpdateHistory;
import com.poc.p_couds.pojo.UpdateV;
import com.poc.p_couds.pojo.UpdateVResponse;

import java.io.IOException;
import java.util.ArrayList;
<<<<<<< HEAD
<<<<<<< HEAD
=======
import java.util.Calendar;
>>>>>>> development
=======
import java.util.Calendar;
>>>>>>> Database of history updates (#486)
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

<<<<<<< HEAD
/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_Update#newInstance} factory method to
 * create an instance of this fragment.
 */
=======
>>>>>>> development
public class Fragment_Update extends Fragment implements View.OnClickListener {

    // Constants
    private final String TAG = "PoC";

    // Elements in view
    private EditText searchnTxt;
    private TextView progressTxt, ecuTxt, versionTxt;
<<<<<<< HEAD
<<<<<<< HEAD
    private Button startDwBtn;
=======
    private Button startDwBtn, nextBtn, firstButton;
>>>>>>> development
=======
    private Button startDwBtn, nextBtn, firstButton;
>>>>>>> Database of history updates (#486)
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
<<<<<<< HEAD
    private CountDownTimer timer;
    private long timeRemaining;     // Track remaining time when paused
    private int totalTimeInMillis;  // Total time for the progress bar
=======
>>>>>>> development
    private boolean isPaused = false;
    private boolean downloading = false;
    private IApiService apiInterface;
    private List<UpdateHistory> listHistoryUpdates = new ArrayList<>();
<<<<<<< HEAD
<<<<<<< HEAD
=======
    private DbHelper db;
    private int page = 0;   //page in the table
    private UpdateViewModel updateViewModel;
>>>>>>> development
=======
    private DbHelper db;
    private int page = 0;   //page in the table
>>>>>>> Database of history updates (#486)

    // String variables
    private String folderNameVersions = "";
    private String idEcu;
    private String currentVersion = "1.5";

<<<<<<< HEAD
=======
    public boolean isDownloading()
    {
        return downloading;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requireActivity().getOnBackPressedDispatcher().addCallback(this, new OnBackPressedCallback(true) {
            @Override
            public void handleOnBackPressed() {
                if (downloading) {
                    // Show a confirmation dialog
                    new AlertDialog.Builder(requireContext())
                            .setTitle("Download in progress")
                            .setMessage("A download is in progress. Do you really want to leave?")
                            .setPositiveButton("Yes", (dialog, which) -> {
                                //stopDownload();
                                setEnabled(false);
                                requireActivity().onBackPressed();  // Allow navigation
                            })
                            .setNegativeButton("No", null)  // Keep the user in the fragment
                            .show();
                } else {
                    setEnabled(false);
                    requireActivity().onBackPressed();  // Allow normal back navigation
                }
            }
        });
    }
>>>>>>> development

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
<<<<<<< HEAD
            ecuTxt.setText(title);
            Toast.makeText(getActivity(), "ID: "+idEcu, Toast.LENGTH_SHORT).show();
        }
=======
            versions = (ArrayList<Pair<String, String>>) args.getSerializable("versions");
            versionFilter.clear();
            for (Pair<String, String> pair : versions) {
                versionFilter.add(pair.first); // Add only the 'key' (first element) to the filter
            }
            adapterVersion.notifyDataSetChanged(); // Notify adapter about data changes
            ecuTxt.setText(title);
            Toast.makeText(getActivity(), "ID: "+idEcu, Toast.LENGTH_SHORT).show();
        }
        Log.i(TAG,"IMP: "+ idEcu);
        UpdateViewModelFactory factory = new UpdateViewModelFactory(getActivity().getApplication(), idEcu);
        updateViewModel = new ViewModelProvider(getActivity(),factory).get(UpdateViewModel.class);
        observeViewModel();
>>>>>>> development

        // Get current version from API
        // getCurrentVersionFor(idEcu);
        displayCurrentVersions();

<<<<<<< HEAD
        // Get list of the versions from the API
        getListOfVersionsAPI();

        // Get history of updates from endpoint and update table base on teh info received
        getListOfUpdatesHistory(idEcu);
        
    }

    @Override
    public void onStop() {
        super.onStop();
        db.close();
    }

    private void initializeElements(View view)
    {
<<<<<<< HEAD
=======
        // Get history of updates from endpoint and update table base on teh info received
        updateViewModel.getListOfUpdatesHistory(idEcu);
        
    }

    @Override
    public void onStop() {
        super.onStop();
        db.close();
    }

    private void initializeElements(View view)
    {
        versions = new ArrayList<>();
        db = new DbHelper(getActivity());
>>>>>>> development
=======
        versions = new ArrayList<>();
        db = new DbHelper(getActivity());
>>>>>>> Database of history updates (#486)
        searchnTxt = view.findViewById(R.id.search_txt);
        progressTxt = view.findViewById(R.id.progess_txt);
        ecuTxt = view.findViewById(R.id.ecu_txt);
        versionTxt = view.findViewById(R.id.versio_txt);

<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> Database of history updates (#486)
        nextBtn = view.findViewById(R.id.nextPageButton);
        nextBtn.setOnClickListener(this);
        firstButton = view.findViewById(R.id.previousPageButton);
        firstButton.setOnClickListener(this);
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> Database of history updates (#486)
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
<<<<<<< HEAD
=======
        progressDw.setMax(100);
>>>>>>> development

        versionFilter = new ArrayList<>();
        spinnerVersions = view.findViewById(R.id.spinner_versions);
        adapterVersion = new ArrayAdapter<>(view.getContext(), android.R.layout.simple_list_item_1,versionFilter);
        spinnerVersions.setAdapter(adapterVersion);

        upgradeSw = view.findViewById(R.id.update_downgrade);
        upgradeSw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
<<<<<<< HEAD
<<<<<<< HEAD
                versionFilter.clear();
                versionFilter.addAll(getVersionFiltered(b));
                adapterVersion.notifyDataSetChanged();
=======
=======
>>>>>>> Database of history updates (#486)
                if (!versions.isEmpty())
                {
                    versionFilter.clear();
                    versionFilter.addAll(getVersionFiltered(b));
                    adapterVersion.notifyDataSetChanged();
                }
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> Database of history updates (#486)
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
<<<<<<< HEAD
<<<<<<< HEAD
        for(UpdateHistory update: listUpdates)
        {
=======
        final int elemPerPage = 3;

        if (listUpdates.size()/elemPerPage + (listUpdates.size()%elemPerPage > 0 ? 1:0)  < page +1)
        {
            Toast.makeText(getContext(), "This are the last elements", Toast.LENGTH_SHORT).show();
            return;
        }
        int count = tableLayout.getChildCount();
        for (int i = 2; i < count; i++) {
            View child = tableLayout.getChildAt(i);
            if (child instanceof TableRow)
            {
                tableLayout.removeViewAt(i);
                i --;
            }
        }

        for (int i = 0; i < elemPerPage; i++)
        {
=======
        final int elemPerPage = 3;

        if (listUpdates.size()/elemPerPage + (listUpdates.size()%elemPerPage > 0 ? 1:0)  < page +1)
        {
            Toast.makeText(getContext(), "This are the last elements", Toast.LENGTH_SHORT).show();
            return;
        }
        int count = tableLayout.getChildCount();
        for (int i = 2; i < count; i++) {
            View child = tableLayout.getChildAt(i);
            if (child instanceof TableRow)
            {
                tableLayout.removeViewAt(i);
                i --;
            }
        }

        for (int i = 0; i < elemPerPage; i++)
        {
>>>>>>> Database of history updates (#486)
            if (i + (elemPerPage * page) >= listUpdates.size())
                break;

            UpdateHistory update = listUpdates.get(i + (elemPerPage * page));
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> Database of history updates (#486)

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
<<<<<<< HEAD
<<<<<<< HEAD

                    // Set the text dynamically (e.g., based on the update artifact)
                    textViewPopup.setText(update.getArtifact());
=======
=======
>>>>>>> Database of history updates (#486)
                    TextView textViewStatusPopup = mDialog.findViewById(R.id.statusDwTxt);

                    // Set the text dynamically (e.g., based on the update artifact)
                    textViewPopup.setText(update.getArtifact());
                    textViewStatusPopup.setText(update.getStatus());
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> Database of history updates (#486)
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

<<<<<<< HEAD
=======
    private void observeViewModel()
    {
        updateViewModel.getUpdateResponse().observe(getViewLifecycleOwner(), this::handleUpdateResponse);
        updateViewModel.getErrorMessage().observe(getViewLifecycleOwner(), this::handleErrorMessage);
        updateViewModel.getProgress().observe(getViewLifecycleOwner(), new Observer<Integer>() {
            @Override
            public void onChanged(Integer progress) {
                progressDw.setProgress(progress, true);
                progressTxt.setText(progress+"%");
            }
        });
        updateViewModel.isNewData().observe(getViewLifecycleOwner(), new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean isNewData) {
                if (isNewData)
                {
                    Toast.makeText(getContext(), "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
//                    Toast.makeText(Fragment_Update.this.getContext(), call.request().url().toString(), Toast.LENGTH_LONG).show();
                }
                listHistoryUpdates = db.getAllUpgrades();
                page = 0;
                updateTable(listHistoryUpdates);
            }
        });
    }

    private void updateVersion() {
        String version = spinnerVersions.getSelectedItem().toString();
        UpdateV updateV = new UpdateV(version, "zip", idEcu);
        String size = getSize() != -1? String.valueOf(getSize()) : "Unknown size";

        // Call ViewModel method to start update
        updateViewModel.updateVersion(updateV, size);
    }

    private void handleUpdateResponse(Response response)
    {
        UpdateVResponse updateVResponse = (UpdateVResponse)response.body();
        if (response.isSuccessful() && updateVResponse !=null)
        {
            if (updateVResponse.getErrors() != null && updateVResponse.getErrors().equals("No errors."))
            {
                Log.i(TAG, "Download completed");
                progressTxt.setText(100+"%");
                downloading = false;
                progressDw.setProgress(100);  // Set to 100% when done
                versionTxt.setText(spinnerVersions.getSelectedItem().toString());
                currentVersion = spinnerVersions.getSelectedItem().toString();
                Toast.makeText(Fragment_Update.this.getContext(), "Download completed", Toast.LENGTH_SHORT).show();

            } else {
                if (updateVResponse.getErrors() != null)
                {
                    Log.e(TAG, updateVResponse.getErrors());
                    Toast.makeText(Fragment_Update.this.getContext(), updateVResponse.getErrors(), Toast.LENGTH_SHORT).show();
                }
                errorDownload();
            }
            listHistoryUpdates = db.getAllUpgrades();
            page = 0;
            updateTable(listHistoryUpdates);
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
    private void handleErrorMessage(String error)
    {
        Toast.makeText(getContext(), "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
        Toast.makeText(getContext(), error, Toast.LENGTH_LONG).show();
        errorDownload();
    }

    private int calculateMiliSecDw(double size)
    {
        if (size >= 0)
        {
            double speed_download = 1000;  // 1b/ms
            return (int) (size / speed_download);
        } else{
            return 10000;
        }
    }

>>>>>>> development
    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.start_download)
        {
<<<<<<< HEAD
<<<<<<< HEAD
            if (!downloading){
=======
            if (!downloading && isSelectedVersion()){
>>>>>>> development
=======
            if (!downloading && isSelectedVersion()){
>>>>>>> Database of history updates (#486)
                // endpoint start download
                int size = getSize();
                updateVersion();

<<<<<<< HEAD
                totalTimeInMillis = calculateMiliSecDw(size);
<<<<<<< HEAD
                startProgressBar(totalTimeInMillis);
=======
                updateViewModel.startProgressBar(calculateMiliSecDw(size));
>>>>>>> development
=======
                //startProgressBar(totalTimeInMillis);
>>>>>>> Database of history updates (#486)
                isPaused = false;
                downloading = true;
            }
        } else if (view.getId() == R.id.continue_b) {
            //endpoint continue download
            if(isPaused && downloading)
            {
<<<<<<< HEAD
                startProgressBar((int)timeRemaining);
=======
                updateViewModel.resumeProgressBar();
>>>>>>> development
                isPaused = false;
            }
        } else if (view.getId() == R.id.pause_b) {
            //endpoint pause download
            if(!isPaused && downloading)
            {
<<<<<<< HEAD
                timer.cancel();
=======
                updateViewModel.pauseProgressBar();
>>>>>>> development
                isPaused = true;
            }
        } else if (view.getId() == R.id.stop_b) {
            if(downloading){
                //endpoint stop download
<<<<<<< HEAD
                timer.cancel();
                downloading = false;
                progressDw.setProgress(0);
                progressTxt.setText("");
                timeRemaining = totalTimeInMillis;
=======
                stopDownload();
>>>>>>> development
            }

        } else if (view.getId() == R.id.search_btn) {
            String version_txt = searchnTxt.getText().toString();
            versionFilter.clear();
            versionFilter.addAll(findVersionEqualTo(version_txt));
            adapterVersion.notifyDataSetChanged();
<<<<<<< HEAD
<<<<<<< HEAD
        }
    }

=======
=======
>>>>>>> Database of history updates (#486)
        } else if (view.getId() == R.id.nextPageButton) {
            listHistoryUpdates = db.getAllUpgrades();
            page++;
            updateTable(listHistoryUpdates);
        } else if (view.getId() == R.id.previousPageButton) {
            listHistoryUpdates = db.getAllUpgrades();
            page = 0;
            updateTable(listHistoryUpdates);
        }
    }

    public void stopDownload()
    {
        updateViewModel.stopProgressBar();
        downloading = false;
    }

>>>>>>> development
    private int getSize()
    {
        String currentVersionSelected = spinnerVersions.getSelectedItem().toString();
        int size = Integer.parseInt(versions.stream()
                .filter(pair -> pair.first.equals(currentVersionSelected))
                .map(pair -> pair.second)
                .findFirst()
<<<<<<< HEAD
<<<<<<< HEAD
                .orElse("0"));
=======
                .orElse("-1"));
>>>>>>> Database of history updates (#486)
        return size;
    }

    private int calculateMiliSecDw(double size)
    {
        if (size >= 0)
        {
            double speed_download = 1000;  // 1b/ms
            return (int) (size / speed_download);
        } else{
            return 10000;
        }
    }

    private void startProgressBar(int durationInMillis) {
        timeRemaining = durationInMillis;  // Set the time remaining
        progressDw.setMax(100);
        timer = new CountDownTimer(timeRemaining, 100) {  // Update every 100ms
            public void onTick(long millisUntilFinished) {
                if (downloading)
                {
                    timeRemaining = millisUntilFinished;  // Update remaining time
                    int progress = (int) ((totalTimeInMillis - millisUntilFinished) / (float) totalTimeInMillis * 100);
                    progressTxt.setText(progress+"%");
                    progressDw.setProgress(progress);

                }
                else {
                    onFinish();
                }
            }

            public void onFinish() {
            }
        }.start();
    }

=======
                .orElse("-1"));
        return size;
    }

>>>>>>> development
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

<<<<<<< HEAD
<<<<<<< HEAD
=======
    private boolean isSelectedVersion()
    {
        return spinnerVersions.getCount() > 0;
    }

    private String getSelectedVersion()
    {
        if (isSelectedVersion())
        {
            String version = spinnerVersions.getSelectedItem().toString();
            String size = getSize() != -1? String.valueOf(getSize()) : "Unknown size";
            return version;
        }
        return "";
    }

>>>>>>> Database of history updates (#486)
    private void updateVersion()
    {
        String version = spinnerVersions.getSelectedItem().toString();
        String size = getSize() != -1? String.valueOf(getSize()) : "Unknown size";
        UpdateV updateV = new UpdateV(version, "zip",idEcu);
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<UpdateVResponse> call = apiInterface.updateVersion(updateV);
        call.enqueue(new Callback<UpdateVResponse>() {
            @Override
            public void onResponse(@NonNull Call<UpdateVResponse> call, @NonNull Response<UpdateVResponse> response) {
                UpdateVResponse updateVResponse = response.body();
                if (response.isSuccessful() && updateVResponse !=null)
                {
                    Calendar calendar = Calendar.getInstance();
                    String data = String.valueOf(calendar.get(Calendar.DAY_OF_MONTH)) + "/"
                            + String.valueOf(calendar.get(Calendar.MONTH)) + "/"
                            + String.valueOf(calendar.get(Calendar.YEAR));
                    String artifact = "Software update "+ version + "(" + db.getAllUpgrades().size() + ")";
                    if (updateVResponse.getErrors() != null && updateVResponse.getErrors().equals("No errors."))
                    {
                        Log.i(TAG, "Download completed");

                        getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                progressTxt.setText(100+"%");
                                downloading = false;
                                progressDw.setProgress(100);  // Set to 100% when done
                                versionTxt.setText(spinnerVersions.getSelectedItem().toString());
                                currentVersion = spinnerVersions.getSelectedItem().toString();
                                Toast.makeText(Fragment_Update.this.getContext(), "Download completed", Toast.LENGTH_SHORT).show();
                                db.addNewUpdate(new UpdateHistory(artifact,"Succeeded",data,size));
                            }
                        });
                    } else {
                        if (updateVResponse.getErrors() != null)
                        {
                            Log.e(TAG, updateVResponse.getErrors());
                            Toast.makeText(Fragment_Update.this.getContext(), updateVResponse.getErrors(), Toast.LENGTH_SHORT).show();
                        }
                        db.addNewUpdate(new UpdateHistory(artifact,"Failed",data,size));
                        errorDownload();
                    }
                    listHistoryUpdates = db.getAllUpgrades();
                    page = 0;
                    updateTable(listHistoryUpdates);
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
        progressDw.setProgress(0);
        progressTxt.setText("0%");
        versionTxt.setText("Error to download");
        Toast.makeText(Fragment_Update.this.getContext(), "Status download error", Toast.LENGTH_SHORT).show();
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
                    List<UpdateHistory> listOfUp = response.body();
                    //save in the DB the updates that are not registered
                    for (UpdateHistory update : listOfUp)
                    {
                        db.addNewUpdate(update);
                    }
                    Log.i(TAG, "All good");
                }
                listHistoryUpdates = db.getAllUpgrades();
                page = 0;
                updateTable(listHistoryUpdates);
            }

            @Override
            public void onFailure(@NonNull Call<List<UpdateHistory>> call, @NonNull Throwable t) {
                Toast.makeText(Fragment_Update.this.getContext(), "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
                Toast.makeText(Fragment_Update.this.getContext(), call.request().url().toString(), Toast.LENGTH_LONG).show();
                Log.e(TAG, "ERROR");
                listHistoryUpdates = db.getAllUpgrades();
                page = 0;
                updateTable(listHistoryUpdates);
                call.cancel();
            }
        });
<<<<<<< HEAD

        // TODO !!!IMPORTANT!!! Delete when call to endpoint is implemented
//        listHistoryUpdates = Arrays.asList(new UpdateHistory[]{new UpdateHistory("Software update 1", "Succeeded", "1 Sept 2024", "23"),
//                new UpdateHistory("Software update 2", "Failed", "1 Ian 20243", "12")});
=======
    private boolean isSelectedVersion()
    {
        return spinnerVersions.getCount() > 0;
    }

    private String getSelectedVersion()
    {
        if (isSelectedVersion())
        {
            String version = spinnerVersions.getSelectedItem().toString();
            String size = getSize() != -1? String.valueOf(getSize()) : "Unknown size";
            return version;
        }
        return "";
    }

    private void errorDownload()
    {
        downloading = false;
        progressDw.setProgress(0);
        progressTxt.setText("0%");
        versionTxt.setText("Error to download");
        Toast.makeText(Fragment_Update.this.getContext(), "Status download error", Toast.LENGTH_SHORT).show();
>>>>>>> development
=======
>>>>>>> Database of history updates (#486)
    }
}