package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class UpdateVResponse {
    @SerializedName("errors")
    private String errors;
    @SerializedName("status_download")
    private String statusDownload;
    @SerializedName("time_stamp")
    private String timeStamp;

    public String getErrors() {
        return errors;
    }

    public String getStatusDownload() {
        return statusDownload;
    }

    public String getTimeStamp() {
        return timeStamp;
    }
}
