package com.poc.p_couds

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
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
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.shape.RoundedCornerShape
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.AlertDialog
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.Icon
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.MaterialTheme
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AccountBox
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Person
import androidx.compose.material.icons.filled.Share
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.sp
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.viewinterop.AndroidView

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            CarsDataTheme {
                MainScreen()
            }
        }
    }
}

@Composable
fun HomeScreen( paddingValues: PaddingValues, onShowDialog: () -> Unit) {
    // State for the dialog
    var showDialog by remember { mutableStateOf(false) }
    var vinText by remember { mutableStateOf("") }
    Column(modifier = Modifier.fillMaxSize()) {
        MainContent(paddingValues, onShowDialog = onShowDialog)
    }

    // Show dialog when `showDialog` is true
    VinDialog(
        showDialog = showDialog,
        onDismiss = { showDialog = false },
        onSend = { vin ->
            vinText = vin
            // Handle VIN text (e.g., send it to a server)
            showDialog = false
        }
    )
}

@Composable
fun LoginScreen() {
    Text("Log in Screen")
}

@Composable
fun SignupScreen() {
    Text("Sign up Screen")
}

@Composable
fun AppNavigator(navController: NavHostController, paddingValues: PaddingValues) {
    NavHost(
        navController = navController,
        startDestination = "home"
    ) {
        composable("home") { HomeScreen(paddingValues = paddingValues, onShowDialog = {} ) }
        composable("login") { LoginScreen() }
        composable("signup") { SignupScreen() }
    }
}

@Composable
fun TopAppBarWithMenu(navController: NavController) {
    TopAppBar(
        title = { Text("Drive Insight") },
        backgroundColor = Color.LightGray,
        actions = {
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
    systemUiController.setSystemBarsColor(color = Color.Blue)  // Make system bars transparent

    Scaffold(
        topBar = {
            TopAppBarWithMenu(navController)
        },
        modifier = Modifier.systemBarsPadding()
    ){  paddingValues  ->
        Box(modifier = Modifier
            .fillMaxSize()
            .padding(paddingValues)) {
            AppNavigator(navController, paddingValues)
        }
    }

}

@Composable
fun MainContent(paddingValues: PaddingValues,
                onShowDialog: () -> Unit) {
    Column(modifier = Modifier
        .fillMaxSize()
        .padding(paddingValues)) {
        ScrollableContent(onShowDialog = onShowDialog)
    }
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
            title = {Text(text = "Enter VIN")},
            text = {
                Column {
                    OutlinedTextField(
                        value = vinText,
                        onValueChange = {vinText = it},
                        label = {Text("VIN")},
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

        LazyColumn(
            modifier = Modifier.fillMaxSize(),
            contentPadding = PaddingValues(0.dp)
        ) {
            item {
                Box(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(0.dp)
                        .height(200.dp)
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
            if (width > 600) {
                item {
                    Box(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(8.dp)
                    ) {
                        Column(
                            modifier = Modifier.padding(16.dp)
                        ) {
                            Text("Automotive Over-The-Air", style = MaterialTheme.typography.h2)
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
                            Text("Key Use Cases", style = MaterialTheme.typography.h2)
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
                            Text("Automotive Over-The-Air", style = MaterialTheme.typography.h2)

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
                                "Key Use Cases", style = MaterialTheme.typography.h2,
                                modifier = Modifier.padding(16.dp)
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
                Box (
                    modifier = Modifier.height(300.dp)
                ) {
                    Text("Sunt si eu aici")
                    AndroidView(
                        factory = { context ->
                        WebView(context).apply {
                            webViewClient = WebViewClient()
                            settings.javaScriptEnabled = true
                            loadUrl("https://www.google.com/maps/embed?pb=!1m14!1m8!1m3!1d12080.73732861526!2d-74.0059418!3d40.7127847!3m2!1i1024!2i768!4f13.1!3m3!1m2!1s0x0%3A0x0!2zM40zMDA2JzEwLjAiTiA3NMKwMjUnMzcuNyJX!5e0!3m2!1sen!2sus!4v1648482801994!5m2!1sen!2sus")
                            Log.d("debug", "am ajuns aici")
                            }
                        },
                        update = { webView ->
                            webView.loadUrl("https://www.google.com/maps/embed?pb=!1m14!1m8!1m3!1d12080.73732861526!2d-74.0059418!3d40.7127847!3m2!1i1024!2i768!4f13.1!3m3!1m2!1s0x0%3A0x0!2zM40zMDA2JzEwLjAiTiA3NMKwMjUnMzcuNyJX!5e0!3m2!1sen!2sus!4v1648482801994!5m2!1sen!2sus")
                        })
                }
            }
        }
    }