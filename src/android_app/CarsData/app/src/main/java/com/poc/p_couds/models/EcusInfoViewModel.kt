package com.poc.p_couds.models

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import com.example.jetpackgettingstarted.pojos.WriteBatteryChargeStateDataClass
import com.poc.p_couds.APIClient
import com.poc.p_couds.pojo.BatteryDataClass
import com.poc.p_couds.pojo.DoorsDataClass
import com.poc.p_couds.pojo.EngineDataClass
import com.poc.p_couds.pojo.HVACDataClass
import com.poc.p_couds.pojo.WriteBatteryLevelDataClass
import com.poc.p_couds.pojo.WriteBatteryPercentageDataClass
import com.poc.p_couds.pojo.WriteBatteryResponseDataClass
import com.poc.p_couds.pojo.WriteBatteryVoltageDataClass
import com.poc.p_couds.pojo.WriteDoorsAjarDataClass
import com.poc.p_couds.pojo.WriteDoorsDoorDataClass
import com.poc.p_couds.pojo.WriteDoorsPassengerDataClass
import com.poc.p_couds.pojo.WriteDoorsPassengerLockDataClass
import com.poc.p_couds.pojo.WriteDoorsResponseDataClass
import com.poc.p_couds.pojo.WriteEngineCoolantTempDataClass
import com.poc.p_couds.pojo.WriteEngineFuelLevelDataClass
import com.poc.p_couds.pojo.WriteEngineFuelPressureDataClass
import com.poc.p_couds.pojo.WriteEngineIntakeAirTempDataClass
import com.poc.p_couds.pojo.WriteEngineLoadDataClass
import com.poc.p_couds.pojo.WriteEngineOilTempDataClass
import com.poc.p_couds.pojo.WriteEngineResponseDataClass
import com.poc.p_couds.pojo.WriteEngineRpmDataClass
import com.poc.p_couds.pojo.WriteEngineSpeedDataClass
import com.poc.p_couds.pojo.WriteEngineThrottlePosDataClass
import com.poc.p_couds.pojo.WriteHvacAmbientAirTempDataClass
import com.poc.p_couds.pojo.WriteHvacCabinTempDataClass
import com.poc.p_couds.pojo.WriteHvacCabinTempDriverSetDataClass
import com.poc.p_couds.pojo.WriteHvacFanSpeedDataClass
import com.poc.p_couds.pojo.WriteHvacMassAirFlowDataClass
import com.poc.p_couds.pojo.WriteHvacModesDataClass
import com.poc.p_couds.pojo.WriteHvacResponseDataClass
import kotlinx.coroutines.launch
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response

class EcusInfoViewModel : ViewModel() {
    var batteryInfo by mutableStateOf<BatteryDataClass?>(null)
        private set
    var errorMessageBattery by mutableStateOf<String?>(null)
        private set
    fun fetchBatteryInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoBattery()
            call.enqueue(object : Callback<BatteryDataClass> {
                override fun onResponse(call: Call<BatteryDataClass>, response: Response<BatteryDataClass>) {
                    if (response.isSuccessful) {
                        batteryInfo = response.body()
                    } else {
                        errorMessageBattery = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<BatteryDataClass>, t: Throwable) {
                    errorMessageBattery = t.message
                }
            })
        }
    }

    fun writeBatteryInfo(inputText: String, title: String) {
        var writeBatteryInfoRequest: Any = ""
        when (title) {
            "Battery level" -> writeBatteryInfoRequest = WriteBatteryLevelDataClass(inputText)
            "State of charge" -> writeBatteryInfoRequest = WriteBatteryChargeStateDataClass(inputText)
            "Percentage" -> writeBatteryInfoRequest = WriteBatteryPercentageDataClass(inputText)
            "Voltage" -> writeBatteryInfoRequest = WriteBatteryVoltageDataClass(inputText)
        }

        viewModelScope.launch {
            try {
                val response: Response<WriteBatteryResponseDataClass> = APIClient.getClient().create(
                    IApiService::class.java).writeInfoBattery(writeBatteryInfoRequest)
                if (response.isSuccessful) {
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                } else {
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Log.e("API_ERROR", "Exception: $e")
            }
        }
    }

    var engineInfo by mutableStateOf<EngineDataClass?>(null)
        private set
    var errorMessageEngine by mutableStateOf<String?>(null)
        private set
    fun fetchEngineInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoEngine()
            call.enqueue(object : Callback<EngineDataClass> {
                override fun onResponse(call: Call<EngineDataClass>, response: Response<EngineDataClass>) {
                    if (response.isSuccessful) {
                        engineInfo = response.body()
                    } else {
                        errorMessageEngine = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<EngineDataClass>, t: Throwable) {
                    errorMessageEngine = t.message
                }
            })
        }
    }

    fun writeEngineInfo(inputText: String, title: String) {
        var writeEngineInfoRequest: Any = ""
        when (title) {
            "Coolant temp" -> writeEngineInfoRequest = WriteEngineCoolantTempDataClass(inputText)
            "Load" -> writeEngineInfoRequest = WriteEngineLoadDataClass(inputText)
            "Rpm" -> writeEngineInfoRequest = WriteEngineRpmDataClass(inputText)
            "Fuel" -> writeEngineInfoRequest = WriteEngineFuelLevelDataClass(inputText)
            "Fuel pressure" -> writeEngineInfoRequest = WriteEngineFuelPressureDataClass(inputText)
            "Intake air temp" -> writeEngineInfoRequest = WriteEngineIntakeAirTempDataClass(inputText)
            "Oil temp" -> writeEngineInfoRequest = WriteEngineOilTempDataClass(inputText)
            "Throttle position" -> writeEngineInfoRequest = WriteEngineThrottlePosDataClass(inputText)
            "Speed" -> writeEngineInfoRequest = WriteEngineSpeedDataClass(inputText)
        }

        viewModelScope.launch {
            try {
                val response: Response<WriteEngineResponseDataClass> = APIClient.getClient().create(
                    IApiService::class.java).writeInfoEngine(writeEngineInfoRequest)
                if (response.isSuccessful) {
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                } else {
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Log.e("API_ERROR", "Exception: $e")
            }
        }
    }

    var doorsInfo by mutableStateOf<DoorsDataClass?>(null)
        private set
    var errorMessageDoors by mutableStateOf<String?>(null)
        private set
    fun fetchDoorsInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoDoors()
            call.enqueue(object : Callback<DoorsDataClass> {
                override fun onResponse(call: Call<DoorsDataClass>, response: Response<DoorsDataClass>) {
                    if (response.isSuccessful) {
                        doorsInfo = response.body()
                    } else {
                        errorMessageDoors = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<DoorsDataClass>, t: Throwable) {
                    errorMessageDoors = t.message
                }
            })
        }
    }

    fun writeDoorsInfo(inputText: String, title: String) {
        var writeDoorsInfoRequest: Any = ""
        when (title) {
            "Ajar" -> writeDoorsInfoRequest = WriteDoorsAjarDataClass(inputText)
            "Door" -> writeDoorsInfoRequest = WriteDoorsDoorDataClass(inputText)
            "Passanger" -> writeDoorsInfoRequest = WriteDoorsPassengerDataClass(inputText)
            "Passanger lock" -> writeDoorsInfoRequest = WriteDoorsPassengerLockDataClass(inputText)
        }

        viewModelScope.launch {
            try {
                val response: Response<WriteDoorsResponseDataClass> = APIClient.getClient().create(
                    IApiService::class.java).writeInfoDoors(writeDoorsInfoRequest)
                if (response.isSuccessful) {
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                } else {
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Log.e("API_ERROR", "Exception: $e")
            }
        }
    }

    var hvacInfo by mutableStateOf<HVACDataClass?>(null)
        private set
    var errorMessageHvac by mutableStateOf<String?>(null)
        private set
    fun fetchHvacInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoHVAC()
            call.enqueue(object : Callback<HVACDataClass> {
                override fun onResponse(call: Call<HVACDataClass>, response: Response<HVACDataClass>) {
                    if (response.isSuccessful) {
                        hvacInfo = response.body()
                    } else {
                        errorMessageHvac = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<HVACDataClass>, t: Throwable) {
                    errorMessageHvac = t.message
                }
            })
        }
    }

    fun writeHVACInfo(inputText: String, title: String) {
        var writeHVACInfoRequest: Any = ""
        when (title) {
            "Ambient temp" -> writeHVACInfoRequest = WriteHvacAmbientAirTempDataClass(inputText)
            "Cabin temp" -> writeHVACInfoRequest = WriteHvacCabinTempDataClass(inputText)
            "Driver set temp" -> writeHVACInfoRequest = WriteHvacCabinTempDriverSetDataClass(inputText)
            "Fan speed" -> writeHVACInfoRequest = WriteHvacFanSpeedDataClass(inputText)
            "AC status" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Air recirc" -> writeHVACInfoRequest =WriteHvacModesDataClass(inputText)
            "Defrost" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Front" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Legs" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Mass air flow" -> writeHVACInfoRequest = WriteHvacMassAirFlowDataClass(inputText)
        }

        viewModelScope.launch {
            try {
                val response: Response<WriteHvacResponseDataClass> = APIClient.getClient().create(
                    IApiService::class.java).writeInfoHVAC(writeHVACInfoRequest)
                if (response.isSuccessful) {
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                } else {
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Log.e("API_ERROR", "Exception: $e")
            }
        }
    }
}
