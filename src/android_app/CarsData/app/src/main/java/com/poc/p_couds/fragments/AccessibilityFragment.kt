package com.poc.p_couds.fragments

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.poc.p_couds.HandleNetworkReq
import com.poc.p_couds.R
import com.poc.p_couds.pojo.VINResponse
import com.poc.p_couds.activities.OTA
import com.poc.p_couds.activities.UDSactivity
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
    private lateinit var vinTextView: TextView

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
        vinTextView = view.findViewById(R.id.vin)

        val vin = arguments?.getString("VIN")
        if (vin != null) {
            fetchVinDetails(vin)
        }
        return view
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

                            val country = result.plantCountry.split(" ")
                            val formattedCountry = if (country.size > 1) {"${country[0]}\n${country[1]}\n${country[2]}"} else {result.plantCountry}
                            plantCountryTextView.text = formattedCountry
                            val vehicle = result.vehicleType.split(" ")
                            val formattedVehicle = if (vehicle.size > 1) {"${vehicle[0]}\n${vehicle[1]}"} else {result.vehicleType}
                            vehicleTypeTextView.text = formattedVehicle
                            errorCodeSecDigitTextView.text = result.errorCodeSecDigit
                            plantCityTextView.text = result.plantCity
                            val manufacturer = result.manufacturer.split(" ")
                            val formattedManufacturer = if (manufacturer.size > 1) {"${manufacturer[0]}\n${manufacturer[1]}\n${manufacturer[2]}\n${manufacturer[3]}"} else {result.manufacturer}
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

            val serialNo = vin.takeLast(6)
            serialNoTextView.text = serialNo
            vinTextView.text = vin
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