#include "../include/ecu_reset.h"
#include "ecu_reset.h"

void EcuReset::ecuReset(uint8_t reset_form)
{
    /* Hard Reset case */
    if (reset_form == 0x1) {
        /* THe logic for hard reset on interface.
        Need a way to access object up in the hierarchy */
    } else if (reset_form == 0x2) {
         /* The logic for key off reset on interface.
        Need a way to access object up in te hierarchy */
    }
}
void EcuReset::hardReset()
{
}

void EcuReset::keyOffReset()
{
}
