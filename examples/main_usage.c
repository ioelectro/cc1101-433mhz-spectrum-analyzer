/*
 * Minimal integration example for STM32 HAL / STM32CubeIDE.
 *
 * Assumptions:
 * - The project already provides main.h, hspi2, SystemClock_Config(),
 *   MX_GPIO_Init(), MX_SPI2_Init(), and a printf() UART retarget.
 * - SPI2_CS_GPIO_Port / SPI2_CS_Pin match the CC1101 chip-select wiring.
 * - The CC1101 MISO/SO signal is connected to PB14, as configured in cc1101.c.
 * - The MCU implements the CMSIS DWT cycle counter.
 *
 * Merge the relevant parts into your generated main.c; this file is an example,
 * not a standalone build target.
 */

#include "main.h"
#include "cc1101.h"
#include "spectrum.h"
#include <stdio.h>

static Spectrum_Result spectrum;

static void DWT_EnableCycleCounter(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_SPI2_Init();

    /* Enable before any call to CC1101_DelayUs(). */
    DWT_EnableCycleCounter();

    /* Optional: initialize your UART before using printf(). */

    CC1101_Init();
    Spectrum_Init();

    while (1)
    {
        Spectrum_Run(&spectrum);
        Spectrum_PrintCSV(&spectrum);
        HAL_Delay(100U);
    }
}
