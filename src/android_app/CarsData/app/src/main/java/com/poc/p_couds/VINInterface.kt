package com.poc.p_couds

import retrofit2.Call
import retrofit2.http.GET
import retrofit2.http.Path

interface VINInterface {
    @GET("DecodeVinValues/{vin}?format=json")
    fun getVinDetails(@Path("vin") vin:String): Call<VINResponse>
}