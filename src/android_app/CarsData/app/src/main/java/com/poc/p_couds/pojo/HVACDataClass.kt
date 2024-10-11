package com.poc.p_couds.pojo

data class hvacModes(
    val AC_Status: String,
    val Air_Recirculation: String,
    val Defrost: String,
    val Front: String,
    val Legs: String,
)

data class HVACDataClass(
    val ambient_air_temperature: String,
    val cabin_temperature: String,
    val cabin_temperature_driver_set: String,
    val fan_speed: String,

    val hvac_modes: hvacModes,

    val mass_air_flow: String,
    val time_stamp: String
)
