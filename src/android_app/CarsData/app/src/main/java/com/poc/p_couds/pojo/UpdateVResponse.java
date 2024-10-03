package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class UpdateVResponse {
    @SerializedName("errors")
    public String errors;
    @SerializedName("status_download")
    public String statusDownload;
    @SerializedName("time_stamp")
    public String timeStamp;
}
