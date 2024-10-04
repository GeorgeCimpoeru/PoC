package com.poc.p_couds

import retrofit2.Call
import retrofit2.http.GET

interface IApiService {
    @GET("api/read_info_battery")
    fun getInfoBattery(): Call<BatteryDataClass>

    @GET("api/read_info_engine")
    fun getInfoEngine(): Call<EngineDataClass>

    @GET("api/read_info_doors")
    fun getInfoDoors(): Call<DoorsDataClass>

    @GET("api/read_info_hvac")
    fun getInfoHVAC(): Call<HVACDataClass>
}