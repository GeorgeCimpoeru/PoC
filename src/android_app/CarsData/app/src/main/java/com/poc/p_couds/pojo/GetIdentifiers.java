package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class GetIdentifiers {
    @SerializedName("Battery_Identifiers")
    private BatteryIdentifiers batteryIdentifiers;
    @SerializedName("Doors_Identifiers")
    private String doorsIdentifiers;
    @SerializedName("Engine_Identifiers")
    private String engineIdentifiers;
    @SerializedName("MCU_Identifiers")
    private McuIdentifiers mcuIdentifiers;

    public static class BatteryIdentifiers{
        @SerializedName("energy_level")
        private String energyLevel;
        @SerializedName("percentage")
        private String percentage;
        @SerializedName("state_of_charge")
        private String stateOfCharge;
        @SerializedName("voltage")
        private String voltage;

        public String getEnergyLevel() {
            return energyLevel;
        }

        public String getStateOfCharge() {
            return stateOfCharge;
        }

        public String getPercentage() {
            return percentage;
        }

        public String getVoltage() {
            return voltage;
        }
    }
    public static class McuIdentifiers{
        @SerializedName("ecu_hardware_number")
        private String ecuHardwareNumber;
        @SerializedName("ecu_hardware_version_number")
        private String ecuHardwareVersionNumber;
        @SerializedName("ecu_manufacturing_date")
        private String ecuManufacturingDate;
        @SerializedName("ecu_part_number")
        private String ecuPartNumber;
        @SerializedName("ecu_serial_number")
        private String ecuSerialNumber;
        @SerializedName("ecu_software_number")
        private String ecuSoftwareNumber;
        @SerializedName("ecu_software_version_number")
        private String ecuSoftwareVersionNumber;
        @SerializedName("engine_type")
        private String engineType;
        @SerializedName("software_version")
        private String softwareVersion;
        @SerializedName("vin")
        private String vin;

        public String getEcuManufacturingDate() {
            return ecuManufacturingDate;
        }

        public String getEcuHardwareNumber() {
            return ecuHardwareNumber;
        }

        public String getEcuHardwareVersionNumber() {
            return ecuHardwareVersionNumber;
        }

        public String getEcuPartNumber() {
            return ecuPartNumber;
        }

        public String getEcuSerialNumber() {
            return ecuSerialNumber;
        }

        public String getEcuSoftwareNumber() {
            return ecuSoftwareNumber;
        }

        public String getEcuSoftwareVersionNumber() {
            return ecuSoftwareVersionNumber;
        }

        public String getEngineType() {
            return engineType;
        }

        public String getSoftwareVersion() {
            return softwareVersion;
        }

        public String getVin() {
            return vin;
        }
    }

    public BatteryIdentifiers getBatteryIdentifiers() {
        return batteryIdentifiers;
    }

    public String getDoorsIdentifiers() {
        return doorsIdentifiers;
    }

    public String getEngineIdentifiers() {
        return engineIdentifiers;
    }

    public McuIdentifiers getMcuIdentifiers() {
        return mcuIdentifiers;
    }
}
