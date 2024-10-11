package com.poc.p_couds.fragments

import android.annotation.SuppressLint
import android.content.Context
import android.content.SharedPreferences
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Share
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.ComposeView
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.poc.p_couds.R
import androidx.fragment.app.FragmentActivity
import com.poc.p_couds.ui.theme.CarsDataTheme


class HomeFragment : Fragment() {
    private lateinit var sharedPreferences: SharedPreferences

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        sharedPreferences = requireActivity().getSharedPreferences("Preferences", Context.MODE_PRIVATE)

        return ComposeView(requireContext()).apply {
            setContent {
                CarsDataTheme {
                          val isLoggedIn = sharedPreferences.getBoolean("isLoggedIn",false)
                          HomeScreen(onVinSubmitted = { vinText ->
                              val accessibilityFragment = AccessibilityFragment()
                              val bundle = Bundle()
                              bundle.putString("VIN", vinText)
                              accessibilityFragment.arguments = bundle
                              navigateToAccessibilityFragment(activity, accessibilityFragment)
                          }, isLoggedIn = isLoggedIn)
                }
            }
        }
    }

    @Composable
    fun HomeScreen(onVinSubmitted: (String) -> Unit, isLoggedIn: Boolean) {
        // State for the dialog
        var showDialog by remember { mutableStateOf(false) }
        var vinText by remember { mutableStateOf("") }
        val context = LocalContext.current

        ScrollableContent(onShowDialog = {
            if (isLoggedIn) {
                showDialog = true
            } else {
                navigateToLogInFragment(context)
            }
        })

        // Show dialog when `showDialog` is true
        VinDialog(
            showDialog = showDialog,
            onDismiss = { showDialog = false },
            onSend = { vin ->
                vinText = vin
                onVinSubmitted(vin) // Trigger fragment navigation
                showDialog = false
            }
        )
    }

    private fun navigateToAccessibilityFragment(activity: FragmentActivity?, accessibilityFragment: AccessibilityFragment) {

        val fragmentManager = activity?.supportFragmentManager
        fragmentManager?.beginTransaction()
            ?.replace(R.id.fragment_container_view, accessibilityFragment)
            ?.addToBackStack(null)
            ?.commit()
    }

    private fun navigateToLogInFragment(context: Context) {
        val activity = context as? AppCompatActivity
        val fragment = LogInFragment()

        val fragmentManager = activity?.supportFragmentManager
        fragmentManager?.beginTransaction()
            ?.replace(R.id.fragment_container_view, fragment)
            ?.addToBackStack(null)
            ?.commit()
    }

    @Composable
    fun VinDialog(
        showDialog: Boolean,
        onDismiss: () -> Unit,
        onSend: (String) -> Unit
    ) {
        var vinText by remember { mutableStateOf("") }
        if( showDialog) {
            AlertDialog(
                onDismissRequest =  onDismiss,
                title = { Text(text = "Enter VIN") },
                text = {
                    Column {
                        OutlinedTextField(
                            value = vinText,
                            onValueChange = {vinText = it},
                            label = { Text("VIN") },
                            singleLine = true
                        )
                    }
                },
                confirmButton = {
                    TextButton(
                        onClick = {
                            onSend(vinText)
                            onDismiss()
                        }
                    ) {
                        Text("Send")
                    }
                },
                dismissButton = {
                    TextButton(onClick = onDismiss) {
                        Text("Cancel")
                    }
                }
            )
        }
    }

    @SuppressLint("SetJavaScriptEnabled")
    @Composable
    fun ScrollableContent(onShowDialog: () -> Unit) {
        val configuration = LocalConfiguration.current
        val width = configuration.screenWidthDp
        val isWideScreen = width > 600

        Box (
            modifier = Modifier.fillMaxSize()
        ) {
            LazyColumn(
                modifier = Modifier.fillMaxSize(),
                contentPadding = PaddingValues(0.dp)
            ) {
                item {
                    Box( modifier = Modifier
                        .fillMaxSize()
                        .padding(0.dp)
                        .height(if (isWideScreen) 250.dp else 150.dp)
                    )
                    {
                        Image(
                            painter = painterResource(id = R.drawable.vw),
                            contentDescription = "Car Image",
                            modifier = Modifier
                                .fillMaxWidth(),
                            contentScale = ContentScale.Crop
                        )
                        TextButton(
                            onClick = { onShowDialog() },
                            modifier = Modifier
                                .align(Alignment.TopStart)
                                .padding(16.dp)
                                .background(
                                    Color.White.copy(alpha = 0.5f),
                                    shape = RoundedCornerShape(8.dp)
                                )
                        ) {
                            Icon(
                                Icons.Filled.Share,
                                contentDescription = "Connect with VIN",
                                tint = Color.Black
                            )
                            Spacer(modifier = Modifier.width(8.dp))
                            Text("Connect", color = Color.Black)

                        }
                    }
                }

                if (isWideScreen) {
                    item {
                        Box(
                            modifier = Modifier
                                .fillMaxSize()
                        ) {
                            Column(
                                modifier = Modifier.padding(16.dp)
                            ) {
                                Text("Automotive Over-The-Air", style = MaterialTheme.typography.headlineLarge.copy(fontSize = 48.sp))
                                Row {

                                    Image(
                                        painter = painterResource(id = R.drawable.ota),
                                        contentDescription = "Car Image",
                                        modifier = Modifier
                                            .size(150.dp)
                                            .padding(end = 8.dp)
                                    )
                                    // Add spacing between the image and the text
                                    Spacer(modifier = Modifier.width(16.dp))
                                    Text(
                                        "Automotive Over-The-Air (Automotive OTA) is changing the industry. In particular, applications such as software updates, live diagnostics and data collection already promise not only enormous savings potential but also enable many new opportunities to increase customer loyalty. Further applications will follow for sure. However, the challenges and efforts involved in introducing such Automotive OTA solutions are considerable. A good integration into existing processes is crucial for successful Automotive OTA projects.",
                                        modifier = Modifier
                                            .fillMaxWidth()
                                            .padding(16.dp),
                                        style = TextStyle(
                                            textAlign = TextAlign.Justify
                                        ),
                                        fontSize = 16.sp,
                                        lineHeight = 24.sp
                                    )
                                }
                            }
                        }
                    }
                    item {
                        Box(
                            modifier = Modifier
                                .fillMaxSize()
                                .padding(8.dp)
                                .background(Color.LightGray)
                        ) {
                            Column(
                                modifier = Modifier.padding(16.dp)
                            ) {
                                Text("Key Use Cases", style = MaterialTheme.typography.headlineLarge.copy(fontSize = 48.sp))
                                Row(
                                    modifier = Modifier.fillMaxWidth(),
                                    horizontalArrangement = Arrangement.SpaceBetween,
                                    verticalAlignment = Alignment.Top,
                                ) {
                                    Column(
                                        modifier = Modifier.weight(1f)
                                    ) {
                                        Text(
                                            "Today, most of the recall campaigns are software related. And the amount of software used in vehicles is still rising rapidly. Software updates over the air offer a tremendous potential for cost savings by avoiding these recalls. Functional and security issues can be fixed remotely. OTA software updates also provide the opportunity to implement new business models with for instance new features installed on demand, after the vehicle has been delivered.",
                                            modifier = Modifier
                                                .padding(end = 16.dp)
                                                .padding(16.dp),
                                            style = TextStyle(
                                                textAlign = TextAlign.Justify
                                            ),
                                            fontSize = 16.sp,
                                            lineHeight = 24.sp
                                        )
                                    }

                                    // Add spacing between the image and the text
                                    Spacer(modifier = Modifier.width(16.dp))

                                    Image(
                                        painter = painterResource(id = R.drawable.usecase),
                                        contentDescription = "Use case car",
                                        modifier = Modifier
                                            .size(200.dp)
                                            .padding(start = 8.dp)
                                    )

                                }
                            }
                        }
                    }
                } else {
                    item {
                        Box(
                            modifier = Modifier
                                .fillMaxSize()
                        ) {
                            Column(
                                modifier = Modifier.padding(16.dp)
                            ) {
                                Text("Automotive Over-The-Air", style = MaterialTheme.typography.headlineLarge.copy(fontSize = 48.sp))

                                Image(
                                    painter = painterResource(id = R.drawable.ota),
                                    contentDescription = "Car Image",
                                    modifier = Modifier
                                        .size(150.dp)
                                        .padding(end = 8.dp)
                                )
                                // Add spacing between the image and the text
                                Spacer(modifier = Modifier.width(16.dp))
                                Text(
                                    "Automotive Over-The-Air (Automotive OTA) is changing the industry. In particular, applications such as software updates, live diagnostics and data collection already promise not only enormous savings potential but also enable many new opportunities to increase customer loyalty. Further applications will follow for sure. However, the challenges and efforts involved in introducing such Automotive OTA solutions are considerable. A good integration into existing processes is crucial for successful Automotive OTA projects.",
                                    modifier = Modifier
                                        .fillMaxWidth()
                                        .padding(16.dp),
                                    style = TextStyle(
                                        textAlign = TextAlign.Justify
                                    ),
                                    fontSize = 16.sp,
                                    lineHeight = 24.sp
                                )

                            }
                        }
                    }
                    item {
                        Box(
                            modifier = Modifier
                                .fillMaxSize()
                                .padding(8.dp)
                                .background(Color.LightGray)
                        ) {
                            Column(
                                modifier = Modifier.padding(16.dp)
                            ) {
                                Text(
                                    "Key Use Cases", style = MaterialTheme.typography.headlineLarge.copy(fontSize = 48.sp),
                                    //modifier = Modifier.padding(16.dp)
                                )
                                Image(
                                    painter = painterResource(id = R.drawable.usecase),
                                    contentDescription = "Use case car",
                                    modifier = Modifier
                                        .size(200.dp)
                                        .padding(start = 8.dp)
                                )
                                // Add spacing between the image and the text
                                Spacer(modifier = Modifier.width(16.dp))
                                Text(
                                    "Today, most of the recall campaigns are software related. And the amount of software used in vehicles is still rising rapidly. Software updates over the air offer a tremendous potential for cost savings by avoiding these recalls. Functional and security issues can be fixed remotely. OTA software updates also provide the opportunity to implement new business models with for instance new features installed on demand, after the vehicle has been delivered.",
                                    modifier = Modifier
                                        .padding(16.dp),
                                    style = TextStyle(
                                        textAlign = TextAlign.Justify
                                    ),
                                    fontSize = 16.sp,
                                    lineHeight = 24.sp
                                )
                            }
                        }
                    }
                }
                item {
                    Box (modifier = Modifier
                        .fillMaxSize()
                        .padding(8.dp)
                        .background(Color.White)) {
                        Text(
                            text = "PoC 2024 ©",
                            style = TextStyle(color = Color.Gray, fontSize = 12.sp),
                            modifier = Modifier
                                .align(Alignment.BottomStart)
                                .padding(8.dp)
                        )
                    }
                }
            }
        }
    }
}

