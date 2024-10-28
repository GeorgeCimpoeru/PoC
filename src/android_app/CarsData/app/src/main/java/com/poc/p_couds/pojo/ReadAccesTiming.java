package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class ReadAccesTiming {
    @SerializedName("message")
    private String message;

    @SerializedName("status")
    private String status;

    @SerializedName("timing_values")
    private TimingValue timingValue;

    public static class TimingValue
    {
        @SerializedName("P2_MAX_TIME_DEFAULT")
        private String p2Max;

        @SerializedName("P2_STAR_MAX_TIME_DEFAULT")
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
