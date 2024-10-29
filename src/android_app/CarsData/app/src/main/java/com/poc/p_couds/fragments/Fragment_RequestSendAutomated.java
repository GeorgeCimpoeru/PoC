package com.poc.p_couds.fragments;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import com.google.gson.Gson;
import com.poc.p_couds.APIClient;
import com.poc.p_couds.R;
import com.poc.p_couds.models.IApiService;
import com.poc.p_couds.pojo.Authenticate;
import com.poc.p_couds.pojo.ChangeStatus;
import com.poc.p_couds.pojo.ChangeStatusSession;
import com.poc.p_couds.pojo.ECU;
import com.poc.p_couds.pojo.FileNode;
import com.poc.p_couds.pojo.GetIdentifiers;
import com.poc.p_couds.pojo.ReadAccesTiming;
import com.poc.p_couds.pojo.ReadAccessTimingPost;
import com.poc.p_couds.pojo.ResetEcu;
import com.poc.p_couds.pojo.ResetEcuPost;
import com.poc.p_couds.pojo.TesterPresent;
import com.poc.p_couds.pojo.WriteTiming;
import com.poc.p_couds.pojo.WriteTimingPost;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;


/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_RequestSendAutomated#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Fragment_RequestSendAutomated extends Fragment {
    private TextView testerTextView;
    private IApiService apiInterface;

    List<String> allEcus = Arrays.asList(new String[]{"MCU", "Battery", "Engine", "Doors", "HVAC"});
    List<String> displayedEcus =Arrays.asList(new String[]{"None"});
    List<ECU.ECUDetail> listOfEcus = new ArrayList<>();
    String mcuId = "";
    ArrayAdapter<String>  adapterEcuReset;

    @SuppressLint("SetTextI18n")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_request_send_automated, container, false);
        testerTextView = view.findViewById(R.id.response);
        testerTextView.setMovementMethod(new ScrollingMovementMethod());

        SwitchCompat switchCompatSession = view.findViewById(R.id.change_session);
        switchCompatSession.setOnCheckedChangeListener((buttonView, isChecked) -> {
            int subFunct = isChecked ? 2 : 1;
            ChangeStatusSession postBodySession = new ChangeStatusSession(subFunct);
            toggleSession(postBodySession);
            if (subFunct == 2) {
                Toast.makeText(Fragment_RequestSendAutomated.this.getContext(), "PROGRAMMING SESSION", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(Fragment_RequestSendAutomated.this.getContext(), "DEFAULT SESSION", Toast.LENGTH_SHORT).show();
            }

        });

        Button docsButton = view.findViewById(R.id.docs);
        docsButton.setOnClickListener(v -> redirectToDocs());

        Button checkVersions = view.findViewById(R.id.q2);
        checkVersions.setOnClickListener(v -> callCheckVersions());

        Button authentication = view.findViewById(R.id.q3);
        authentication.setOnClickListener(v -> callApiAuthentication());

        Button readIdentifiers = view.findViewById(R.id.q4);
        readIdentifiers.setOnClickListener(v -> callApiIdentifiers());

        Button requestId = view.findViewById(R.id.q5);
        requestId.setOnClickListener(v -> callRequestId());

        getEcuAndIds();

        Spinner ecuReset = view.findViewById(R.id.ecu_reset_spinner);
        adapterEcuReset = new ArrayAdapter<>(view.getContext(), android.R.layout.simple_list_item_1, displayedEcus);
        ecuReset.setAdapter(adapterEcuReset);

        List<String> typeResetList = Arrays.asList(new String[]{"Type","soft", "hard"});
        Spinner typeReset = view.findViewById(R.id.type_reset_spinner);
        ArrayAdapter<String>  adapterTypeReset = new ArrayAdapter<>(view.getContext(), android.R.layout.simple_list_item_1, typeResetList);
        typeReset.setAdapter(adapterTypeReset);
        typeReset.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                Toast.makeText(view.getContext(), "Reset", Toast.LENGTH_SHORT).show();
                String ecuId;
                if (i == 0)
                {
                    return;
                }
                if (ecuReset.getSelectedItemPosition() > 0)
                {
                    ecuId = listOfEcus.get(ecuReset.getSelectedItemPosition()).getEcu_id();
                }
                else
                {
                    ecuId = mcuId;
                }
                String typeOfReset = typeResetList.get(i);
                ResetEcuPost r = new ResetEcuPost(ecuId, typeOfReset);
                callEcuReset(r);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });


        SwitchCompat switchCompatTester = view.findViewById(R.id.tester);
        switchCompatTester.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                toggleTester();
                Log.d("Toggle", "ON");
            } else {
                Log.d("Toggle", "OFF");
            }
        });
        SwitchCompat switchCompatReadAccess = view.findViewById(R.id.read_access);
        switchCompatReadAccess.setOnCheckedChangeListener((buttonView, isChecked) -> {
            int subFunct = isChecked ? 3 : 1; // If checked, subFunct = 3; else subFunct = 1

            // Create the POST body object
            ReadAccessTimingPost postBody = new ReadAccessTimingPost(subFunct);
            toggleRead(postBody);
        });

        Button writeTimingP2Max = view.findViewById(R.id.q6);
        writeTimingP2Max.setOnClickListener(v -> showInputDialogTiming());

        return view;
    }

    private void toggleSession(ChangeStatusSession postBodySession){
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<ChangeStatus> call=apiInterface.requestChangeStatus(postBodySession);
        call.enqueue(new retrofit2.Callback<ChangeStatus>() {
            @Override
            public void onResponse(Call<ChangeStatus> call, Response<ChangeStatus> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                }
                else {
                    testerTextView.setText("Failed to retrive response");
                }
            }
            @Override
            public void onFailure(Call<ChangeStatus> call, Throwable t) {
                testerTextView.setText("Request Failed: " + t);
            }
        });
    }

    private void redirectToDocs()
    {
        Intent browserIntent = new Intent(Intent. ACTION_VIEW, Uri.parse("https://github.com/GeorgeCimpoeru/PoC/blob/master/README.md"));
        startActivity(browserIntent);
    }

    private void getEcuAndIds()
    {
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<ECU> call=apiInterface.requestListOfEcus();
        call.enqueue(new Callback<ECU>() {
            @Override
            public void onResponse(Call<ECU> call, Response<ECU> response) {
                if (response.isSuccessful() && response.body() != null) {
                    ECU ecusResponse = response.body();
                    listOfEcus = ecusResponse.getEcus();
                    mcuId = ecusResponse.getMcuId();
                    String status = ecusResponse.getStatus();
                    if (Objects.equals(status, "Success")) {
                        displayedEcus.addAll(allEcus.subList(0, listOfEcus.size() + 1));
                        adapterEcuReset.notifyDataSetChanged();
                    } else{
                        Log.w("PoC", "Unable to retrieve ECUs available from api...");
                        Toast.makeText(getContext(), "Unable to retrieve ECUs available from api...", Toast.LENGTH_SHORT).show();
                    }
                }
                else {
                    Log.e("PoC","Error while making call to retrieve ecu ids information. Code:" + response.code());
                }
            }

            @Override
            public void onFailure(Call<ECU> call, Throwable t) {
                Log.e("PoC","Error while making call to retrieve ecu ids information...");
            }
        });
    }

    private void callEcuReset(ResetEcuPost ecu) {
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<ResetEcu> call=apiInterface.requestResetEcu(ecu);
        call.enqueue(new Callback<ResetEcu>() {
            @Override
            public void onResponse(Call<ResetEcu> call, Response<ResetEcu> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                }
                else {
                    testerTextView.setText("Failed to retrive response");
                }
            }

            @Override
            public void onFailure(Call<ResetEcu> call, Throwable t) {
                testerTextView.setText("Request Failed: " + t.getMessage());
            }
        });
    }

    private void callCheckVersions() {
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<FileNode> call=apiInterface.requestListOfVersions();
        call.enqueue(new Callback<FileNode>() {
            @Override
            public void onResponse(Call<FileNode> call, Response<FileNode> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                }
                else {
                    testerTextView.setText("Failed to retrive response");
                }
            }

            @Override
            public void onFailure(Call<FileNode> call, Throwable t) {
                testerTextView.setText("Request Failed: " + t.getMessage());
            }
        });
    }

    private void callRequestId() {
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<ECU> call=apiInterface.requestListOfEcus();
        call.enqueue(new Callback<ECU>() {
            @Override
            public void onResponse(Call<ECU> call, Response<ECU> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                }
                else {
                    testerTextView.setText("Failed to retrive response");
                }
            }

            @Override
            public void onFailure(Call<ECU> call, Throwable t) {
                testerTextView.setText("Request Failed: " + t.getMessage());
            }
        });
    }

    private void callApiAuthentication() {
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<Authenticate> call=apiInterface.requestAuthenticate();
        call.enqueue(new retrofit2.Callback<Authenticate>() {
            @Override
            public void onResponse(Call<Authenticate> call, Response<Authenticate> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                }
                else {
                    testerTextView.setText("Failed to retrive response");
                }
            }
            @Override
            public void onFailure(Call<Authenticate> call, Throwable t) {
                testerTextView.setText("Request Failed: " + t.getMessage());
            }
        });
    }

    private void callApiIdentifiers() {
        apiInterface = APIClient.getClient().create(IApiService.class);

        Call<GetIdentifiers> call = apiInterface.requestGetIdentifiers();
        call.enqueue(new retrofit2.Callback<GetIdentifiers>() {
            @Override
            public void onResponse(Call<GetIdentifiers> call, Response<GetIdentifiers> response) {
                if (response.isSuccessful() && response != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                } else {
                    testerTextView.setText("Failed to retrieve response");
                }
            }
            @Override
            public void onFailure(Call<GetIdentifiers> call, Throwable t) {
                testerTextView.setText("Request Failed: "+ t.getMessage());
            }
        });
    }

    private void toggleTester() {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<TesterPresent> call = apiInterface.requestTesterPresent();
        call.enqueue(new retrofit2.Callback<TesterPresent>() {
            @Override
            public void onResponse(Call<TesterPresent> call, Response<TesterPresent> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                } else {
                    testerTextView.setText("Failed to retrieve response.");
                }
            }

            @Override
            public void onFailure(Call<TesterPresent> call, Throwable t) {
                testerTextView.setText("Request failed: " + t.getMessage());
            }
        });
    }

    private void toggleRead(ReadAccessTimingPost postBody) {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<ReadAccesTiming> call = apiInterface.requestReadAccessTiming(postBody);
        call.enqueue(new retrofit2.Callback<ReadAccesTiming>() {
            @Override
            public void onResponse(Call<ReadAccesTiming> call, Response<ReadAccesTiming> response) {
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());
                    testerTextView.setText(jsonResponse);
                } else {
                    testerTextView.setText("Failed to retrieve response.");
                }
            }

            @Override
            public void onFailure(Call<ReadAccesTiming> call, Throwable t) {
                testerTextView.setText("Request failed: " + t.getMessage());
            }
        });
    }

    private void showInputDialogTiming() {
        LayoutInflater inflater = LayoutInflater.from(requireContext());
        View dialogView = inflater.inflate(R.layout.dialog_input_p2time, null);

        EditText editTextP2MaxTime = dialogView.findViewById(R.id.editTextP2MaxTime);
        EditText editTextP2StarTime = dialogView.findViewById(R.id.editTextP2StarMaxTime);

        AlertDialog.Builder builder = new AlertDialog.Builder(requireContext());
        builder.setTitle("P2 Timing Values");
        builder.setView(dialogView);

        builder.setPositiveButton("Submit", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                String p2MaxTimeStr = editTextP2MaxTime.getText().toString();
                String p2StarTimeStr = editTextP2StarTime.getText().toString();
                ;

                if (!p2MaxTimeStr.isEmpty() && !p2StarTimeStr.isEmpty()) {
                    try {
                        // Convert to integers
                        int p2MaxTime = Integer.parseInt(p2MaxTimeStr);
                        int p2StarTime = Integer.parseInt(p2StarTimeStr);

                        // Call the API with integer values
                        callApiWithCustomTiming(p2MaxTime, p2StarTime);
                    } catch (NumberFormatException e) {
                        // Show an error message for invalid integer input
                        testerTextView.setText("Please enter valid integer values.");
                    }
                } else {
                    testerTextView.setText("Please enter values for both fields.");
                }
            }
        });
        builder.setNegativeButton("Cancel", null);
        builder.create().show();
    }

    private void callApiWithCustomTiming(int p2MaxTime, int p2StarTime) {
        apiInterface = APIClient.getClient().create(IApiService.class);

        WriteTimingPost postBody = new WriteTimingPost(p2MaxTime, p2StarTime);

        Call<WriteTiming> call= apiInterface.requestWriteTiming(postBody);
        call.enqueue(new retrofit2.Callback<WriteTiming>() {
            @Override
            public void onResponse(Call<WriteTiming> call, Response<WriteTiming> response){
                if (response.isSuccessful() && response.body() != null) {
                    Gson gson = new Gson();
                    String jsonResponse = gson.toJson(response.body());

                    testerTextView.setText(jsonResponse);
                }
                else {
                    testerTextView.setText("Failed to retrieve the response.");
                }
            }

            @Override
            public void onFailure(Call<WriteTiming> call, Throwable t) {
                testerTextView.setText("Request failed: " + t.getMessage());
            }
        });
    }
}
