package com.poc.p_couds

import android.annotation.SuppressLint
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
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
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableIntState
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
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.navigation.compose.rememberNavController
import com.poc.p_couds.ui.theme.CarsDataTheme

class UDSactivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        setContent {
            CarsDataTheme {
                ActivityLayout()
            }
        }
    }
}

@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@Composable
fun ActivityLayout() {
    var selectedIndex = remember { mutableIntStateOf(0) }

    Scaffold(
        modifier = Modifier.fillMaxSize(),
        bottomBar = {
            selectedIndex = BottomAppBar(
                selectedIndex = selectedIndex.value,
                onButtonSelected = { index -> selectedIndex.value = index }
            )
        },
    ) {
        Column(
            modifier = Modifier.fillMaxSize()
        ) {
            TopBarNavigation()

            ThreeColumnLayout(
                modifier = Modifier
                    .weight(1f),
                selectedIndex = selectedIndex.value
            )
        }
    }
}

@Composable
fun ThreeColumnLayout(modifier: Modifier = Modifier, selectedIndex: Int) {
    val screenWidth = LocalConfiguration.current.screenWidthDp
    val columnWidth: Dp = (screenWidth * 0.27).dp
    var vectImg = ImageVector.vectorResource(id = R.drawable.car)
    when (selectedIndex) {
        1 -> vectImg = ImageVector.vectorResource(id = R.drawable.battery)
        2 -> vectImg = ImageVector.vectorResource(id = R.drawable.engine)
        3 -> vectImg = ImageVector.vectorResource(id = R.drawable.door)
        4 -> vectImg = ImageVector.vectorResource(id = R.drawable.hvac)
    }
    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color.Transparent)
    ) {
        GridBackground()

        Row(
            modifier = Modifier
                .fillMaxSize()
        ) {
            Column(
                modifier = Modifier
                    .width(columnWidth)
                    .fillMaxHeight(),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val1", "Param1")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val2", "Param2")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val3", "Param3")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val4", "Param4")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val5", "Param5")
            }
            Column(
                modifier = Modifier
                    .weight(1f)
                    .fillMaxHeight(),
                verticalArrangement = Arrangement.Center,
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
                    .fillMaxHeight(),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val6", "Param6")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val7", "Param7")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val8", "Param8")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val9", "Param9")
                ParamsButton(onClick = { /*TODO*/ }, Color.Blue,"Val10", "Param10")
            }
        }
    }
}

@Composable
fun ParamsButton(onClick: () -> Unit, buttonColor: Color, txtBtn: String, paramName: String) {
    Button(
        onClick = { /*TODO*/ },
        colors = ButtonDefaults.buttonColors(
            containerColor = buttonColor
        ),
        modifier = Modifier.padding(top = 10.dp)
    ) {
        Text(txtBtn, color = Color.White)
    }
    Text(paramName, color = Color.White)
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

fun handleMenuItemClick() {
    /*TODO OPEN NEW SEND REQUESTS FRAGMENT*/
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TopBarNavigation() {
    var showMenu by remember { mutableStateOf(false) }
    val navController = rememberNavController()

    TopAppBar(
        title = {
            Text(
                "UDS",
                modifier = Modifier.fillMaxWidth(),
                textAlign = TextAlign.Start
            )
        },
        navigationIcon = {
            IconButton(onClick = {navController.popBackStack()}) {
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
                    onClick = {}
                )
            }
        },
        colors = TopAppBarDefaults.topAppBarColors(
            containerColor = Color.LightGray
        )
    )
}

@Composable
fun BottomAppBar(selectedIndex: Int, onButtonSelected: (Int) -> Unit): MutableIntState {
    val selectedIndex = remember { mutableIntStateOf(selectedIndex) }

    androidx.compose.material3.BottomAppBar(
        containerColor = MaterialTheme.colorScheme.primaryContainer,
        contentColor = MaterialTheme.colorScheme.primary,
    ) {
        val buttonLabels = listOf("MCU", "Battery", "Engine", "Doors", "HVAC")

        buttonLabels.forEachIndexed { index, label ->
            CustomButtonForBottomAppBar(
                isSelected = selectedIndex.value == index,
                onClick = {
                    selectedIndex.value = index
                },
                ecuName = label
            )
        }
    }
    return selectedIndex
}

@Composable
fun CustomButtonForBottomAppBar(isSelected: Boolean, onClick: () -> Unit, ecuName: String) {
    val buttonColor = if (isSelected) Color.Gray else Color.Transparent

    Box(
        modifier = Modifier
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


@Preview(showBackground = true)
@Composable
fun ActivPreview() {
    CarsDataTheme {
        ActivityLayout()
    }
}