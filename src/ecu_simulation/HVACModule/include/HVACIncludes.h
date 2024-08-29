#ifndef HVACINCLUDES_H
#define HVACINCLUDES_H

#define HVAC_ECU_ID 0x14

#define AMBIENT_TEMPERATURE_DID   0x0140
#define CABIN_TEMPERATURE_DID     0x0150
#define HVAC_SET_TEMPERATURE_DID  0x01C0
#define FAN_SPEED_DID             0x01D0
#define HVAC_MODES_DID            0x01F0

#define AC_STATUS           (1 << 0)
#define LEGS                (1 << 1)
#define FRONT               (1 << 2)
#define AIR_RECIRCULATION   (1 << 3)
#define DEFROST             (1 << 4)
#define RESERVED_B5         (1 << 5)
#define RESERVED_B6         (1 << 6)
#define RESERVED_B7         (1 << 7)

#define HVAC_MAX_AMBIENT_TEMPERATURE 50
#define HVAC_MIN_AMBIENT_TEMPERATURE 0
#define HVAC_AMBIENT_TEMPERATURE_MOD ((HVAC_MAX_AMBIENT_TEMPERATURE - HVAC_MIN_AMBIENT_TEMPERATURE) + 1)

#define HVAC_MAX_CABIN_TEMPERATURE 30
#define HVAC_MIN_CABIN_TEMPERATURE 0
#define HVAC_CABIN_TEMPERATURE_MOD ((HVAC_MAX_CABIN_TEMPERATURE - HVAC_MIN_CABIN_TEMPERATURE) + 1)

#define HVAC_MAX_SET_TEMPERATURE 25
#define HVAC_MIN_SET_TEMPERATURE 16
#define HVAC_SET_TEMPERATURE_MOD ((HVAC_MAX_SET_TEMPERATURE - HVAC_MIN_SET_TEMPERATURE) + 1)

#define HVAC_MAX_FAN_SPEED 100
#define HVAC_MIN_FAN_SPEED 0
#define HVAC_MAX_FAN_SPEED_LEVELS 5
#define HVAC_FAN_SPEED_MOD ((HVAC_MAX_FAN_SPEED - HVAC_MIN_FAN_SPEED) + 1)

#define HVAC_MODES_MOD 256

#endif /* HVACINCLUDES_H */