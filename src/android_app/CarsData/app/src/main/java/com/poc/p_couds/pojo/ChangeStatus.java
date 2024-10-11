package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.io.Serial;

public class ChangeStatus {
    @SerializedName("message")
    private String message;
    @SerializedName("status")
    private String status;

    public String getMessage() {
        return message;
    }

    public String getStatus() {
        return status;
    }
}
