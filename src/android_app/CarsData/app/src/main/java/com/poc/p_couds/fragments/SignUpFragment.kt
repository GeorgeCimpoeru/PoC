package com.poc.p_couds.fragments

import android.os.Bundle
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
import androidx.navigation.NavController
import androidx.navigation.NavHostController
import androidx.navigation.compose.rememberNavController
import androidx.navigation.findNavController
import com.poc.p_couds.R
import com.poc.p_couds.models.DbSignInUp
import com.poc.p_couds.ui.theme.CarsDataTheme

class SignUpFragment(private val navController: NavHostController) : Fragment() {

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        // Inflate the layout for this fragment
        return ComposeView(requireContext()).apply {
            setContent {
                //FragmentContent()
                CarsDataTheme {
                    val dbSignInUp = DbSignInUp(requireContext()) // Replace with your database helper instance
                    SignUpScreen (
                        navController = navController,
                        dbSignInUp = dbSignInUp,
                        onSignup = { email, password, _ ->
                            println("Email: $email, Password: $password")
                        }
                    )
                }
            }
        }
    }

    @Composable
    fun SignUpScreen(navController: NavController, dbSignInUp: DbSignInUp, onSignup: (String, String, String) -> Unit) {
        var email by remember { mutableStateOf("")}
        var password by remember { mutableStateOf("")}
        var cpassword by remember { mutableStateOf("")}
        var isLoading by remember { mutableStateOf(false)}
        var passwordsMatch by remember { mutableStateOf(true) }
        var isPasswordVisible by remember { mutableStateOf(false) }

        val configuration = LocalConfiguration.current
        val width = configuration.screenWidthDp
        val isWideScreen = width > 600
        Box(modifier = Modifier
            .fillMaxSize()
            .background(Color.White)
        ) {
            Image (
                painter = painterResource(id = R.drawable.register),
                contentDescription = "car_background",
                modifier = Modifier
                    .fillMaxSize()
                    .alpha(0.5f),
                contentScale = ContentScale.Crop
            )
            Column (
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
                    horizontalAlignment  = Alignment.CenterHorizontally
                ) {
                    item {
                        Text(
                            "Signup", style = MaterialTheme.typography.headlineMedium,
                            modifier = Modifier
                                .padding(bottom = 16.dp)
                                .align(Alignment.CenterHorizontally)
                        )
                    }

                    item {
                        OutlinedTextField(
                            value = email,
                            onValueChange = { email = it },
                            label = { Text("Email") },
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
                                    shape = MaterialTheme.shapes.extraSmall
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
                        Spacer(modifier = Modifier.height(8.dp))
                    }

                    item {
                        OutlinedTextField(
                            value = cpassword,
                            onValueChange = { cpassword = it },
                            label = { Text("Confirm password") },
                            isError = !passwordsMatch,
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

                        // Show error message if passwords do not match
                        if (!passwordsMatch) {
                            Text("Passwords do not match", color = MaterialTheme.colorScheme.error)
                        }
                    }

                    item {
                        Button(
                            onClick = {
                                isLoading = true
                                passwordsMatch = (password == cpassword)
                                if (passwordsMatch) {
                                    onSignup(email, password, cpassword)
                                    performSignup(email, password, dbSignInUp, navController)
                                } else {
                                    isLoading = false
                                }
                            },
                            colors = ButtonDefaults.buttonColors(
                                containerColor = Color.DarkGray,
                                contentColor  = Color.White
                            ),
                            modifier = Modifier
                                .then(if (isWideScreen) Modifier.width(LocalConfiguration.current.screenWidthDp.dp / 3) else Modifier.fillMaxWidth())
                                .align(Alignment.CenterHorizontally),
                            enabled = email.isNotEmpty() && password.isNotEmpty() && cpassword.isNotEmpty() && !isLoading
                        ) {
                            if (isLoading) {
                                CircularProgressIndicator(modifier = Modifier.size(24.dp))
                            } else {
                                Text("Signup")
                            }
                        }
                    }
                }
                Text(
                    text = "PoC 2024 ©",
                    style = TextStyle(color = Color.Black, fontSize = 12.sp),
                    modifier = Modifier
                        .padding(8.dp)
                        .align(Alignment.Start)
                )
            }
        }
    }

    // Function to simulate an API call
    private fun performSignup(email: String, password: String, dbSignInUp: DbSignInUp, navController: NavController) {
        if (dbSignInUp.addNewUser(email, password)) {
            Toast.makeText(context, "Email already exists.", Toast.LENGTH_SHORT).show()
            navController.navigate("signup")
        } else {
            Toast.makeText(context, "Sign-up successful!", Toast.LENGTH_SHORT).show()
            navController.navigate("login")
        }
    }

}

