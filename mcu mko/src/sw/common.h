#ifndef __COMMON_H
    #define __COMMON_H

    #define MCU_CLOCK 20000000UL

    // Enable debug options (e.g. printf)
    //#define _DEBUG

    // SpaceWire receiver buffer size
    #define _SpW_BUFFER_SIZE     256

    // SpaceWire target configuration
    #define _SpW_ADDRESS         0x13
    #define _SpW_KEY             0x04

    // Allowed area for reading or writing by RMAP
    #define _RMAP_BOUND_L {0x00, 0x00, 0x00, 0x08, 0x00}
    #define _RMAP_BOUND_H {0x00, 0x00, 0x00, 0x17, 0xFF}

#endif
