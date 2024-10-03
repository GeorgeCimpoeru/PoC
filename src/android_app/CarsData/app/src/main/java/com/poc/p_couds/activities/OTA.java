package com.poc.p_couds.activities;

import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import androidx.fragment.app.Fragment;
import android.view.View;
import android.widget.Toast;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;
import com.poc.p_couds.APIInterface;
import com.poc.p_couds.APIClient;
import com.poc.p_couds.R;
import com.poc.p_couds.fragments.Fragment_RequestSend;
import com.poc.p_couds.fragments.Fragment_Update;
import com.poc.p_couds.pojo.ECU;
import com.poc.p_couds.pojo.FileNode;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class OTA extends AppCompatActivity {

    private ECU ecu;
    private final String TAG = "PoC";

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.toolbar_menu, menu);
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ota);

        getListOfECUs();
    }

    private void initElements()
    {
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottomNavigationView);
        bottomNavigationView
                .setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
                    @Override
                    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                        String title = "";
                        String idEcu = "";
                        String fileName = "";

                        if(item.getItemId() == R.id.mcu)
                        {
                            title = "MCU";
                            idEcu = ecu.mcuId;
                            fileName = "MCU_SW_VERSIONS";
                        } else if(item.getItemId() == R.id.battery)
                        {
                            title = "Battery";
                            idEcu = ecu.ecus.get(0);
                            fileName = "ECU_BATTERY_SW_VERSIONS";
                        } else if(item.getItemId() == R.id.engine)
                        {
                            title = "Engine";
                            idEcu = ecu.ecus.get(1);
                            fileName = "";
                        }else if(item.getItemId() == R.id.door)
                        {
                            title = "Door";
                            idEcu = ecu.ecus.get(2);
                            fileName = "";
                        }else if(item.getItemId() == R.id.hvac)
                        {
                            title = "HVAC";
                            idEcu = ecu.ecus.get(3);
                            fileName = "";
                        }
                        openFragment(new Fragment_Update(),title, idEcu, fileName);
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
        toolbar.setNavigationOnClickListener(v -> {
            if (getSupportFragmentManager().getBackStackEntryCount() > 0) {
                getSupportFragmentManager().popBackStack();
            } else {
                finish();
            }
        });
    }
    private void openFragment(Fragment fr, String title, String idEcu, String fileName)
    {
        if (!title.isEmpty())
        {
            HashMap<String, String> hashMap = new HashMap<>();
            Bundle bundle = new Bundle();
            bundle.putString("FRAGMENT_TITLE", title);
            bundle.putString("id",idEcu);
            bundle.putString("folderName",fileName);
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
            openFragment(fr,"","","");
        } else if(item.getItemId() == R.id.updates_btn)
        {
            Fragment fr = new Fragment_Update();
            openFragment(fr,"MCU",ecu.ecus.get(0), "MCU_SW_VERSIONS");
        }
        return super.onOptionsItemSelected(item);
    }

    public void getListOfECUs()
    {
        APIInterface apiInterface = APIClient.getClient().create(APIInterface.class);
        Call<ECU> call = apiInterface.requestListOfEcus();
        call.enqueue(new Callback<ECU>() {
            @Override
            public void onResponse(Call<ECU> call, Response<ECU> response) {
                if (response.isSuccessful() && response.body() !=null)
                {
                    Log.d(TAG,response.code()+"");
                    ecu = response.body();
                    List<String> listOfEcus = response.body().ecus;
                    String mcu_id = response.body().mcuId;
                    String status = response.body().status;
                    String time_stamp = response.body().timeStamp;
                    if (listOfEcus != null && mcu_id != null && status !=null)
                    {
                        initElements();
                    }
                    else{
                        Log.e(TAG, "The JSON have a different format what is expected");
                        Toast.makeText(OTA.this, "Response is not the one expected", Toast.LENGTH_LONG).show();
                    }
                } else {
                    // Response was not successful or body is null
                    Log.e(TAG, "Error: Unsuccessful response or empty body. Code: " + response.code());
                    if (response.errorBody() != null) {
                        try {
                            // Optionally log the error body
                            Log.e("TAG", "Error body: " + response.errorBody().string());
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }


            @Override
            public void onFailure(Call<ECU> call, Throwable t) {

                Toast.makeText(OTA.this, "!!! Unable to connect to the server. Please check URL or the API is started.", Toast.LENGTH_LONG).show();
                Toast.makeText(OTA.this, call.request().url().toString(), Toast.LENGTH_LONG).show();
                Log.e(TAG, "ERROR");
                call.cancel();
            }
        });
    }
}