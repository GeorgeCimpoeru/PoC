package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class ReadDtc {
    @SerializedName("CAN_ID")
    private String canId;
    @SerializedName("CAN_Interface")
    private String canInterface;
    @SerializedName("Data_Bytes")
    private List<DtcFrame> dataBytes;
    @SerializedName("Data_Length")
    private String dataLength;

    public static class DtcFrame{
        @SerializedName("DTCCount")
        private dtcItem dtcCount;
        @SerializedName("DTCFormatIdentifier")
        private dtcItem dtcFormatIdentifier;
        @SerializedName("PCI")
        private dtcItem pci;
        @SerializedName("SID")
        private dtcItem sid;
        @SerializedName("StatusAvailabilityMask")
        private dtcItem statusAvailabilityMask;
        @SerializedName("SubFunction")
        private dtcItem subFunction;

        public dtcItem getDtcCount() {
            return dtcCount;
        }

        public void setDtcCount(dtcItem dtcCount) {
            this.dtcCount = dtcCount;
        }

        public dtcItem getDtcFormatIdentifier() {
            return dtcFormatIdentifier;
        }

        public void setDtcFormatIdentifier(dtcItem dtcFormatIdentifier) {
            this.dtcFormatIdentifier = dtcFormatIdentifier;
        }

        public dtcItem getPci() {
            return pci;
        }

        public void setPci(dtcItem pci) {
            this.pci = pci;
        }

        public dtcItem getSid() {
            return sid;
        }

        public void setSid(dtcItem sid) {
            this.sid = sid;
        }

        public dtcItem getStatusAvailabilityMask() {
            return statusAvailabilityMask;
        }

        public void setStatusAvailabilityMask(dtcItem statusAvailabilityMask) {
            this.statusAvailabilityMask = statusAvailabilityMask;
        }

        public dtcItem getSubFunction() {
            return subFunction;
        }

        public void setSubFunction(dtcItem subFunction) {
            this.subFunction = subFunction;
        }
    }
    public static class dtcItem{

        @SerializedName("description")
        private String description;
        @SerializedName("value")
        private String value;

        public String getDescription() {
            return description;
        }

        public void setDescription(String description) {
            this.description = description;
        }

        public String getValue() {
            return value;
        }

        public void setValue(String value) {
            this.value = value;
        }
    }

    public String getCanId() {
        return canId;
    }

    public void setCanId(String canId) {
        this.canId = canId;
    }

    public String getCanInterface() {
        return canInterface;
    }

    public void setCanInterface(String canInterface) {
        this.canInterface = canInterface;
    }

    public List<DtcFrame> getDataBytes() {
        return dataBytes;
    }

    public void setDataBytes(List<DtcFrame> dataBytes) {
        this.dataBytes = dataBytes;
    }

    public String getDataLength() {
        return dataLength;
    }

    public void setDataLength(String dataLength) {
        this.dataLength = dataLength;
    }
}
