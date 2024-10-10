package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class UpdateHistory {
    @SerializedName("artifact")
    private String  artifact;
    @SerializedName("status")
    private String status;
    @SerializedName("start_time")
    private String startTime;
    @SerializedName("size")
    private String size;

    public UpdateHistory(String artifact, String status, String startTime, String size) {
        this.artifact = artifact;
        this.status = status;
        this.startTime = startTime;
        this.size = size;
    }
    public UpdateHistory() {
    }

    public String getArtifact() {
        return artifact;
    }

    public String getStatus() {
        return status;
    }

    public String getSize() {
        return size;
    }

    public String getStartTime() {
        return startTime;
    }
}
