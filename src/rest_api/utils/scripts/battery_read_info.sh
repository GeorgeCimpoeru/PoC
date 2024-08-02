cansend vcan1 2FA#025001
sleep 1
cansend vcan1 2FA#0469010102
sleep 1
cansend vcan1 2FA#026902
sleep 1

# Battery energy level (0-100%)
cansend vcan1 2FA#0462016401 # Example: 100%

sleep 1

# Battery voltage (in tenths of volts, e.g., 1234 = 123.4V)
cansend vcan1 2FA#046201A00A # Example: 10.0V

sleep 1

# Battery state of charge (0-100%)
cansend vcan1 2FA#046201D050 # Example: 80%

sleep 1

# Battery temperature (in tenths of degrees Celsius, e.g., 250 = 25.0°C)
cansend vcan1 2FA#046201E190 # Example: 25.0°C

sleep 1

# Battery life cycle (e.g., 500 cycles)
cansend vcan1 2FA#046201F1F4 # Example: 500 cycles

sleep 1

# Battery fully charged (1 for true, 0 for false)
cansend vcan1 2FA#046202A001 # Example: Fully charged

sleep 1

# ECU Serial number (example value, usually a fixed identifier)
cansend vcan1 2FA#0462F12F07 # Example: 12345

sleep 1

# Battery range (in kilometers, e.g., 250km)
cansend vcan1 2FA#046202B0FA # Example: 250km

sleep 1

# Battery charging time (in minutes, e.g., 120 minutes)
cansend vcan1 2FA#046202C078 # Example: 120 minutes

sleep 1

# Battery consumption (in tenths of kWh, e.g., 150 = 15.0kWh)
cansend vcan1 2FA#046202D096 # Example: 15.0kWh

sleep 1
