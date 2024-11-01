package com.poc.p_couds.fragments

import android.content.Context.MODE_PRIVATE
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.poc.p_couds.HandleNetworkReq
import com.poc.p_couds.R
import com.poc.p_couds.activities.OTA
import com.poc.p_couds.activities.UDSactivity
import com.poc.p_couds.pojo.VINResponse
import retrofit2.Call
import retrofit2.Response


class AccessibilityFragment : Fragment() {
    private lateinit var plantCountryTextView: TextView
    private lateinit var vehicleTypeTextView: TextView
    private lateinit var errorCodeSecDigitTextView: TextView
    private lateinit var plantCityTextView: TextView
    private lateinit var manufacturerTextView: TextView
    private lateinit var yearTextView: TextView
    private lateinit var serialNoTextView: TextView
    private lateinit var bodyClassEngineHPModelTextView: TextView

    private lateinit var vin1TextView: TextView
    private lateinit var vin2TextView: TextView
    private lateinit var vin3TextView: TextView
    private lateinit var vin4TextView: TextView
    private lateinit var vin5TextView: TextView
    private lateinit var vin6TextView: TextView
    private lateinit var vin7TextView: TextView
    private lateinit var vin8TextView: TextView


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.fragment_accessibility, container, false)
        plantCountryTextView = view.findViewById(R.id.plantCountryLabel)
        vehicleTypeTextView = view.findViewById(R.id.vehicleTypeLabel)
        errorCodeSecDigitTextView = view.findViewById(R.id.ErrorTextLabel)
        plantCityTextView = view.findViewById(R.id.plantCityLabel)
        manufacturerTextView = view.findViewById(R.id.manufacturerLabel)
        bodyClassEngineHPModelTextView = view.findViewById(R.id.bodyClass_engineHP_engineModel_model_Label)
        yearTextView = view.findViewById(R.id.modelYearLabel)
        serialNoTextView = view.findViewById(R.id.serialNoLabel)

        vin1TextView = view.findViewById(R.id.vin1)
        vin2TextView = view.findViewById(R.id.vin2)
        vin3TextView = view.findViewById(R.id.vin3)
        vin4TextView = view.findViewById(R.id.vin4)
        vin5TextView = view.findViewById(R.id.vin5)
        vin6TextView = view.findViewById(R.id.vin6)
        vin7TextView = view.findViewById(R.id.vin7)
        vin8TextView = view.findViewById(R.id.vin8)

        var vin = arguments?.getString("VIN")
        if (vin != null) {
            fetchVinDetails(vin)
            // Save VIN in the local storage
            val sharedPreferences = activity?.getSharedPreferences("MySharedPref", MODE_PRIVATE)
            if (sharedPreferences != null)
            {
                with (sharedPreferences.edit()){
                    putString("VIN", vin)
                    apply()
                }
            }
        }
        else
        {
            // Search for VIN in local storage
            val sharedPreferences = activity?.getSharedPreferences("MySharedPref", MODE_PRIVATE)
            if (sharedPreferences != null)
            {
                vin = sharedPreferences.getString("VIN","")
                if (vin != null) {
                    fetchVinDetails(vin)
                }
                else
                {
                    Log.w("PoC","Unavailable to find VIN from input or in local storage")
                }
            }
        }
        return view
    }

    private fun updateVinTextViews(vin: String) {
        val vinDefaults = MutableList(17) { "null" }

        // Iterate over the indices of the vin string
        for (i in vin.indices) {
            if (i < vinDefaults.size) {
                // Assign the character from vin to vinDefaults at index i
                vinDefaults[i] = vin[i].toString()
            }
        }

        // Ensure the VIN has at least 8 characters
        if (vin.length >= 11) {
            vin1TextView.text = vinDefaults[0]
            vin2TextView.text = vinDefaults[1]
            vin3TextView.text = vinDefaults[2]
            vin4TextView.text = vinDefaults.subList(3,8).joinToString ("")
            vin5TextView.text = vinDefaults[8]
            vin6TextView.text = vinDefaults[9]
            vin7TextView.text = vinDefaults[10]
            val subListVin8 = vinDefaults.subList(11, 17)

            // Check if all values are "null"
            val combinedVin8 = if (subListVin8.all { it == "null" || it.isBlank() }) {
                "null" // Show "null" only once if all are "null"
            } else {
                subListVin8.joinToString("") // Concatenate valid values
            }
            vin8TextView.text = combinedVin8
        } else {
            // Handle cases where the VIN is shorter than 11 characters
            Toast.makeText(context, "VIN is too short", Toast.LENGTH_SHORT).show()
        }
    }

    private fun fetchVinDetails(vin:String) {
        val call = HandleNetworkReq.api.getVinDetails(vin)
        call.enqueue(object : retrofit2.Callback<VINResponse> {
            override fun onResponse(call: Call<VINResponse>, response: Response<VINResponse>) {
                if (response.isSuccessful) {
                    val vinResponse = response.body()
                    vinResponse?.let {
                        if (it.results.isNotEmpty()) {
                            val result = it.results[0]

                            // Utility function to split and join words with new lines
                            fun splitAndFormat(text: String): String {
                                return text.split(" ").joinToString(separator = "\n") { it.trim() }
                            }

                            val formattedCountry = if (result.plantCountry.isNotEmpty()) {
                                splitAndFormat(result.plantCountry)
                            }  else { ""
                            }
                            plantCountryTextView.text = formattedCountry

                            val formattedVehicle = if(result.vehicleType.isNotEmpty()) {
                                splitAndFormat(result.vehicleType)
                            }else {  ""
                            }
                            vehicleTypeTextView.text = formattedVehicle

                            errorCodeSecDigitTextView.text = result.errorCodeSecDigit

                            plantCityTextView.text = result.plantCity

                            val formattedManufacturer = if(result.manufacturer.isNotEmpty()) {
                                splitAndFormat(result.manufacturer)
                            }else { ""
                            }
                            manufacturerTextView.text = formattedManufacturer

                            yearTextView.text = result.year
                            updateTextView(it,vin)
                            moreInfo(it)
                        }
                    }
                }
            }

            override fun onFailure(call: Call<VINResponse>, t: Throwable) {
                Toast.makeText(context, "Failed to fetch data", Toast.LENGTH_SHORT).show()
            }
        })
    }

    private fun updateTextView(vinResponse: VINResponse, vin: String) {
        if (vinResponse.results.isNotEmpty()) {
            val result = vinResponse.results[0]
            val combinedInfo =
                "(${result.bodyClass}\n${result.engineHP}\n${result.engineModel}\n${result.model}\n${result.fuel}\n${result.transmission})"
            bodyClassEngineHPModelTextView.text = combinedInfo

            if (vin.length == 17) {
                val serialNo = vin.takeLast(6)
                serialNoTextView.text = serialNo
            } else {
                serialNoTextView.text = "Not provided"
            }
            updateVinTextViews(vin)
        }
    }

    private fun moreInfo(vinResponse: VINResponse) {
        val combinedInfoTextView = view?.findViewById<TextView>(R.id.combined_info_text)
        val moreInfoButton = view?.findViewById<Button>(R.id.info_button)
        if (vinResponse.results.isNotEmpty()) {
            view?.findViewById<Button>(R.id.info_button)?.setOnClickListener {
                if (combinedInfoTextView != null) {
                    if (combinedInfoTextView.visibility == View.GONE) {
                        val result = vinResponse.results[0]

                        // Create a combinedInfo string with labels and values
                        val combinedInfo = """
                                                    Body Class: ${result.bodyClass}
                                                    Engine HP: ${result.engineHP}
                                                    Engine Model: ${result.engineModel}
                                                    Model: ${result.model}
                                                    Fuel Type: ${result.fuel}
                                                    Transmission: ${result.transmission}
                                                    Check digit: ${result.errorTextSecDigit}
                                                """.trimIndent()

                        // Update the TextView with the formatted combinedInfo
                        combinedInfoTextView.text = combinedInfo
                        combinedInfoTextView.visibility = View.VISIBLE
                        if (moreInfoButton != null) {
                            moreInfoButton.text = getString(R.string.less_info)
                        }

                    } else {
                        combinedInfoTextView.visibility = View.GONE
                        if (moreInfoButton != null) {
                            moreInfoButton.text = getString(R.string.more_info)
                        }
                    }
                }
            }
        }
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val udsButton = view.findViewById<Button>(R.id.UDS_action_button)
        val otaButton = view.findViewById<Button>(R.id.OTA_action_button)

        udsButton.setOnClickListener {
            navigateToUDSActivity()
        }

        otaButton.setOnClickListener {
            navigateToOTAActivity()
        }
    }

    private fun navigateToUDSActivity() {
        val intent = Intent(requireContext(), UDSactivity::class.java)
        startActivity(intent)
    }

    private fun navigateToOTAActivity() {
        val intent = Intent(requireContext(), OTA::class.java)
        startActivity(intent)
    }
}