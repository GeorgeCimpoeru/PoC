package com.poc.p_couds

import android.os.Bundle
import android.util.Log
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.Scaffold
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.Text
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.TextButton
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.navigation.NavController
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.poc.p_couds.ui.theme.CarsDataTheme
import com.google.accompanist.systemuicontroller.rememberSystemUiController
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.Icon
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AccountBox
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Person
import androidx.compose.ui.platform.ComposeView
import androidx.compose.ui.viewinterop.AndroidView
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentContainerView
import androidx.fragment.app.commit

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            CarsDataTheme {
                MainScreen()
            }
        }
    }

    @Composable
    fun ShowFragmentHome() {
        FragmentContainer(Modifier.fillMaxSize(), HomeFragment())
    }

    @Composable
    fun ShowFragmentLogin() {
        FragmentContainer(Modifier.fillMaxSize(), LogInFragment())
    }

    @Composable
    fun ShowFragmentSignup() {
        FragmentContainer(Modifier.fillMaxSize(), SignUpFragment())
    }

    @Composable
    fun FragmentContainer(modifier: Modifier, fragment: Fragment) {
        AndroidView(
            modifier = modifier,
            factory = { context ->
                FragmentContainerView(context).apply {
                    id = R.id.fragment_container_view // Make sure this ID is defined in ids.xml
                }
            },
            update = { fragmentContainerView ->
                supportFragmentManager.commit {
                    setReorderingAllowed(true)
                    setCustomAnimations(R.anim.fade_in_custom, R.anim.fade_out_custom)
                    replace(fragmentContainerView.id, fragment)
                }
            }
        )
    }

    @Composable
    fun AppNavigator(navController: NavHostController) {
        NavHost(
            navController = navController,
            startDestination = "home"
        ) {
            Log.d("main", "am intrat")
            composable("home") { ShowFragmentHome() }
            composable("login") { ShowFragmentLogin() }
            composable("signup") { ShowFragmentSignup() }
        }
    }

    @Composable
    fun TopAppBarWithMenu(navController: NavController) {
        TopAppBar(
            title = { Text("Drive Insight") },
            backgroundColor = Color.LightGray,
            actions = {
                Log.d("buton", "home")
                TextButton(onClick = { navController.navigate("home") }) {
                    Icon(Icons.Filled.Home, contentDescription = "Home", tint = Color.White)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text("Home", color = Color.White)
                }
                TextButton(onClick = { navController.navigate("login") }) {
                    Icon(Icons.Filled.Person, contentDescription = "Log in", tint = Color.White)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text("Log in", color = Color.White)
                }
                TextButton(onClick = { navController.navigate("signup") }) {
                    Icon(Icons.Filled.AccountBox, contentDescription = "Sign up", tint = Color.White)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text("Sign Up", color = Color.White)
                }
            })
    }

    @Composable
    fun MainScreen() {
        val navController = rememberNavController()
        val systemUiController = rememberSystemUiController()
        systemUiController.setSystemBarsColor(color = Color.Gray)  // Make system bars transparent

        Scaffold(
            topBar = {
                TopAppBarWithMenu(navController)
                Log.d("topbar", "sunt in navBar")
            },
            modifier = Modifier.systemBarsPadding()
        ){  paddingValues  ->
            Box(modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)) {
                AppNavigator(navController)
            }
        }

    }
}



