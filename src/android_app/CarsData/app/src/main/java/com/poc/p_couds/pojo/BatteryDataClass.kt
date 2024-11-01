package com.poc.p_couds.pojo

data class BatteryDataClass(
    var battery_level: String,
    var battery_state_of_charge: String,
    var percentage: String,
    val time_stamp: String,
    var voltage: String
)
