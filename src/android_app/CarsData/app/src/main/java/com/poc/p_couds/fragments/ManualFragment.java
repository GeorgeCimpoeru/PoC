package com.poc.p_couds.fragments;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.poc.p_couds.APIClient;
import com.poc.p_couds.APIInterface;
import com.poc.p_couds.R;
import com.poc.p_couds.pojo.Authenticate;
import com.poc.p_couds.pojo.ChangeStatus;
import com.poc.p_couds.pojo.ReadAccesTiming;
import com.poc.p_couds.pojo.ReadAccessTimingPost;
import com.poc.p_couds.pojo.ReadDtc;
import com.poc.p_couds.pojo.ResetEcu;
import com.poc.p_couds.pojo.ResetEcuPost;
import com.poc.p_couds.pojo.TesterPresent;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ManualFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_manual, container, false);
    }
}