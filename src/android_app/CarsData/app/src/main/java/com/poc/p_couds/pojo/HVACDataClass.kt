package com.poc.p_couds.pojo

data class hvacModes(
    var AC_Status: String,
    var Air_Recirculation: String,
    var Defrost: String,
    var Front: String,
    var Legs: String,
)

data class HVACDataClass(
    var ambient_air_temperature: String,
    var cabin_temperature: String,
    var cabin_temperature_driver_set: String,
    var fan_speed: String,

    val hvac_modes: hvacModes,

    var mass_air_flow: String,
    val time_stamp: String
)
