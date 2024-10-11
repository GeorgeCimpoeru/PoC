package com.poc.p_couds

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import kotlinx.coroutines.launch
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import android.util.Log
import com.poc.p_couds.pojo.BatteryDataClass
import com.poc.p_couds.pojo.DoorsDataClass
import com.poc.p_couds.pojo.EngineDataClass
import com.poc.p_couds.pojo.HVACDataClass
import retrofit2.create

class EcusInfoViewModel : ViewModel() {
    var batteryInfo by mutableStateOf<BatteryDataClass?>(null)
        private set
    var errorMessageBattery by mutableStateOf<String?>(null)
        private set
    fun fetchBatteryInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoBattery();
            call.enqueue(object : Callback<BatteryDataClass> {
                override fun onResponse(call: Call<BatteryDataClass>, response: Response<BatteryDataClass>) {
                    Log.d("error1", response.toString())
                    if (response.isSuccessful) {
                        batteryInfo = response.body()
                    } else {
                        errorMessageBattery = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<BatteryDataClass>, t: Throwable) {
                    Log.d("error1", "${t}")
                    errorMessageBattery = t.message
                }
            })
        }
    }

    var engineInfo by mutableStateOf<EngineDataClass?>(null)
        private set
    var errorMessageEngine by mutableStateOf<String?>(null)
        private set
    fun fetchEngineInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoEngine();
            call.enqueue(object : Callback<EngineDataClass> {
                override fun onResponse(call: Call<EngineDataClass>, response: Response<EngineDataClass>) {
                    Log.d("error1", response.toString())
                    if (response.isSuccessful) {
                        engineInfo = response.body()
                    } else {
                        errorMessageEngine = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<EngineDataClass>, t: Throwable) {
                    Log.d("error1", "${t}")
                    errorMessageEngine = t.message
                }
            })
        }
    }

    var doorsInfo by mutableStateOf<DoorsDataClass?>(null)
        private set
    var errorMessageDoors by mutableStateOf<String?>(null)
        private set
    fun fetchDoorsInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoDoors();
            call.enqueue(object : Callback<DoorsDataClass> {
                override fun onResponse(call: Call<DoorsDataClass>, response: Response<DoorsDataClass>) {
                    Log.d("error1", response.toString())
                    if (response.isSuccessful) {
                        doorsInfo = response.body()
                    } else {
                        errorMessageDoors = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<DoorsDataClass>, t: Throwable) {
                    Log.d("error1", "${t}")
                    errorMessageDoors = t.message
                }
            })
        }
    }

    var hvacInfo by mutableStateOf<HVACDataClass?>(null)
        private set
    var errorMessageHvac by mutableStateOf<String?>(null)
        private set
    fun fetchHvacInfo() {
        viewModelScope.launch {
            val call = APIClient.getClient().create(IApiService::class.java).getInfoHVAC();
            call.enqueue(object : Callback<HVACDataClass> {
                override fun onResponse(call: Call<HVACDataClass>, response: Response<HVACDataClass>) {
                    Log.d("error1", response.toString())
                    if (response.isSuccessful) {
                        hvacInfo = response.body()
                    } else {
                        errorMessageHvac = "Failed to retrieve data"
                    }
                }

                override fun onFailure(call: Call<HVACDataClass>, t: Throwable) {
                    Log.d("error1", "${t}")
                    errorMessageHvac = t.message
                }
            })
        }
    }
}
