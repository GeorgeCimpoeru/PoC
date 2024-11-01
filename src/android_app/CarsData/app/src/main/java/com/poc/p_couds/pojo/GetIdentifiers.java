package com.poc.p_couds.pojo;

import com.google.gson.annotations.SerializedName;

public class GetIdentifiers {
    @SerializedName("Battery_Identifiers")
    private BatteryIdentifiers batteryIdentifiers;
    @SerializedName("Doors_Identifiers")
    private DoorsIdentifiers doorsIdentifiers;
    @SerializedName("Engine_Identifiers")
    private EngineIdentifiers engineIdentifiers;
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

    public static class DoorsIdentifiers{
        @SerializedName("ajar")
        private String ajar;
        @SerializedName("door")
        private String door;
        @SerializedName("driver")
        private String driver;
        @SerializedName("passenger")
        private String passenger;
        @SerializedName("passenger_lock")
        private String passengerLock;

        public String getAjarStatus() {
            return ajar;
        }

        public String getDoorStatus() {
            return door;
        }

        public String getDriverStatus() {
            return driver;
        }

        public String getPassengerStatus() {
            return passenger;
        }

        public String getPassengerLockStatus() {
            return passengerLock;
        }
    }

    public static class EngineIdentifiers{
        @SerializedName("coolant_temperature")
        private String coolantTemperature;
        @SerializedName("engine_load")
        private String engineLoad;
        @SerializedName("engine_rpm")
        private String engineRpm;
        @SerializedName("fuel_level")
        private String fuelLevel;
        @SerializedName("fuel_pressure")
        private String fuelPressure;
        @SerializedName("intake_air_temperature")
        private String intakeAirTemperature;
        @SerializedName("oil_temperature")
        private String oilTemperature;
        @SerializedName("throttle_position")
        private String throttlePosition;
        @SerializedName("vehicle_speed")
        private String vehicleSpeed;

        public String getCoolantTemperatureStatus() {
            return coolantTemperature;
        }

        public String getEngineLoadStatus() {
            return engineLoad;
        }

        public String getEngineRpmStatus() {
            return engineRpm;
        }

        public String getFuelLevelStatus() {
            return fuelLevel;
        }

        public String getFuelPressureStatus() {
            return fuelPressure;
        }

        public String getIntakeAirTemperatureStatus() {
            return intakeAirTemperature;
        }

        public String getOilTemperatureStatus() {
            return oilTemperature;
        }

        public String getThrottlePositionStatus() {
            return throttlePosition;
        }

        public String getVehicleSpeedStatus() {
            return vehicleSpeed;
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

    public DoorsIdentifiers getDoorsIdentifiers() {
        return doorsIdentifiers;
    }

    public EngineIdentifiers getEngineIdentifiers() {
        return engineIdentifiers;
    }

    public McuIdentifiers getMcuIdentifiers() {
        return mcuIdentifiers;
    }
}
