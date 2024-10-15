package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class TesterPresent {
    @SerializedName("can_id")
    private String canId;
    @SerializedName("message")
    private String message;

    public String getCanId() {
        return canId;
    }

    public String getMessage() {
        return message;
    }
}
