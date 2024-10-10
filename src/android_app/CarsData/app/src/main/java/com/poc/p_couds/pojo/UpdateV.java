package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class UpdateV {
    @SerializedName("update_file_version")
    private String updateFileVersion;
    @SerializedName("update_file_type")
    private String updateFileType;
    @SerializedName("ecu_id")
    private String ecuId;
    public UpdateV(String updateFileVersion, String updateFileType, String ecuId)
    {
        this.updateFileVersion = updateFileVersion;
        this.updateFileType = updateFileType;
        this.ecuId = ecuId;
    }

    public String getUpdateFileVersion() {
        return updateFileVersion;
    }

    public String getUpdateFileType() {
        return updateFileType;
    }

    public String getEcuId() {
        return ecuId;
    }
}
