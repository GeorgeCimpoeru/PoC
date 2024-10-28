package com.poc.p_couds.models

import android.content.Context
import android.util.Log
import android.widget.Toast
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import kotlinx.coroutines.launch
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import com.poc.p_couds.pojo.BatteryDataClass
import com.poc.p_couds.pojo.DoorsDataClass
import com.poc.p_couds.pojo.EngineDataClass
import com.poc.p_couds.pojo.HVACDataClass
import com.poc.p_couds.pojo.WriteBatteryChargeStateDataClass
import com.poc.p_couds.pojo.WriteBatteryLevelDataClass
import com.poc.p_couds.pojo.WriteBatteryPercentageDataClass
import com.poc.p_couds.pojo.WriteBatteryVoltageDataClass
import com.poc.p_couds.pojo.WriteBatteryResponseDataClass
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
import com.google.gson.Gson
import com.poc.p_couds.APIClient
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow

class EcusInfoViewModel : ViewModel() {
    var batteryInfo by mutableStateOf<BatteryDataClass?>(null)
        private set
    var errorMessageBattery by mutableStateOf<String?>(null)
        private set

    private val _loading = MutableStateFlow(false)
    val loading: StateFlow<Boolean> get() = _loading

    fun fetchBatteryInfo(context: Context) {
        _loading.value = true
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoBattery()
            call.enqueue(object : Callback<BatteryDataClass> {
                override fun onResponse(
                    call: Call<BatteryDataClass>,
                    response: Response<BatteryDataClass>
                ) {
                    if (response.isSuccessful) {
                        batteryInfo = response.body()
                    } else {
                        Toast.makeText(context, "Read battery info failed", Toast.LENGTH_SHORT)
                            .show()
                        errorMessageBattery = "Failed to retrieve data"
                    }
                    _loading.value = false
                }

                override fun onFailure(call: Call<BatteryDataClass>, t: Throwable) {
                    Toast.makeText(context, "Read battery info failed", Toast.LENGTH_SHORT).show()
                    errorMessageBattery = t.message
                    _loading.value = false
                }
            })
        }
    }

    fun writeBatteryInfo(context: Context, inputText: String, title: String) {
        var writeBatteryInfoRequest: Any = ""
        when (title) {
            "Battery level" -> writeBatteryInfoRequest = WriteBatteryLevelDataClass(inputText)
            "State of charge" -> writeBatteryInfoRequest =
                WriteBatteryChargeStateDataClass(inputText)

            "Percentage" -> writeBatteryInfoRequest = WriteBatteryPercentageDataClass(inputText)
            "Voltage" -> writeBatteryInfoRequest = WriteBatteryVoltageDataClass(inputText)
        }
        viewModelScope.launch {
            _loading.value = true
            delay(1000L)
            try {
                val response = APIClient.getClient().create(IApiService::class.java)
                    .writeInfoBattery(writeBatteryInfoRequest)
                if (response.isSuccessful && response.body() != null) {
                    val gson: Gson = Gson()
                    val jsonResponse = gson.toJson(response.body())
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                    Log.d("API_SUCCESS", "Post created: $jsonResponse")
                    when (title) {
                        "Battery level" -> batteryInfo?.battery_level = inputText
                        "State of charge" -> batteryInfo?.battery_state_of_charge = inputText
                        "Percentage" -> batteryInfo?.percentage = inputText
                        "Voltage" -> batteryInfo?.voltage = inputText
                    }
                } else {
                    Toast.makeText(context, "Write battery info failed", Toast.LENGTH_SHORT).show()
                    errorMessageBattery = "Failed to retrieve data"
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Toast.makeText(context, "Write battery info failed", Toast.LENGTH_SHORT).show()
                errorMessageBattery = e.message ?: "Unknown error"
                Log.e("API_ERROR", "Exception: $e")
            } finally {
                _loading.value = false
            }
        }
    }

    var engineInfo by mutableStateOf<EngineDataClass?>(null)
        private set
    var errorMessageEngine by mutableStateOf<String?>(null)
        private set

    fun fetchEngineInfo(context: Context) {
        _loading.value = true
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoEngine()
            call.enqueue(object : Callback<EngineDataClass> {
                override fun onResponse(
                    call: Call<EngineDataClass>,
                    response: Response<EngineDataClass>
                ) {
                    if (response.isSuccessful) {
                        engineInfo = response.body()
                    } else {
                        Toast.makeText(context, "Read engine info failed", Toast.LENGTH_SHORT)
                            .show()
                        errorMessageEngine = "Failed to retrieve data"
                    }
                    _loading.value = false
                }

                override fun onFailure(call: Call<EngineDataClass>, t: Throwable) {
                    Toast.makeText(context, "Read engine info failed", Toast.LENGTH_SHORT).show()
                    errorMessageEngine = t.message
                    _loading.value = false
                }
            })
        }
    }

    fun writeEngineInfo(context: Context, inputText: String, title: String) {
        var writeEngineInfoRequest: Any = ""
        when (title) {
            "Coolant temp" -> writeEngineInfoRequest = WriteEngineCoolantTempDataClass(inputText)
            "Load" -> writeEngineInfoRequest = WriteEngineLoadDataClass(inputText)
            "Rpm" -> writeEngineInfoRequest = WriteEngineRpmDataClass(inputText)
            "Fuel" -> writeEngineInfoRequest = WriteEngineFuelLevelDataClass(inputText)
            "Fuel pressure" -> writeEngineInfoRequest = WriteEngineFuelPressureDataClass(inputText)
            "Intake air temp" -> writeEngineInfoRequest =
                WriteEngineIntakeAirTempDataClass(inputText)

            "Oil temp" -> writeEngineInfoRequest = WriteEngineOilTempDataClass(inputText)
            "Throttle position" -> writeEngineInfoRequest =
                WriteEngineThrottlePosDataClass(inputText)

            "Speed" -> writeEngineInfoRequest = WriteEngineSpeedDataClass(inputText)
        }
        viewModelScope.launch {
            _loading.value = true
            delay(1000L)
            try {
                val response: Response<Any> =
                    APIClient.getClient().create(IApiService::class.java)
                        .writeInfoEngine(writeEngineInfoRequest)
                if (response.isSuccessful && response.body() != null) {
                    val gson: Gson = Gson()
                    val jsonResponse = gson.toJson(response.body())
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                    Log.d("API_SUCCESS", "Post created: $jsonResponse")
                    when (title) {
                        "Coolant temp" -> engineInfo?.coolant_temperature = inputText
                        "Load" -> engineInfo?.engine_load = inputText
                        "Rpm" -> engineInfo?.engine_rpm = inputText
                        "Fuel" -> engineInfo?.fuel_level = inputText
                        "Fuel pressure" -> engineInfo?.fuel_pressure = inputText
                        "Intake air temp" -> engineInfo?.intake_air_temperature = inputText
                        "Oil temp" -> engineInfo?.oil_temperature = inputText
                        "Throttle position" -> engineInfo?.throttle_position = inputText
                        "Speed" -> engineInfo?.vehicle_speed = inputText
                    }
                } else {
                    Toast.makeText(context, "Write engine info failed", Toast.LENGTH_SHORT).show()
                    errorMessageBattery = "Failed to retrieve data"
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Toast.makeText(context, "Write engine info failed", Toast.LENGTH_SHORT).show()
                errorMessageBattery = e.message ?: "Unknown error"
                Log.e("API_ERROR", "Exception: $e")
            } finally {
                _loading.value = false
            }
        }
    }

    var doorsInfo by mutableStateOf<DoorsDataClass?>(null)
        private set
    var errorMessageDoors by mutableStateOf<String?>(null)
        private set

    fun fetchDoorsInfo(context: Context) {
        _loading.value = true
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoDoors()
            call.enqueue(object : Callback<DoorsDataClass> {
                override fun onResponse(
                    call: Call<DoorsDataClass>,
                    response: Response<DoorsDataClass>
                ) {
                    if (response.isSuccessful) {
                        doorsInfo = response.body()
                    } else {
                        Toast.makeText(context, "Read doors info failed", Toast.LENGTH_SHORT).show()
                        errorMessageDoors = "Failed to retrieve data"
                    }
                    _loading.value = false
                }

                override fun onFailure(call: Call<DoorsDataClass>, t: Throwable) {
                    Toast.makeText(context, "Read doors info failed", Toast.LENGTH_SHORT).show()
                    errorMessageDoors = t.message
                    _loading.value = false
                }
            })
        }
    }

    fun writeDoorsInfo(context: Context, inputText: String, title: String) {
        var writeDoorsInfoRequest: Any = ""
        when (title) {
            "Ajar" -> writeDoorsInfoRequest = WriteDoorsAjarDataClass(inputText)
            "Door" -> writeDoorsInfoRequest = WriteDoorsDoorDataClass(inputText)
            "Passanger" -> writeDoorsInfoRequest = WriteDoorsPassengerDataClass(inputText)
            "Passanger lock" -> writeDoorsInfoRequest = WriteDoorsPassengerLockDataClass(inputText)
        }
        viewModelScope.launch {
            _loading.value = true
            delay(1000L)
            try {
                val response: Response<Any> =
                    APIClient.getClient().create(IApiService::class.java)
                        .writeInfoDoors(writeDoorsInfoRequest)
                if (response.isSuccessful && response.body() != null) {
                    val gson: Gson = Gson()
                    val jsonResponse = gson.toJson(response.body())
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                    Log.d("API_SUCCESS", "Post created: $jsonResponse")
                    when (title) {
                        "Ajar" -> doorsInfo?.ajar = inputText
                        "Door" -> doorsInfo?.door = inputText
                        "Passanger" -> doorsInfo?.passenger = inputText
                        "Passanger lock" -> doorsInfo?.passenger_lock = inputText
                    }
                } else {
                    Toast.makeText(context, "Write doors info failed", Toast.LENGTH_SHORT).show()
                    errorMessageBattery = "Failed to retrieve data"
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Toast.makeText(context, "Write doors info failed", Toast.LENGTH_SHORT).show()
                errorMessageBattery = e.message ?: "Unknown error"
                Log.e("API_ERROR", "Exception: $e")
            } finally {
                _loading.value = false
            }
        }
    }

    var hvacInfo by mutableStateOf<HVACDataClass?>(null)
        private set
    var errorMessageHvac by mutableStateOf<String?>(null)
        private set

    fun fetchHvacInfo(context: Context) {
        _loading.value = true
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoHVAC()
            call.enqueue(object : Callback<HVACDataClass> {
                override fun onResponse(
                    call: Call<HVACDataClass>,
                    response: Response<HVACDataClass>
                ) {
                    if (response.isSuccessful) {
                        hvacInfo = response.body()
                    } else {
                        Toast.makeText(context, "Read HVAC info failed", Toast.LENGTH_SHORT).show()
                        errorMessageHvac = "Failed to retrieve data"
                    }
                    _loading.value = false
                }

                override fun onFailure(call: Call<HVACDataClass>, t: Throwable) {
                    Toast.makeText(context, "Read HVAC info failed", Toast.LENGTH_SHORT).show()
                    errorMessageHvac = t.message
                    _loading.value = false
                }
            })
        }
    }

    fun writeHVACInfo(context: Context, inputText: String, title: String) {
        var writeHVACInfoRequest: Any = ""
        when (title) {
            "Ambient temp" -> writeHVACInfoRequest = WriteHvacAmbientAirTempDataClass(inputText)
            "Cabin temp" -> writeHVACInfoRequest = WriteHvacCabinTempDataClass(inputText)
            "Driver set temp" -> writeHVACInfoRequest =
                WriteHvacCabinTempDriverSetDataClass(inputText)

            "Fan speed" -> writeHVACInfoRequest = WriteHvacFanSpeedDataClass(inputText)
            "AC status" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Air recirc" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Defrost" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Front" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Legs" -> writeHVACInfoRequest = WriteHvacModesDataClass(inputText)
            "Mass air flow" -> writeHVACInfoRequest = WriteHvacMassAirFlowDataClass(inputText)
        }
        viewModelScope.launch {
            _loading.value = true
            delay(1000L)
            try {
                val response: Response<Any> = APIClient.getClient().create(IApiService::class.java).writeInfoHVAC(writeHVACInfoRequest)
                if (response.isSuccessful && response.body() != null) {
                    val gson: Gson = Gson()
                    val jsonResponse = gson.toJson(response.body())
                    Log.d("API_SUCCESS", "Post created: ${response.body()}")
                    Log.d("API_SUCCESS", "Post created: $jsonResponse")
                    when (title) {
                        "Ambient temp" -> hvacInfo?.ambient_air_temperature = inputText
                        "Cabin temp" -> hvacInfo?.cabin_temperature = inputText
                        "Driver set temp" -> hvacInfo?.cabin_temperature_driver_set = inputText
                        "Fan speed" -> hvacInfo?.fan_speed = inputText
                        "AC status" -> hvacInfo?.hvac_modes?.AC_Status = inputText
                        "Air recirc" -> hvacInfo?.hvac_modes?.Air_Recirculation = inputText
                        "Defrost" -> hvacInfo?.hvac_modes?.Defrost = inputText
                        "Front" -> hvacInfo?.hvac_modes?.Front = inputText
                        "Legs" -> hvacInfo?.hvac_modes?.Legs = inputText
                        "Mass air flow" -> hvacInfo?.mass_air_flow = inputText
                    }
                } else {
                    Toast.makeText(context, "Write HVAC info failed", Toast.LENGTH_SHORT).show()
                    errorMessageBattery = "Failed to retrieve data"
                    Log.d("API_ERROR", "Failed to create post: ${response.errorBody()}")
                }
            } catch (e: Exception) {
                Toast.makeText(context, "Write HVAC info failed", Toast.LENGTH_SHORT).show()
                errorMessageBattery = e.message ?: "Unknown error"
                Log.e("API_ERROR", "Exception: $e")
            } finally {
                _loading.value = false
            }
        }
    }


    var batteryDTC by mutableStateOf<Any?>(null)
        private set

    suspend fun readBatteryDTC(context: Context) {
        _loading.value = true
        delay(5000L)
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getBatteryDTC()
            call.enqueue(object : Callback<Any> {
                override fun onResponse(call: Call<Any>, response: Response<Any>) {
                    if (response.isSuccessful) {
                        batteryDTC = response.body()
                    } else {
                        Toast.makeText(context, "Read battery DTC failed", Toast.LENGTH_SHORT)
                            .show()
                        errorMessageHvac = "Failed to retrieve data"
                    }
                    _loading.value = false
                }

                override fun onFailure(call: Call<Any>, t: Throwable) {
                    Toast.makeText(context, "Read battery DTC failed", Toast.LENGTH_SHORT).show()
                    errorMessageHvac = t.message
                    _loading.value = false
                }
            })
        }
    }
}