package com.poc.p_couds.activities

import LoadingIndicator
import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.content.res.Configuration
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.compose.ui.window.DialogProperties
import androidx.core.content.ContextCompat.startActivity
import com.poc.p_couds.models.EcusInfoViewModel
import com.poc.p_couds.R
import com.poc.p_couds.ui.theme.CarsDataTheme

class UDSactivity : ComponentActivity() {
    private val ecusInfoViewModel: EcusInfoViewModel by viewModels()
//    val context = LocalContext.current

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        setContent {
            CarsDataTheme{
                ActivityLayout(ecusInfoViewModel, onBackPressed = { onBackPressed() }, this.baseContext)
            }
        }
    }
}

@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@Composable
fun ActivityLayout(ecusInfoViewModel: EcusInfoViewModel, onBackPressed: () -> Unit, context: Context) {
    val selectedIndex = remember { mutableIntStateOf(0) }
    val context = LocalContext.current

    Scaffold(
        modifier = Modifier.fillMaxSize(),
        bottomBar = {
            BottomAppBar(
                selectedIndex = selectedIndex.intValue,
                onButtonSelected = { index -> selectedIndex.intValue = index }
            )
        },
    ) {
        Column(
            modifier = Modifier.fillMaxSize()
        ) {
            TopBarNavigation(onBackPressed, context)
            ThreeColumnLayout(ecusInfoViewModel, modifier = Modifier.weight(1f), selectedIndex = selectedIndex.intValue, context)
        }
    }
}

@Composable
fun BottomAppBar(selectedIndex: Int, onButtonSelected: (Int) -> Unit) {
    androidx.compose.material3.BottomAppBar(
        containerColor = MaterialTheme.colorScheme.primaryContainer,
        contentColor = MaterialTheme.colorScheme.primary,
    ) {
        val buttonLabels = listOf("MCU", "Battery", "Engine", "Doors", "HVAC")

        buttonLabels.forEachIndexed { index, label ->
            CustomButtonForBottomAppBar(
                modifier = Modifier.weight(1f),
                isSelected = selectedIndex == index,
                onClick = {
                    onButtonSelected(index)
                },
                ecuName = label
            )
        }
    }
}

@Composable
fun CustomButtonForBottomAppBar(modifier: Modifier = Modifier, isSelected: Boolean, onClick: () -> Unit, ecuName: String) {
    val buttonColor = if (isSelected) Color.Gray else Color.Transparent
    Box(
        modifier = modifier
            .padding(start = 2.dp)
            .clickable(onClick = onClick)
            .background(buttonColor, shape = RoundedCornerShape(12.dp))
            .size(79.dp)
    ) {
        Column(
            verticalArrangement = Arrangement.Bottom,
            horizontalAlignment = Alignment.CenterHorizontally,
            modifier = Modifier.fillMaxSize()
        ) {
            Icon(
                imageVector = ImageVector.vectorResource(id = R.drawable.ecu),
                contentDescription = "Custom Icon",
                modifier = Modifier
                    .size(57.dp)
                    .padding(0.dp)
            )
            Text(
                text = ecuName,
                style = TextStyle(fontSize = 12.sp),
                modifier = Modifier.padding(top = 0.dp)
            )
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TopBarNavigation(onBackPressed: () -> Unit, context: Context) {
    var showMenu by remember { mutableStateOf(false) }
//    val context = LocalContext.current

    TopAppBar(
        title = {
            Text(
                "UDS",
                modifier = Modifier.fillMaxWidth(),
                textAlign = TextAlign.Start
            )
        },
        navigationIcon = {
            IconButton(onClick = {
                onBackPressed()
            }) {
                Icon(
                    imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                    contentDescription = "Localized description"
                )
            }
        },
        actions = {
            IconButton(onClick = { showMenu = true }) {
                Icon(
                    imageVector = Icons.Default.MoreVert, // 3 dots icon
                    contentDescription = "More options"
                )
            }
            DropdownMenu(
                expanded = showMenu,
                onDismissRequest = { showMenu = false }
            ) {
                DropdownMenuItem(
                    text = { Text(text = "Send requests") },
                    onClick = {
                        val intent = Intent(context, RequestActivity::class.java)
                        context.startActivity(intent)
                    }
                )
            }
        },
        colors = TopAppBarDefaults.topAppBarColors(
            containerColor = Color.LightGray
        )
    )
}

@Composable
fun ThreeColumnLayout(ecusInfoViewModel: EcusInfoViewModel, modifier: Modifier = Modifier, selectedIndex: Int, context: Context) {
    val screenWidth = LocalConfiguration.current.screenWidthDp
    val orientation = LocalConfiguration.current.orientation
    val columnWidth: Dp = (screenWidth * 0.27).dp
    var vectImg: ImageVector = ImageVector.vectorResource(id = R.drawable.car)
    val isLoading by ecusInfoViewModel.loading.collectAsState()
    val scrollState = rememberScrollState()
    when (selectedIndex) {
        0 -> { vectImg = ImageVector.vectorResource(id = R.drawable.car) }
        1 -> { vectImg = ImageVector.vectorResource(id = R.drawable.battery) }
        2 -> { vectImg = ImageVector.vectorResource(id = R.drawable.engine) }
        3 -> { vectImg = ImageVector.vectorResource(id = R.drawable.door) }
        4 -> { vectImg = ImageVector.vectorResource(id = R.drawable.hvac) }
    }
    LaunchedEffect(selectedIndex) {
        when (selectedIndex) {
            0 -> { /*TODO*/ }
            1 -> {
                ecusInfoViewModel.fetchBatteryInfo(context)
                ecusInfoViewModel.readBatteryDTC(context)
            }
            2 -> { ecusInfoViewModel.fetchEngineInfo(context) }
            3 -> { ecusInfoViewModel.fetchDoorsInfo(context) }
            4 -> { ecusInfoViewModel.fetchHvacInfo(context) }
        }
    }
    Box(modifier = modifier.fillMaxSize().background(Color.Transparent)) {
        GridBackground()
        if (isLoading) {
            LoadingIndicator()
        }
        Column(modifier = Modifier.fillMaxSize().let { if (orientation == Configuration.ORIENTATION_LANDSCAPE) it.verticalScroll(scrollState) else it }) {
            Row(modifier = Modifier.fillMaxHeight(0.065f)
                .fillMaxWidth(1f),
                horizontalArrangement = Arrangement.Center
            ) { DTCbuttons(ecusInfoViewModel.batteryDTC) }
            Row(modifier = Modifier.fillMaxSize()) {
                Column(
                    modifier = Modifier
                        .width(columnWidth)
                        .fillMaxHeight()
                        .padding(bottom = 120.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    when (selectedIndex) {
                        0 -> {
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val1", "Param1", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val2", "Param2", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val3", "Param3", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val4", "Param4", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val5", "Param5", "MCU")
                        }
                        1 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.batteryInfo?.battery_level}",
                                "Battery level",
                                "Battery"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.batteryInfo?.battery_state_of_charge}",
                                "State of charge",
                                "Battery"
                            )
                        }
                        2 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.coolant_temperature}",
                                "Coolant temp",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.engine_load}",
                                "Load",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.engine_rpm}",
                                "Rpm",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.fuel_level}",
                                "Fuel",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.fuel_pressure}",
                                "Fuel pressure",
                                "Engine"
                            )
                        }
                        3 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.doorsInfo?.ajar}",
                                "Ajar",
                                "Doors"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.doorsInfo?.door}",
                                "Door",
                                "Doors"
                            )
                        }
                        4 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.ambient_air_temperature}",
                                "Ambient temp",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.cabin_temperature}",
                                "Cabin temp",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.cabin_temperature_driver_set}",
                                "Driver set temp",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.fan_speed}",
                                "Fan speed",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.hvac_modes?.AC_Status}",
                                "AC status",
                                "HVAC"
                            )
                        }
                    }
                }
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .fillMaxWidth()
                        .padding(top = 100.dp),
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = vectImg,
                        contentDescription = null,
                        tint = Color.White,
                        modifier = Modifier.size(350.dp)
                    )
                }

                Column(
                    modifier = Modifier
                        .width(columnWidth)
                        .fillMaxHeight()
                        .padding(bottom = 120.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    when (selectedIndex) {
                        0 -> {
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val6", "Param6", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val7", "Param7", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val8", "Param8", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val9", "Param9", "MCU")
                            ParamsButton(context, ecusInfoViewModel, Color.Blue, "Val10", "Param10", "MCU")
                        }

                        1 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.batteryInfo?.percentage}",
                                "Percentage",
                                "Battery"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.batteryInfo?.voltage}",
                                "Voltage",
                                "Battery"
                            )
                        }

                        2 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.intake_air_temperature}",
                                "Intake air temp",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.oil_temperature}",
                                "Oil temp",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.throttle_position}",
                                "Throttle position",
                                "Engine"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.engineInfo?.vehicle_speed}",
                                "Speed",
                                "Engine"
                            )
                        }

                        3 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.doorsInfo?.passenger}",
                                "Passanger",
                                "Doors"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.doorsInfo?.passenger_lock}",
                                "Passanger lock",
                                "Doors"
                            )
                        }

                        4 -> {
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.hvac_modes?.Air_Recirculation}",
                                "Air recirc",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.hvac_modes?.Defrost}",
                                "Defrost",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.hvac_modes?.Front}",
                                "Front",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.hvac_modes?.Legs}",
                                "Legs",
                                "HVAC"
                            )
                            ParamsButton(context,
                                ecusInfoViewModel,
                                Color.Blue,
                                "${ecusInfoViewModel.hvacInfo?.mass_air_flow}",
                                "Mass air flow",
                                "HVAC"
                            )
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun GridBackground(
    modifier: Modifier = Modifier,
    color: Color = Color(0xFF0A0B45),
    lineColor: Color = Color(0xFF2B2D62),
    gridSize: Dp = 32.dp
) {
    Canvas(modifier = modifier.fillMaxSize()) {
        drawRect(color = color)

        val columns = (size.width / gridSize.toPx()).toInt()
        val rows = (size.height / gridSize.toPx()).toInt()

        for (i in 0..columns) {
            val x = i * gridSize.toPx()
            drawLine(
                color = lineColor,
                start = Offset(x, 0f),
                end = Offset(x, size.height),
                strokeWidth = 1.dp.toPx()
            )
        }

        for (i in 0..rows) {
            val y = i * gridSize.toPx()
            drawLine(
                color = lineColor,
                start = Offset(0f, y),
                end = Offset(size.width, y),
                strokeWidth = 1.dp.toPx()
            )
        }
    }
}

@Composable
fun ParamsButton(context: Context, ecusInfoViewModel: EcusInfoViewModel, buttonColor: Color, txtBtn: String, paramName: String, selectedEcu: String) {
    var showParamsDialog by remember { mutableStateOf(false) }
    var inputText by remember { mutableStateOf(txtBtn) }
    Button(
        onClick = { showParamsDialog = true },
        colors = ButtonDefaults.buttonColors(
            containerColor = buttonColor
        ),
        modifier = Modifier.padding(top = 10.dp)
    ) {
        Text(txtBtn, color = Color.White)
    }
    Text(paramName, color = Color.White)

    if (showParamsDialog) {
        DialogParams(context,
            ecusInfoViewModel,
            title = paramName,
            onDismiss = { showParamsDialog = false },
            inputText = inputText,
            onInputChange = { inputText = it },
            selectedEcu
        )
    }
}

@Composable
fun DialogParams(
    context: Context,
    ecusInfoViewModel: EcusInfoViewModel,
    title: String,
    onDismiss: () -> Unit,
    inputText: String,
    onInputChange: (String) -> Unit,
    selectedEcu: String
) {
    Dialog(onDismissRequest = onDismiss, properties = DialogProperties()) {
        Surface(
            shape = MaterialTheme.shapes.medium,
            color = MaterialTheme.colorScheme.background
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(text = title, style = MaterialTheme.typography.headlineSmall)

                Spacer(modifier = Modifier.padding(8.dp))

                BasicTextField(
                    value = inputText,
                    onValueChange = onInputChange,
                    modifier = Modifier
                        .padding(8.dp)
                )

                Spacer(modifier = Modifier.padding(8.dp))

                Row {
                    Button(onClick = onDismiss) {
                        Text("Cancel")
                    }
                    Spacer(modifier = Modifier.padding(8.dp))
                    Button(onClick = {
                        when (selectedEcu) {
                            "MCU" -> {/*TODO*/ }
                            "Battery" -> { ecusInfoViewModel.writeBatteryInfo(context, inputText, title) }
                            "Engine" -> { ecusInfoViewModel.writeEngineInfo(context, inputText, title) }
                            "Doors" -> { ecusInfoViewModel.writeDoorsInfo(context, inputText, title) }
                            "HVAC" -> { ecusInfoViewModel.writeHVACInfo(context, inputText, title) }
                        }
                        onDismiss()
                    }) {
                        Text(text = "Save")
                    }
                }
            }
        }
    }
}

@Composable
fun DTCbuttons(batteryDTC: Any?) {
    var showReadDTCdialog by remember { mutableStateOf(false) }
    Button(
        onClick = { showReadDTCdialog = true },
        colors = ButtonDefaults.buttonColors(
            containerColor = Color(0xFFFFA500)
        ),
        border = BorderStroke(2.dp, Color.Red),
        elevation = ButtonDefaults.buttonElevation(8.dp),
        shape = RoundedCornerShape(8.dp),
        modifier = Modifier
            .padding(8.dp)
    ) {
        Text(
            text = "! Read DTC !",
            color = Color.White,
            fontWeight = FontWeight.Bold
        )
    }
    Button(
        onClick = {  },
        colors = ButtonDefaults.buttonColors(
            containerColor = Color(0xFF4CAF50)
        ),
        border = BorderStroke(2.dp, Color(0xFF388E3C)),
        elevation = ButtonDefaults.buttonElevation(8.dp),
        modifier = Modifier
            .padding(8.dp)
    ) {
        Text(
            text = "Clear DTC",
            color = Color.White,
            fontWeight = FontWeight.Bold
        )
    }
    if (showReadDTCdialog) {
        DialogReadDTC(onDismiss = { showReadDTCdialog = false }, batteryDTC)
    }
}

@Composable
fun DialogReadDTC(
    onDismiss: () -> Unit,
    batteryDTC: Any?
) {
    Dialog(onDismissRequest = onDismiss, properties = DialogProperties()) {
        Surface(
            shape = MaterialTheme.shapes.medium,
            color = MaterialTheme.colorScheme.background,
            modifier = Modifier
                .fillMaxWidth(1.0f)
                .fillMaxHeight(0.8f)
        ) {
            Text(
                modifier = Modifier.padding(4.dp),
                text = batteryDTC.toString(),
                color = Color.Black,
                fontWeight = FontWeight.Bold
            )
        }
    }
}

@Preview(showBackground = true)
@Composable
fun ActivityPreview() {
    val ecusInfoViewModel = EcusInfoViewModel()
    val context = LocalContext.current
    CarsDataTheme{
        ActivityLayout(ecusInfoViewModel,
            onBackPressed = {  },
            context = context
        )
    }
}