package com.poc.p_couds.fragments;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.fragment.app.Fragment;
import com.poc.p_couds.R;



/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Fragment_RequestSendAutomated#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Fragment_RequestSendAutomated extends Fragment {

    @SuppressLint("SetTextI18n")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_request_send_automated, container, false);
    }
}
