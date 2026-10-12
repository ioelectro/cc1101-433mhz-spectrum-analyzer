#include "spectrum.h"
#include "cc1101.h"
#include "cc1101_settings.h"

#include <stdio.h>
#include <stddef.h>

static CC1101_Calibration Spectrum_Calibration[SPECTRUM_POINTS];

static uint32_t Spectrum_Frequency(uint32_t index)
{
    return SPECTRUM_START_HZ + (index * SPECTRUM_STEP_HZ);
}

static void Spectrum_ApplySettings(void)
{
    const uint8_t count =
        sizeof(CC1101_Settings_Spectrum) /
        sizeof(registerSetting_t);

    CC1101_ApplySettings(CC1101_Settings_Spectrum, count);
}

static void Spectrum_Calibrate(void)
{
    for (uint32_t i = 0U; i < SPECTRUM_POINTS; i++)
    {
        const uint32_t freq = Spectrum_Frequency(i);

        CC1101_SetIDLE();
        CC1101_DelayUs(50U);

        CC1101_SetFrequency(freq);

        /*
         * Calibrate PLL at this frequency and save the result.
         */
        CC1101_Calibrate();
        CC1101_ReadCalibration(&Spectrum_Calibration[i]);
    }
}

static int16_t Spectrum_ReadRSSI(void)
{
    int16_t max=-120,rssi;

    for (uint8_t i = 0U; i < SPECTRUM_RSSI_SAMPLES; i++)
    {
        rssi= CC1101_ReadRSSI();
        if(rssi>max)max=rssi;
        CC1101_DelayUs(80U);
    }
    return max;
}

void Spectrum_Init(void)
{
    Spectrum_ApplySettings();

    /*
     * Calibrate PLL once at every sweep frequency and keep the
     * FSCAL3/FSCAL2/FSCAL1 values for fast frequency hopping.
     */
    Spectrum_Calibrate();

    CC1101_SetIDLE();
    CC1101_DelayUs(50U);
    CC1101_SetFrequency(SPECTRUM_CENTER_HZ);
    CC1101_WriteCalibration(&Spectrum_Calibration[
        (SPECTRUM_CENTER_HZ - SPECTRUM_START_HZ) / SPECTRUM_STEP_HZ
    ]);
    CC1101_SetRX();
    CC1101_DelayUs(SPECTRUM_AGC_WAIT_US);
}

void Spectrum_Run(Spectrum_Result *result)
{
    if (result == NULL) return;

    result->peak_frequency_hz = SPECTRUM_CENTER_HZ;
    result->peak_rssi_dbm     = -127;

    for (uint32_t i = 0U; i < SPECTRUM_POINTS; i++)
    {
        const uint32_t freq = Spectrum_Frequency(i);

        CC1101_SetIDLE();
        CC1101_DelayUs(50U);

        CC1101_SetFrequency(freq);

        /*
         * Restore the PLL calibration previously measured at this
         * exact frequency. Auto-calibration is disabled for spectrum mode.
         */
        CC1101_WriteCalibration(&Spectrum_Calibration[i]);

        CC1101_SetRX();

        CC1101_DelayUs(SPECTRUM_AGC_WAIT_US);

        const int16_t rssi = Spectrum_ReadRSSI();

        result->rssi_dbm[i] = rssi;

        if (rssi > result->peak_rssi_dbm)
        {
            result->peak_rssi_dbm = rssi;
            result->peak_frequency_hz = freq;
        }
    }
}

void Spectrum_PrintCSV(const Spectrum_Result *result)
{
    if (result == NULL) return;

    printf("FREQ_HZ,RSSI_DBM\r\n");

    for (uint32_t i = 0U; i < SPECTRUM_POINTS; i++)
    {
        printf("%lu,%d\r\n",
               (unsigned long)Spectrum_Frequency(i),
               result->rssi_dbm[i]);
    }

    printf("PEAK,%lu,%d\r\n",
           (unsigned long)result->peak_frequency_hz,
           result->peak_rssi_dbm);
}
