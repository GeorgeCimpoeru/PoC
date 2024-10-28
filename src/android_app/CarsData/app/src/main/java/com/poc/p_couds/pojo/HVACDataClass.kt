package com.poc.p_couds.pojo

data class hvacModes(
<<<<<<< HEAD
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
=======
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
>>>>>>> development
    val time_stamp: String
)
