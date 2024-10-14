package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class ReadAccessTimingPost {
    @SerializedName("sub_funct")
    private int subFunct;

    public ReadAccessTimingPost(int subFunct)
    {
        this.subFunct = subFunct;
    }
}
