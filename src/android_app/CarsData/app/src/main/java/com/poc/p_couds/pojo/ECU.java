package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ECU {

<<<<<<< HEAD
<<<<<<< HEAD
    @SerializedName("ecu_ids")
    private ArrayList<String> ecus;
=======
    @SerializedName("ecus")
    private ArrayList<ECUDetail> ecus;
>>>>>>> development
=======
    @SerializedName("ecus")
    private ArrayList<ECUDetail> ecus;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
    @SerializedName("mcu_id")
    private String mcuId;
    @SerializedName("status")
    private String status;
    @SerializedName("time_stamp")
    private String timeStamp;

<<<<<<< HEAD
<<<<<<< HEAD
    public ArrayList<String> getEcus() {
=======
    public ArrayList<ECUDetail> getEcus() {
>>>>>>> development
=======
    public ArrayList<ECUDetail> getEcus() {
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
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
<<<<<<< HEAD
=======
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae

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
<<<<<<< HEAD
>>>>>>> development
=======
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
}
