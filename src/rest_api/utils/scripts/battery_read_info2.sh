cansend vcan1 2FA#025001
sleep 1
cansend vcan1 2FA#0469010102
sleep 1
cansend vcan1 2FA#026902
sleep 1

# Battery energy level (75% -> 0x4B)
cansend vcan1 2FA#0462014B00 # 75%

sleep 1

# Battery voltage (120.0V -> 0x04B0)
cansend vcan1 2FA#046201A004B0 # 120.0V

sleep 1

# Battery state of charge (70% -> 0x46)
cansend vcan1 2FA#046201D04600 # 70%

sleep 1

# Battery temperature (30.0°C -> 0x012C)
cansend vcan1 2FA#046201E0012C # 30.0°C

sleep 1

# Battery life cycle (350 cycles -> 0x015E)
cansend vcan1 2FA#046201F0015E # 350 cycles

sleep 1

# Battery fully charged (False -> 0x00)
cansend vcan1 2FA#046202A00000 # Not fully charged

sleep 1

# ECU Serial number (Example: 23456 -> 0x05BA0)
cansend vcan1 2FA#0462F1005BA0 # Example serial number

sleep 1

# Battery range (300km -> 0x012C)
cansend vcan1 2FA#046202B0012C # 300km

sleep 1

# Battery charging time (90 minutes -> 0x005A)
cansend vcan1 2FA#046202C0005A # 90 minutes

sleep 1

# Battery consumption (18.0kWh -> 0x00B4)
cansend vcan1 2FA#046202D000B4 # 18.0kWh

sleep 1
