package com.poc.p_couds.pojo

data class EngineDataClass(
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
)
