package com.poc.p_couds.fragments

import android.content.Context
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.Face
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextFieldDefaults

import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.ComposeView
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.text.input.VisualTransformation
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.poc.p_couds.R
import com.poc.p_couds.ui.theme.CarsDataTheme

class LogInFragment : Fragment() {
    private lateinit var sharedPreferences: SharedPreferences

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        sharedPreferences = requireActivity().getSharedPreferences("Preferences", Context.MODE_PRIVATE)
        // Inflate the layout for this fragment
        return ComposeView(requireContext()).apply {
            setContent {
                //FragmentContent()
                CarsDataTheme {
                LoginScreen {email, password ->
                    println("Email: $email, Password: $password")
                    //call login api in future implementation
                    //for testing purposes
                }
                }
            }
        }
    }

    // Function to simulate an API call
    private fun performLogin(email: String, password: String) {
        if (email == "miruna.stoisor@randstaddigital.com" && password == "password") {
            println("Login successful")
            val sharedPreferences = requireActivity().getSharedPreferences("Preferences", Context.MODE_PRIVATE)
            with(sharedPreferences.edit()) {
                putBoolean("isLoggedIn", true)
                putString("loggedInUser", email)
                apply()
            }
            Toast.makeText(requireContext(), "Login successful", Toast.LENGTH_SHORT).show()
            navigateToHomeFragment()
        } else {
            println("Invalid credentials")
            Toast.makeText(requireContext(), "Invalid credentials", Toast.LENGTH_SHORT).show()
        }
    }

    private fun navigateToHomeFragment() {
        val fragment = HomeFragment()
        val fragmentManager = requireActivity().supportFragmentManager
        fragmentManager.beginTransaction()
            .replace(R.id.fragment_container_view, fragment)
            .commit()
    }

    @Composable
    fun LoginScreen(onLogin: (String, String) -> Unit) {
        var email by remember { mutableStateOf("")}
        var password by remember { mutableStateOf("")}
        var isPasswordVisible by remember { mutableStateOf(false) }
        var isLoading by remember { mutableStateOf(false)}

        val configuration = LocalConfiguration.current
        val width = configuration.screenWidthDp
        val isWideScreen = width > 600
        Box(modifier = Modifier
            .fillMaxSize()
            .background(Color.White)
        ) {
            Image (
                painter = painterResource(id = R.drawable.porche),
                contentDescription = "porche_background",
                modifier = Modifier
                    .fillMaxSize()
                    .alpha(0.5f),
                contentScale = ContentScale.Crop
            )
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(16.dp),
                verticalArrangement  = Arrangement.SpaceBetween,
                horizontalAlignment  = Alignment.CenterHorizontally
            ) {
                // Sign-up form
                LazyColumn(
                    modifier = Modifier
                        .weight(1f) // Take available space
                        .padding(16.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    item {
                        Text(
                            "Login", style = MaterialTheme.typography.headlineMedium,
                            modifier = Modifier
                                .padding(bottom = 16.dp)
                                .align(Alignment.CenterHorizontally)
                        )
                    }

                    item {
                        OutlinedTextField(
                            value = email,
                            onValueChange = { email = it },
                            label = { Text("Email")
                            },
                            modifier = Modifier
                                .then(if (isWideScreen) Modifier.width(LocalConfiguration.current.screenWidthDp.dp / 3) else Modifier.fillMaxWidth())
                                .align(Alignment.CenterHorizontally)
                                .background(
                                    Color.White.copy(alpha = 0.7f),
                                    shape = MaterialTheme.shapes.extraSmall
                                ),
                            colors = TextFieldDefaults.colors(
                                focusedContainerColor = Color.Transparent, // Transparent background when focused
                                unfocusedContainerColor = Color.Transparent,
                                focusedIndicatorColor = Color.Transparent,
                                unfocusedIndicatorColor = Color.Transparent
                            )
                        )

                        Spacer(modifier = Modifier.height(8.dp))
                    }

                    item {
                        OutlinedTextField(
                            value = password,
                            onValueChange = { password = it },
                            label = { Text("Password") },
                            modifier = Modifier
                                .then(if (isWideScreen) Modifier.width(LocalConfiguration.current.screenWidthDp.dp / 3) else Modifier.fillMaxWidth())
                                .align(Alignment.CenterHorizontally)
                                .background(
                                    Color.White.copy(alpha = 0.7f),
                                    shape = MaterialTheme.shapes.small
                                ),
                            colors = TextFieldDefaults.colors(
                                focusedContainerColor = Color.Transparent, // Transparent background when focused
                                unfocusedContainerColor = Color.Transparent,
                                focusedIndicatorColor = Color.Transparent,
                                unfocusedIndicatorColor = Color.Transparent
                            ),
                            visualTransformation = if (isPasswordVisible) VisualTransformation.None else PasswordVisualTransformation(),
                            trailingIcon =  {
                                IconButton(onClick = { isPasswordVisible = !isPasswordVisible}) {
                                    Icon(
                                        imageVector = if (isPasswordVisible) Icons.Filled.Close else Icons.Filled.Face,
                                        contentDescription = if (isPasswordVisible) "Hide password" else "Show password"
                                    )

                                }
                            }
                        )
                        Spacer(modifier = Modifier.height(16.dp))
                    }

                    item {
                        Button(
                            onClick = {
                                isLoading = true
                                onLogin(email, password)
                                isLoading = false
                                performLogin(email, password)
                                Log.d("check", "click")
                            },
                            colors = ButtonDefaults.buttonColors(
                                containerColor = Color.DarkGray,
                                contentColor  = Color.White
                            ),
                            modifier = Modifier
                                .then(if (isWideScreen) Modifier.width(LocalConfiguration.current.screenWidthDp.dp / 3) else Modifier.fillMaxWidth())
                                .align(Alignment.CenterHorizontally),
                            enabled = email.isNotEmpty() && password.isNotEmpty() && !isLoading,
                        ) {
                            if (isLoading) {
                                CircularProgressIndicator(modifier = Modifier.size(24.dp))
                            } else {
                                Text("Login")
                            }
                        }
                    }
                }
                Text(
                    text = "PoC 2024 ©",
                    style = TextStyle(color = Color.Black, fontSize = 12.sp),
                    modifier = Modifier
                        .align(Alignment.Start)
                        .padding(8.dp)
                )
            }
        }
    }
}

