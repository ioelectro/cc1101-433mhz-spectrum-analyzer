#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <stdint.h>

/*
 * Spectrum Configuration
 *
 * Center = 433.920 MHz
 * Span   = 10 MHz (+/- 5 MHz)
 * Range  = 428.920 .. 438.920 MHz
 * Points = 128
 * Step   = 78.125 kHz
*/
#define SPECTRUM_CENTER_HZ      433920000UL
#define SPECTRUM_SPAN_HZ        10000000UL
#define SPECTRUM_POINTS         128U
#define SPECTRUM_STEP_HZ        (SPECTRUM_SPAN_HZ / SPECTRUM_POINTS)
#define SPECTRUM_START_HZ       (SPECTRUM_CENTER_HZ - (SPECTRUM_SPAN_HZ / 2U))

/* Timing */
#define SPECTRUM_SCAL_WAIT_US   900U
#define SPECTRUM_AGC_WAIT_US    300U
#define SPECTRUM_RSSI_SAMPLES   5U

typedef struct
{
    uint32_t peak_frequency_hz;
    int16_t  peak_rssi_dbm;
    int16_t  rssi_dbm[SPECTRUM_POINTS];
} Spectrum_Result;

void Spectrum_Init(void);
void Spectrum_Run(Spectrum_Result *result);
void Spectrum_PrintCSV(const Spectrum_Result *result);

#endif
