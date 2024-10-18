package com.poc.p_couds.fragments;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import com.google.gson.Gson;
import com.poc.p_couds.APIClient;
import com.poc.p_couds.R;
import com.poc.p_couds.models.IApiService;
import com.poc.p_couds.pojo.ReadAccesTiming;
import com.poc.p_couds.pojo.ReadAccessTimingPost;
import com.poc.p_couds.pojo.TesterPresent;
import com.poc.p_couds.pojo.WriteTiming;
import com.poc.p_couds.pojo.WriteTimingPost;

import retrofit2.Call;
import retrofit2.Response;


/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_RequestSendAutomated#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Fragment_RequestSendAutomated extends Fragment {
    private TextView testerTextView;
    private IApiService apiInterface;

    @SuppressLint("SetTextI18n")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_request_send_automated, container, false);
        testerTextView = view.findViewById(R.id.response);
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
