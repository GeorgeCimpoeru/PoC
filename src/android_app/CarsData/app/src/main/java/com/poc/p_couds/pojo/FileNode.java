package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;

public class FileNode {
    @SerializedName("children")
    public ArrayList<FileNode> children;
    @SerializedName("id")
    public String id_f;
    @SerializedName("name")
    public String name;
    @SerializedName("type")
    public String type;
    @SerializedName("size")
    public String size;
    @SerializedName("sw_version")
    public String swVersion;
}
