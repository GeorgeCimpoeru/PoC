package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ECU {

<<<<<<< HEAD
    @SerializedName("ecu_ids")
    private ArrayList<String> ecus;
=======
    @SerializedName("ecus")
    private ArrayList<ECUDetail> ecus;
>>>>>>> development
    @SerializedName("mcu_id")
    private String mcuId;
    @SerializedName("status")
    private String status;
    @SerializedName("time_stamp")
    private String timeStamp;

<<<<<<< HEAD
    public ArrayList<String> getEcus() {
=======
    public ArrayList<ECUDetail> getEcus() {
>>>>>>> development
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
<<<<<<< HEAD
=======

    public static class ECUDetail{
        @SerializedName("ecu_id")
        private String ecu_id;
        @SerializedName("version")
        private String version;

        public String getEcu_id() {
            return ecu_id;
        }

        public String getVersion() {
            return version;
        }
    }
>>>>>>> development
}
