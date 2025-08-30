/************************************************
 Järjestelmä määritykset tänne
*************************************************/
#pragma once

#include <config/config.h>

#ifdef MODULE_MB
    #include <mb/mb.h>
#endif

uint32_t get_board_mac(uint8_t mac[6])
{
    #if defined(MODULE_MB)
        // Kysytään tieto mailboxilta
        return mb_get_mac(mac);
    #endif

    // Palautetaan oletuksena 0
    return 0;
}

uint32_t get_board_sn(uint32_t sn[2])
{
    #ifdef MODULE_MB
        // Kysytään tieto mailboxilta
        return mb_get_sn(sn);
    #endif

    // Palautetaan oletuksena 0
    return 0;
}
