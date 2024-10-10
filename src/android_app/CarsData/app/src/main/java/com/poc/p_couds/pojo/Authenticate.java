package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class Authenticate {
    @SerializedName("message")
    private String message;

    public String getMessage()
    {
        return message;
    }
}
