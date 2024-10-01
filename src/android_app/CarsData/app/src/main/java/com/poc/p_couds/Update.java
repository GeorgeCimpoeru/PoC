package com.poc.p_couds;

public class Update {
    private String artifact;
    private String status;
    private String startTime;
    private int size;

    public Update(String artifact, String status, String startTime, int size) {
        this.artifact = artifact;
        this.status = status;
        this.startTime = startTime;
        this.size = size;
    }

    public String getArtifact() {
        return artifact;
    }

    public void setArtifact(String artifact) {
        this.artifact = artifact;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getStartTime() {
        return startTime;
    }

    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }

    public int getSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }
}
