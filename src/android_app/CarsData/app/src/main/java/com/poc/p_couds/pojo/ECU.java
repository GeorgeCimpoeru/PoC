package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ECU {

    @SerializedName("ecu_ids")
    public ArrayList<String> ecus;
    @SerializedName("mcu_id")
    public String mcuId;
    @SerializedName("status")
    public String status;
    @SerializedName("time_stamp")
    public String timeStamp;
}
