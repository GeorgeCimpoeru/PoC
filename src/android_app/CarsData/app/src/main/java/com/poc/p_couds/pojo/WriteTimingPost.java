package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class WriteTimingPost {
    @SerializedName("p2_max") private int p2max;
    @SerializedName("p2_star_max") private int p2star;

    public WriteTimingPost(int p2max, int p2star)
    {
        this.p2max = p2max;
        this.p2star = p2star;
    }
}
