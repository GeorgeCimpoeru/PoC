package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class UpdateV {
    @SerializedName("update_file_version")
    public String updateFileVersion;
    @SerializedName("update_file_type")
    public String updateFileType;
    @SerializedName("ecu_id")
    public String ecuId;
    public UpdateV(String updateFileVersion, String updateFileType, String ecuId)
    {
        this.updateFileVersion = updateFileVersion;
        this.updateFileType = updateFileType;
        this.ecuId = ecuId;
    }
}
