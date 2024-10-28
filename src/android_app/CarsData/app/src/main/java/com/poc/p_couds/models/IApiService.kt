package com.poc.p_couds.models

import com.poc.p_couds.pojo.WriteBatteryResponseDataClass
import com.poc.p_couds.pojo.WriteDoorsResponseDataClass
import com.poc.p_couds.pojo.WriteEngineResponseDataClass
import com.poc.p_couds.pojo.WriteHvacResponseDataClass
import com.poc.p_couds.pojo.Authenticate
import com.poc.p_couds.pojo.BatteryDataClass
import com.poc.p_couds.pojo.ChangeStatus
import com.poc.p_couds.pojo.ChangeStatusSession
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
import com.poc.p_couds.pojo.WriteTiming
import com.poc.p_couds.pojo.WriteTimingPost
import retrofit2.Call
import retrofit2.Response
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.POST
import retrofit2.http.Path

interface IApiService {

    //UDS
    @GET("api/read_info_battery?is_manual_flow=false")
    fun getInfoBattery(): Call<BatteryDataClass>

    @GET("api/read_info_engine?is_manual_flow=false")
    fun getInfoEngine(): Call<EngineDataClass>

    @GET("api/read_info_doors?is_manual_flow=false")
    fun getInfoDoors(): Call<DoorsDataClass>

    @GET("api/read_info_hvac?is_manual_flow=false")
    fun getInfoHVAC(): Call<HVACDataClass>

    @POST("api/write_info_battery")
    suspend fun writeInfoBattery(
        @Body request: Any
    ): Response<Any>

    @POST("api/write_info_engine")
    suspend fun writeInfoEngine(
        @Body request: Any
    ): Response<Any>

    @POST("api/write_info_doors")
    suspend fun writeInfoDoors(
        @Body request: Any
    ): Response<Any>

    @POST("api/write_info_hvac")
    suspend fun writeInfoHVAC(
        @Body request: Any
    ): Response<Any>

    @GET("api/read_dtc_info")
    fun getBatteryDTC(): Call<Any>

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

    @POST("/api/change_session")
    fun requestChangeStatus(@Body changeStatusSession: ChangeStatusSession): Call<ChangeStatus>

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

    @POST("/api/write_timing")
    fun requestWriteTiming(@Body writeTimingPost: WriteTimingPost): Call<WriteTiming>
}