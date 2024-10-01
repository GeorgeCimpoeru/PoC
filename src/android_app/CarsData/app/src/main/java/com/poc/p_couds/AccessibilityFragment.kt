package com.poc.p_couds

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import androidx.fragment.app.Fragment

class AccessibilityFragment : Fragment() {

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_accessibility, container, false)
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