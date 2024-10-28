package com.poc.p_couds.activities;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
<<<<<<< HEAD
<<<<<<< HEAD
=======
import android.util.Pair;
>>>>>>> development
=======
import android.util.Pair;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
import android.view.Menu;
import android.view.MenuItem;

import androidx.annotation.NonNull;
<<<<<<< HEAD
<<<<<<< HEAD
=======
import androidx.appcompat.app.AlertDialog;
>>>>>>> development
=======
import androidx.appcompat.app.AlertDialog;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import androidx.fragment.app.Fragment;
<<<<<<< HEAD
<<<<<<< HEAD
import android.view.View;
=======
import androidx.lifecycle.ViewModelProvider;

import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;
>>>>>>> development
=======
import androidx.lifecycle.ViewModelProvider;

import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
import android.widget.Toast;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;
import com.poc.p_couds.APIClient;
<<<<<<< HEAD
<<<<<<< HEAD
import com.poc.p_couds.IApiService;
=======
import com.poc.p_couds.models.IApiService;
import com.poc.p_couds.OTAViewModel;
>>>>>>> development
=======
import com.poc.p_couds.models.IApiService;
import com.poc.p_couds.OTAViewModel;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
import com.poc.p_couds.R;
import com.poc.p_couds.fragments.Fragment_Update;
import com.poc.p_couds.fragments.ManualFragment;
import com.poc.p_couds.pojo.ECU;
<<<<<<< HEAD
<<<<<<< HEAD

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
=======
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
import com.poc.p_couds.pojo.FileNode;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Objects;
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class OTA extends AppCompatActivity {

<<<<<<< HEAD
<<<<<<< HEAD
    private ECU ecu;
    private final String TAG = "PoC";
=======
    private FileNode fileNodes;
    private OTAViewModel otaViewModel;
    private ECU ecu;
    private final String TAG = "PoC";
    private ProgressBar progressBar;
    private TextView retrieveDataTxt;
    private boolean allReady = false;
>>>>>>> development
=======
    private FileNode fileNodes;
    private OTAViewModel otaViewModel;
    private ECU ecu;
    private final String TAG = "PoC";
    private ProgressBar progressBar;
    private TextView retrieveDataTxt;
    private boolean allReady = false;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.toolbar_menu, menu);
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ota);
<<<<<<< HEAD
<<<<<<< HEAD
=======
        otaViewModel = new ViewModelProvider(this).get(OTAViewModel.class);
        observeViewModel();
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setTitle("OTA");
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });
<<<<<<< HEAD
=======
        retrieveDataTxt = findViewById(R.id.dataRetrievingTxt);
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae

        getListOfECUs();
        getListOfVersionsAPI();
    }

    private void observeViewModel()
    {
        otaViewModel.getListOfVersionResponse().observe(this, this::handleGetListVersionsResponse);
        otaViewModel.getErrorMessageVersions().observe(this, this::handleErrorMessageVersions);
    }
    private void getListOfVersionsAPI()
    {
        retrieveDataTxt.setText("Geting available versions");
        otaViewModel.getListOfVersionsAPI();
    }

    private void handleGetListVersionsResponse(FileNode body)
    {
        fileNodes = body;
        if (allReady)
        {
            initElements();
        }
        allReady = true;

    }
    private void handleErrorMessageVersions(String error)
    {
        Toast.makeText(this , "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
        Toast.makeText(this,error, Toast.LENGTH_LONG).show();
        Log.e(TAG, "ERROR while retrieving the versions");
        finish();
    }

    private ArrayList<Pair<String,String>> extractVersion(FileNode swVersions, String ecuFolderName)
    {
        try{
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
                    ArrayList<Pair<String,String>> versions = new ArrayList<>();
                    for (FileNode swVersion: targetFolder.getChildren())
                    {
                        if (!Objects.equals(swVersion.getType(), "folder"))
                        {
                            versions.add(new Pair<>(swVersion.getSwVersion(), swVersion.getSize()));
                        }
                    }
<<<<<<< HEAD
                });
        bottomNavigationView.setSelectedItemId(R.id.mcu);
    }
<<<<<<< HEAD
=======
        otaViewModel = new ViewModelProvider(this).get(OTAViewModel.class);
        observeViewModel();
=======
                    return versions;
                }
            }
        }catch (NullPointerException e) {
            return new ArrayList<>();
        }
        return new ArrayList<>();
    }
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae

    private void initElements()
    {
        progressBar = findViewById(R.id.progress_circular);
        progressBar.setVisibility(View.INVISIBLE);
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottomNavigationView);
        bottomNavigationView.setVisibility(View.VISIBLE);

        bottomNavigationView.setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
            @Override
<<<<<<< HEAD
            public void onClick(View view) {
                finish();
            }
        });
        retrieveDataTxt = findViewById(R.id.dataRetrievingTxt);

        getListOfECUs();
        getListOfVersionsAPI();
    }

    private void observeViewModel()
    {
        otaViewModel.getListOfVersionResponse().observe(this, this::handleGetListVersionsResponse);
        otaViewModel.getErrorMessageVersions().observe(this, this::handleErrorMessageVersions);
    }
    private void getListOfVersionsAPI()
    {
        retrieveDataTxt.setText("Geting available versions");
        otaViewModel.getListOfVersionsAPI();
    }

    private void handleGetListVersionsResponse(FileNode body)
    {
        fileNodes = body;
        if (allReady)
        {
            initElements();
        }
        allReady = true;

    }
    private void handleErrorMessageVersions(String error)
    {
        Toast.makeText(this , "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
        Toast.makeText(this,error, Toast.LENGTH_LONG).show();
        Log.e(TAG, "ERROR while retrieving the versions");
        finish();
    }

    private ArrayList<Pair<String,String>> extractVersion(FileNode swVersions, String ecuFolderName)
    {
        try{
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
                    ArrayList<Pair<String,String>> versions = new ArrayList<>();
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
        }catch (NullPointerException e) {
            return new ArrayList<>();
        }
        return new ArrayList<>();
    }

    private void initElements()
    {
        progressBar = findViewById(R.id.progress_circular);
        progressBar.setVisibility(View.INVISIBLE);
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottomNavigationView);
        bottomNavigationView.setVisibility(View.VISIBLE);

        bottomNavigationView.setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
            @Override
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                String title = "";
                String idEcu = "";
                String fileName = "";

                // Get the current fragment
                Fragment currentFragment = getSupportFragmentManager().findFragmentById(R.id.content_frame);

                // If the current fragment is Fragment_Update and a download is in progress, show the alert dialog
                if (currentFragment instanceof Fragment_Update) {
                    Fragment_Update fragmentUpdate = (Fragment_Update) currentFragment;

                    Log.i(TAG, fragmentUpdate.isDownloading() +" Down");
                    if (fragmentUpdate.isDownloading()) {
                        // Show the AlertDialog to confirm navigation
                        new AlertDialog.Builder(OTA.this)
                                .setTitle("Download in progress")
                                .setMessage("A download is in progress. Do you really want to leave?")
                                .setPositiveButton("Yes", (dialog, which) -> {
                                    // Allow navigation only if user confirms
                                    //fragmentUpdate.stopDownload();
                                    navigateToFragment(item);
                                })
                                .setNegativeButton("No", (dialog, which) -> {
                                    // Do nothing, keep the user in the current fragment
                                    dialog.dismiss();
                                })
                                .show();

                        // Return false to prevent immediate navigation (wait for user decision)
                        return false;
                    }
                }

                // If no download is in progress, allow immediate navigation
                return navigateToFragment(item);
            }
        });
        bottomNavigationView.setSelectedItemId(R.id.mcu);
    }

    private boolean navigateToFragment(MenuItem item) {
        String title = "";
        String idEcu = "";
        String fileName = "";

        // Determine which fragment to navigate to based on the selected item
        if (item.getItemId() == R.id.mcu) {
            title = "MCU";
            idEcu = ecu.getMcuId();
            fileName = "MCU_SW_VERSIONS";
        } else if (item.getItemId() == R.id.battery) {
            title = "Battery";
            idEcu = ecu.getEcus().get(0).getEcu_id();
            fileName = "ECU_BATTERY_SW_VERSIONS";
        } else if (item.getItemId() == R.id.engine) {
            title = "Engine";
            idEcu = ecu.getEcus().get(1).getEcu_id();
            fileName = "";
        } else if (item.getItemId() == R.id.door) {
            title = "Door";
            idEcu = ecu.getEcus().get(2).getEcu_id();
            fileName = "";
        } else if (item.getItemId() == R.id.hvac) {
            title = "HVAC";
            idEcu = ecu.getEcus().get(3).getEcu_id();
            fileName = "";
        }

        // Check if the idEcu is valid
        if (!Objects.equals(idEcu, "00")) {
            openFragment(new Fragment_Update(), title, idEcu, fileName);
            return true;  // Return true to indicate successful navigation
        } else {
            Toast.makeText(OTA.this, "Unavailable ECU " + title, Toast.LENGTH_SHORT).show();
            return false;  // Return false if navigation is blocked due to invalid ECU
        }
    }

<<<<<<< HEAD
>>>>>>> development
=======

>>>>>>> Database of history updates (#486)
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
    private void openFragment(Fragment fr, String title, String idEcu, String fileName)
    {
        if (!title.isEmpty())
        {
<<<<<<< HEAD
<<<<<<< HEAD
=======
            ArrayList<Pair<String, String>> listOfVersions = extractVersion(fileNodes, fileName);

>>>>>>> development
=======
            ArrayList<Pair<String, String>> listOfVersions = extractVersion(fileNodes, fileName);

>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
            HashMap<String, String> hashMap = new HashMap<>();
            Bundle bundle = new Bundle();
            bundle.putString("FRAGMENT_TITLE", title);
            bundle.putString("id",idEcu);
<<<<<<< HEAD
<<<<<<< HEAD
            bundle.putString("folderName",fileName);
            fr.setArguments(bundle);
        }
=======
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
            ArrayList<Pair<String, String>> x = new ArrayList<>();
            bundle.putSerializable("versions",listOfVersions );
            bundle.putString("folderName",fileName);
            fr.setArguments(bundle);
        }
        allReady = false;
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
        getSupportFragmentManager().beginTransaction()
                .replace(R.id.content_frame,fr)
                .commit();
    }
    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if(item.getItemId() == R.id.sendRe_btn)
        {
            Intent intent = new Intent(this, RequestActivity.class);
            startActivity(intent);
            return true;
        } else if(item.getItemId() == R.id.updates_btn)
        {
            Fragment fr = new Fragment_Update();
<<<<<<< HEAD
<<<<<<< HEAD
            openFragment(fr,"MCU",ecu.getEcus().get(0), "MCU_SW_VERSIONS");
=======
            openFragment(fr,"MCU",ecu.getMcuId(), "MCU_SW_VERSIONS");
>>>>>>> development
=======
            openFragment(fr,"MCU",ecu.getMcuId(), "MCU_SW_VERSIONS");
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
        }
        return super.onOptionsItemSelected(item);
    }

    public void getListOfECUs()
    {
        IApiService apiInterface = APIClient.getClient().create(IApiService.class);
        Call<ECU> call = apiInterface.requestListOfEcus();
<<<<<<< HEAD
<<<<<<< HEAD
=======
        retrieveDataTxt.setText("Get ECUs IDs");
>>>>>>> development
=======
        retrieveDataTxt.setText("Get ECUs IDs");
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
        call.enqueue(new Callback<ECU>() {
            @Override
            public void onResponse(Call<ECU> call, Response<ECU> response) {
                if (response.isSuccessful() && response.body() !=null)
                {
                    Log.d(TAG,response.code()+"");
                    ecu = response.body();
<<<<<<< HEAD
<<<<<<< HEAD
                    List<String> listOfEcus = response.body().getEcus();
=======
                    List<ECU.ECUDetail> listOfEcus = response.body().getEcus();
>>>>>>> development
=======
                    List<ECU.ECUDetail> listOfEcus = response.body().getEcus();
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
                    String mcu_id = response.body().getMcuId();
                    String status = response.body().getStatus();
                    String time_stamp = response.body().getTimeStamp();
                    if (listOfEcus != null && mcu_id != null && status !=null)
                    {
<<<<<<< HEAD
<<<<<<< HEAD
                        initElements();
=======
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
                        if (allReady)
                        {
                            initElements();
                        }
                        allReady = true;
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
                    }
                    else{
                        Log.e(TAG, "The JSON have a different format what is expected");
                        Toast.makeText(OTA.this, "Response is not the one expected", Toast.LENGTH_LONG).show();
<<<<<<< HEAD
<<<<<<< HEAD
=======
                        finish();
>>>>>>> development
=======
                        finish();
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
                    }
                } else {
                    // Response was not successful or body is null
                    Log.e(TAG, "Error: Unsuccessful response or empty body. Code: " + response.code());
                    if (response.errorBody() != null) {
                        try {
                            // Optionally log the error body
                            Log.e("TAG", "Error body: " + response.errorBody().string());
                        } catch (IOException e) {
<<<<<<< HEAD
<<<<<<< HEAD
                            e.printStackTrace();
                        }
                    }
=======
                            finish();
                        }
                    }
                    finish();
>>>>>>> development
=======
                            finish();
                        }
                    }
                    finish();
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
                }
            }


            @Override
            public void onFailure(Call<ECU> call, Throwable t) {

                Toast.makeText(OTA.this, "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
                Toast.makeText(OTA.this, call.request().url().toString(), Toast.LENGTH_LONG).show();
<<<<<<< HEAD
<<<<<<< HEAD
                Log.e(TAG, "ERROR");
=======
                Log.e(TAG, "ERROR while retrieving the ecus ids");
                finish();
>>>>>>> development
=======
                Log.e(TAG, "ERROR while retrieving the ecus ids");
                finish();
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
                call.cancel();
            }
        });
    }
}