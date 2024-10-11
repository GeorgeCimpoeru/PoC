package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ECU {

    @SerializedName("ecu_ids")
    private ArrayList<String> ecus;
    @SerializedName("mcu_id")
    private String mcuId;
    @SerializedName("status")
    private String status;
    @SerializedName("time_stamp")
    private String timeStamp;

    public ArrayList<String> getEcus() {
        return ecus;
    }

    public String getMcuId() {
        return mcuId;
    }

    public String getStatus() {
        return status;
    }

    public String getTimeStamp() {
        return timeStamp;
    }
}
