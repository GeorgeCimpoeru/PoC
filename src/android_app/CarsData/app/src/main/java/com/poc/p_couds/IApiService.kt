package com.poc.p_couds

import com.poc.p_couds.pojo.Authenticate
import com.poc.p_couds.pojo.BatteryDataClass
import com.poc.p_couds.pojo.ChangeStatus
import com.poc.p_couds.pojo.DoorsDataClass
import com.poc.p_couds.pojo.ECU
import com.poc.p_couds.pojo.EngineDataClass
import com.poc.p_couds.pojo.FileNode
import com.poc.p_couds.pojo.GetIdentifiers
import com.poc.p_couds.pojo.HVACDataClass
import com.poc.p_couds.pojo.ReadAccesTiming
import com.poc.p_couds.pojo.ReadAccessTimingPost
import com.poc.p_couds.pojo.ReadDtc
import com.poc.p_couds.pojo.ResetEcu
import com.poc.p_couds.pojo.ResetEcuPost
import com.poc.p_couds.pojo.TesterPresent
import com.poc.p_couds.pojo.UpdateHistory
import com.poc.p_couds.pojo.UpdateV
import com.poc.p_couds.pojo.UpdateVResponse
import com.poc.p_couds.pojo.VINResponse
import retrofit2.Call
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.POST
import retrofit2.http.Path

interface IApiService {

    //UDS
    @GET("api/read_info_battery")
    fun getInfoBattery(): Call<BatteryDataClass>

    @GET("api/read_info_engine")
    fun getInfoEngine(): Call<EngineDataClass>

    @GET("api/read_info_doors")
    fun getInfoDoors(): Call<DoorsDataClass>

    @GET("api/read_info_hvac")
    fun getInfoHVAC(): Call<HVACDataClass>

    //MAIN

    @GET("DecodeVinValues/{vin}?format=json")
    fun getVinDetails(@Path("vin") vin:String): Call<VINResponse>

    // OTA
    @GET("/api/request_ids")
    fun requestListOfEcus(): Call<ECU>

    @POST("/api/update_to_version")
    fun updateVersion(@Body updateV: UpdateV): Call<UpdateVResponse>

    @GET("/api/drive_update_data")
    fun requestListOfVersions(): Call<FileNode>

    @GET("/api/history_updates")
    fun requestListOfUpdatesHistory(): Call<List<UpdateHistory>>

    @GET("/api/change_session")
    fun requestChangeStatus(): Call<ChangeStatus>

    @GET("/api/read_dtc_info")
    fun requestReadDtcInfo(): Call<ReadDtc>

    @GET("/api/authenticate")
    fun requestAuthenticate(): Call<Authenticate>

    @POST("/api/read_access_timing")
    fun requestReadAccessTiming(@Body readAccessTimingPost: ReadAccessTimingPost): Call<ReadAccesTiming>

    @GET("/api/tester_present")
    fun requestTesterPresent(): Call<TesterPresent>

    @POST("/api/reset_ecu")
    fun requestResetEcu(@Body resetEcuPost: ResetEcuPost): Call<ResetEcu>

    @GET("/api/get_identifiers")
    fun requestGetIdentifiers(): Call<GetIdentifiers>
}