#ifndef CC1101_SETTINGS_H
#define CC1101_SETTINGS_H

#include "cc1101.h"

/* =========================================================
 * MODE 1: Raw ASK/OOK RX — 433.92 MHz Remote Controls
 *
 * XTAL      : 26 MHz
 * Carrier   : 433.919830 MHz
 * Data rate : 4.79794 kBaud
 * RX BW     : 812.5 kHz (maximum)
 * Output    : GDO0 asynchronous serial data
 * ========================================================= */

static const registerSetting_t CC1101_Settings_RX_ASK[] =
{
  /* GDO0 = asynchronous serial data output */
  {CC1101_IOCFG0,      0x0D},

  /*
    * Async serial mode.
    * TI DN119 explicitly uses 0x32 for asynchronous operation.
    */
  {CC1101_PKTCTRL0,    0x32},

  /*
    * 812.5 kHz RX bandwidth.
    * IF = 14 * 26 MHz / 1024 = 355.47 kHz.
    */
  {CC1101_FSCTRL1,     0x0E},
  {CC1101_FSCTRL0,     0x00},

  /* 433.919830 MHz with 26 MHz crystal */
  {CC1101_FREQ2,       0x10},
  {CC1101_FREQ1,       0xB0},
  {CC1101_FREQ0,       0x71},

  /*
    * CHANBW = 812.5 kHz
    * Data rate = 4.79794 kBaud
    */
  {CC1101_MDMCFG4,     0x07},
  {CC1101_MDMCFG3,     0x83},

  /*
    * ASK/OOK
    * DC blocking filter ENABLED
    * Manchester OFF
    * No sync word
    */
  {CC1101_MDMCFG2,     0x30},

  /*
    * Automatic frequency synthesizer calibration
    * when going IDLE -> RX.
    */
  {CC1101_MCSM0,       0x18},

  /*
    * OOK AGC optimized for very wide RX bandwidth.
    *
    * 0xC7:
    *   MAX_DVGA_GAIN = 3
    *   MAX_LNA_GAIN  = 0
    *   MAGN_TARGET   = 42 dB
    *
    * 0xB2:
    *   medium hysteresis
    *   32-sample AGC wait
    *   12 dB OOK decision boundary
    */
  {CC1101_AGCCTRL2,    0xC7},
  {CC1101_AGCCTRL1,    0x00},
  {CC1101_AGCCTRL0,    0xB2},

  /*
    * Required TI setting for RX BW > 325 kHz.
    */
  {CC1101_FIFOTHR,     0x07},

  {CC1101_FREND1,      0xB6},

  /*
    * Not relevant to RX operation, but safe to keep.
    */
  {CC1101_FREND0,      0x11},

  /*
    * For ASK/OOK frequency-offset compensation is not supported.
    * FOC_LIMIT must therefore be 0.
    */
  {CC1101_FOCCFG,      0x14},

  /*
    * Required TI settings for RX BW > 325 kHz.
    */
  {CC1101_TEST2,       0x88},
  {CC1101_TEST1,       0x31},
  {CC1101_TEST0,       0x09},
};


/* =========================================================
 * MODE 2: Spectrum Analyzer — Wide Sweep
 *
 * Span = 10 MHz, Step = 78.125 kHz
 * BW = 203 kHz (matches step for good resolution)
 * FS_AUTOCAL = 01 (auto calibration on every IDLE->RX)
 * ========================================================= */
static const registerSetting_t CC1101_Settings_Spectrum[] =
{
  {CC1101_IOCFG0,      0x0D},
  {CC1101_PKTCTRL0,    0x30},
  {CC1101_FSCTRL1,     0x06},
  {CC1101_FSCTRL0,     0x00},
  {CC1101_FREQ2,       0x10}, // 433.920 MHz (overwritten per sweep)
  {CC1101_FREQ1,       0xB0},
  {CC1101_FREQ0,       0x71},
  {CC1101_MDMCFG4,     0x87}, // CHANBW_E=2, CHANBW_M=0 -> 203 kHz
  {CC1101_MDMCFG3,     0x83}, // DRATE_M=131 -> ~4.8 kBaud
  {CC1101_MDMCFG2,     0x30}, // ASK/OOK, no sync
  {CC1101_MCSM0,       0x00}, // FS_AUTOCAL = 00 (use stored FSCAL values)
  {CC1101_AGCCTRL2,    0x04}, // Best sensitivity for OOK
  {CC1101_AGCCTRL1,    0x00},
  {CC1101_AGCCTRL0,    0x92}, // Fast AGC for OOK
  {CC1101_FREND0,      0x11},
  {CC1101_TEST0,       0x09},
};

#endif
