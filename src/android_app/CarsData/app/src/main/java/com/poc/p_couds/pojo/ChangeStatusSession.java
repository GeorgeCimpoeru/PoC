package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class ChangeStatusSession {
    @SerializedName("sub_funct")
    private int subFunct;

    public ChangeStatusSession(int subFunct)
    {
        this.subFunct = subFunct;
    }
}
