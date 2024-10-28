package com.poc.p_couds.pojo

data class EngineDataClass(
<<<<<<< HEAD
<<<<<<< HEAD
    val coolant_temperature: String,
    val engine_load: String,
    val engine_rpm: String,
    val fuel_level: String,
    val fuel_pressure: String,
    val intake_air_temperature: String,
    val oil_temperature: String,
    val throttle_position: String,
    val time_stamp: String,
    val vehicle_speed: String
=======
    var coolant_temperature: String,
    var engine_load: String,
    var engine_rpm: String,
    var fuel_level: String,
    var fuel_pressure: String,
    var intake_air_temperature: String,
    var oil_temperature: String,
    var throttle_position: String,
    val time_stamp: String,
    var vehicle_speed: String
>>>>>>> development
=======
    var coolant_temperature: String,
    var engine_load: String,
    var engine_rpm: String,
    var fuel_level: String,
    var fuel_pressure: String,
    var intake_air_temperature: String,
    var oil_temperature: String,
    var throttle_position: String,
    val time_stamp: String,
    var vehicle_speed: String
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
)
