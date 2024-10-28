package com.poc.p_couds.pojo

data class hvacModes(
<<<<<<< HEAD
<<<<<<< HEAD
    val AC_Status: String,
    val Air_Recirculation: String,
    val Defrost: String,
    val Front: String,
    val Legs: String,
=======
    var AC_Status: String,
    var Air_Recirculation: String,
    var Defrost: String,
    var Front: String,
    var Legs: String,
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
)

data class HVACDataClass(
    var ambient_air_temperature: String,
    var cabin_temperature: String,
    var cabin_temperature_driver_set: String,
    var fan_speed: String,

    val hvac_modes: hvacModes,

<<<<<<< HEAD
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
=======
    var mass_air_flow: String,
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
    val time_stamp: String
)
