package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class UpdateHistory {
    @SerializedName("artifact")
    public String  artifact;
    @SerializedName("status")
    public String status;
    @SerializedName("start_time")
    public String startTime;
    @SerializedName("size")
    public String size;

    public UpdateHistory(String artifact, String status, String startTime, String size) {
        this.artifact = artifact;
        this.status = status;
        this.startTime = startTime;
        this.size = size;
    }
    public UpdateHistory() {
    }
}
