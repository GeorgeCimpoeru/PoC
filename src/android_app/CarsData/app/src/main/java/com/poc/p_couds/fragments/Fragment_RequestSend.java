package com.poc.p_couds.fragments;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.Switch;

import com.poc.p_couds.APIClient;
import com.poc.p_couds.IApiService;
import com.poc.p_couds.R;
import com.poc.p_couds.pojo.Authenticate;
import com.poc.p_couds.pojo.ChangeStatus;
import com.poc.p_couds.pojo.GetIdentifiers;
import com.poc.p_couds.pojo.ReadAccesTiming;
import com.poc.p_couds.pojo.ReadAccessTimingPost;
import com.poc.p_couds.pojo.ReadDtc;
import com.poc.p_couds.pojo.ResetEcu;
import com.poc.p_couds.pojo.ResetEcuPost;
import com.poc.p_couds.pojo.TesterPresent;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_RequestSend#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Fragment_RequestSend extends Fragment {

    private final String TAG = "PoC";
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_request_send, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        Switch sw = view.findViewById(R.id.sw);
        sw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                testerPresent();
            }
        });
    }

    IApiService apiInterface;

    private void changeSession() {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<ChangeStatus> call = apiInterface.requestChangeStatus();

        call.enqueue(new Callback<ChangeStatus>() {
            @Override
            public void onResponse(Call<ChangeStatus> call, Response<ChangeStatus> response) {
                // Logic here
            }

            @Override
            public void onFailure(Call<ChangeStatus> call, Throwable t) {

            }
        });
    }

    private void readDtcInfo()
    {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<ReadDtc> call = apiInterface.requestReadDtcInfo();
        call.enqueue(new Callback<ReadDtc>() {
            @Override
            public void onResponse(Call<ReadDtc> call, Response<ReadDtc> response) {

            }

            @Override
            public void onFailure(Call<ReadDtc> call, Throwable t) {

            }
        });
    }
    private void authenticate()
    {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<Authenticate> call = apiInterface.requestAuthenticate();
        call.enqueue(new Callback<Authenticate>() {
            @Override
            public void onResponse(Call<Authenticate> call, Response<Authenticate> response) {
                Authenticate body = response.body();
                if (body != null)
                {
                    if (body.getMessage().equals("Authentication successful"))
                    {
                        // code if authentication is successful
                    }
                    else {
                        // code if authentication failed
                    }
                }
            }

            @Override
            public void onFailure(Call<Authenticate> call, Throwable t) {

            }
        });
    }
    private void readAccessTiming(int subFunction)
    {
        ReadAccessTimingPost readAccessTimingPost = new ReadAccessTimingPost(subFunction);
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<ReadAccesTiming> call = apiInterface.requestReadAccessTiming(readAccessTimingPost);

        call.enqueue(new Callback<ReadAccesTiming>() {
            @Override
            public void onResponse(Call<ReadAccesTiming> call, Response<ReadAccesTiming> response) {

            }

            @Override
            public void onFailure(Call<ReadAccesTiming> call, Throwable t) {

            }
        });
    }
    private void testerPresent()
    {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<TesterPresent> call = apiInterface.requestTesterPresent();

        call.enqueue(new Callback<TesterPresent>() {

            @Override
            public void onResponse(Call<TesterPresent> call, Response<TesterPresent> response) {
                TesterPresent body = response.body();
                if (body != null)
                {
                    if (body.getMessage().equals("Tester is present"))
                    {
                        Log.i(TAG, "Tester is present");
                        // code if tester is present
                    }
                    else {
                        // code if tester is not present
                    }
                }
            }

            @Override
            public void onFailure(Call<TesterPresent> call, Throwable t) {

            }
        });
    }
    private void resetEcu(String ecuId, String typeReset)
    {
        ResetEcuPost resetEcuPost = new ResetEcuPost(ecuId, typeReset);
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<ResetEcu> call = apiInterface.requestResetEcu(resetEcuPost);

        call.enqueue(new Callback<ResetEcu>() {
            @Override
            public void onResponse(Call<ResetEcu> call, Response<ResetEcu> response) {
                ResetEcu body = response.body();
                if (body != null)
                {
                    if (body.getMessage().equals("ECU reset successful"))
                    {
                        // code if reset is successful
                    }
                    else {
                        // code if reset failed
                    }
                }
            }

            @Override
            public void onFailure(Call<ResetEcu> call, Throwable t) {

            }
        });
    }
    private void getIdentifiers()
    {
        apiInterface = APIClient.getClient().create(IApiService.class);
        Call<GetIdentifiers> call = apiInterface.requestGetIdentifiers();

        call.enqueue(new Callback<GetIdentifiers>() {
            @Override
            public void onResponse(Call<GetIdentifiers> call, Response<GetIdentifiers> response) {

            }

            @Override
            public void onFailure(Call<GetIdentifiers> call, Throwable t) {

            }
        });
    }
}