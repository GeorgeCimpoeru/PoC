package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class ResetEcuPost {
    @SerializedName("ecu_id")
    private String ecuId;
    @SerializedName("type_reset")
    private String typeReset;

    public ResetEcuPost(String ecuId, String typeReset) {
        this.ecuId = ecuId;
        this.typeReset = typeReset;
    }
}
