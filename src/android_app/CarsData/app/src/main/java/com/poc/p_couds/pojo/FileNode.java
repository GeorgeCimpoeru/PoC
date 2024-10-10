package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;

public class FileNode {
    @SerializedName("children")
    private ArrayList<FileNode> children;
    @SerializedName("id")
    private String id_f;
    @SerializedName("name")
    private String name;
    @SerializedName("type")
    private String type;
    @SerializedName("size")
    private String size;
    @SerializedName("sw_version")
    private String swVersion;

    public ArrayList<FileNode> getChildren() {
        return children;
    }

    public String getId_f() {
        return id_f;
    }

    public String getName() {
        return name;
    }

    public String getType() {
        return type;
    }

    public String getSize() {
        return size;
    }

    public String getSwVersion() {
        return swVersion;
    }
}
