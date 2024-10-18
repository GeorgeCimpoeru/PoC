package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class WriteTiming {
    @SerializedName("message")
    private String message;

    @SerializedName("status")
    private String status;

    @SerializedName("written_values")
    private TimingValue timingValue;

    public static class TimingValue
    {
        // For subFunct = 3 (custom values)
        @SerializedName("New P2 Max Time")
        private String p2Max;

        @SerializedName("New P2 Star Max")
        private String p2Star;


        public String getP2Max()
        {
            return p2Max;
        }
        public String getP2Star()
        {
            return p2Star;
        }
    }
    public String getMessage()
    {
        return message;
    }
    public String getStatus()
    {
        return status;
    }
    public TimingValue getTimingValue()
    {
        return timingValue;
    }
}
