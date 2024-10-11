package com.poc.p_couds.pojo

import com.google.gson.annotations.SerializedName
data class VINResponse(
    @SerializedName("Count") val count: Int,
    @SerializedName("Message") val message: String,
    @SerializedName("SearchCriteria") val searchCriteria: String,
    @SerializedName("Results") val results: List<VINResult>
)
data class VINResult (
    @SerializedName("PlantCountry") val plantCountry: String,
    @SerializedName("VehicleType") val vehicleType: String,
    @SerializedName("ErrorCode") val errorCodeSecDigit: String,
    @SerializedName("ErrorText") val errorTextSecDigit: String,
    @SerializedName("PlantCity") val plantCity: String,
    @SerializedName("Manufacturer") val manufacturer: String,

    @SerializedName("BodyClass")val bodyClass: String,
    @SerializedName("EngineHP") val engineHP: String,
    @SerializedName("EngineModel") val engineModel: String,
    @SerializedName("Model") val model: String,
    @SerializedName("FuelTypePrimary") val fuel: String,
    @SerializedName("TransmissionStyle") val transmission: String,

    @SerializedName("ModelYear") val year: String,
)
