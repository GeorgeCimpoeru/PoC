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
<<<<<<< HEAD
<<<<<<< HEAD
        @SerializedName("P2_MAX_TIME_DEFAULT")
=======
        // For subFunct = 3 (custom values)
        @SerializedName("p2_max_time")
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
        private String p2Max;

        @SerializedName("p2_star_max_time")
        private String p2Star;

        @SerializedName("P2_MAX_TIME_DEFAULT")
        private String p2MaxDefault;

        @SerializedName("P2_STAR_MAX_TIME_DEFAULT")
        private String p2StarDefault;

        public String getP2Max()
        {
            return p2Max != null ? p2Max: p2MaxDefault;
        }
        public String getP2Star()
        {
<<<<<<< HEAD
            return p2Star;
=======
        // For subFunct = 3 (custom values)
        @SerializedName("p2_max_time")
        private String p2Max;

        @SerializedName("p2_star_max_time")
        private String p2Star;

        @SerializedName("P2_MAX_TIME_DEFAULT")
        private String p2MaxDefault;

        @SerializedName("P2_STAR_MAX_TIME_DEFAULT")
        private String p2StarDefault;

        public String getP2Max()
        {
            return p2Max != null ? p2Max: p2MaxDefault;
        }
        public String getP2Star()
        {
            return p2Star != null ? p2Star: p2StarDefault;
>>>>>>> development
=======
            return p2Star != null ? p2Star: p2StarDefault;
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
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
<<<<<<< HEAD
<<<<<<< HEAD
}
=======
}
>>>>>>> development
=======
}
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
