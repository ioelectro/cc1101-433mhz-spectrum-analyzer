# STM32 CC1101 433 MHz Spectrum Analyzer

A lightweight CC1101 driver and a 433 MHz RSSI sweep example for STM32 projects using **STM32 HAL**. The project also includes register settings for raw ASK/OOK reception from common 433.92 MHz remote controls.

## Features

- CC1101 SPI driver using STM32 HAL; no third-party library required.
- 128-point RSSI sweep around 433.92 MHz.
- Configurable sweep span, point count, and RSSI sampling count.
- Per-frequency PLL calibration: captures `FSCAL3..FSCAL1` for each sweep point and restores those values during later sweeps.
- CSV output suitable for a serial terminal or plotting script.
- Separate register profiles for spectrum mode and raw ASK/OOK RX mode.

## Current spectrum defaults

| Parameter | Default |
|---|---:|
| Center frequency | 433.920 MHz |
| Span | 10 MHz |
| Start frequency | 428.920 MHz |
| Number of points | 128 |
| Frequency step | 78.125 kHz |
| RSSI samples per point | 5 |
| AGC wait after entering RX | 300 µs |
| CC1101 reference crystal | 26 MHz |

The sweep uses 128 points with a step of `span / points`. Consequently, the final sampled point is one step below `start + span`.

## Hardware and project requirements

- An STM32 target supported by STM32 HAL.
- A CC1101 module using a **26 MHz crystal**.
- SPI2 configured in STM32CubeMX and initialized as `hspi2`.
- Chip-select macros named `SPI2_CS_GPIO_Port` and `SPI2_CS_Pin`.
- CC1101 MISO/SO connected to `GPIOB`, pin `GPIO_PIN_14` (change the definitions in `cc1101.c` if your wiring differs).
- CMSIS DWT cycle counter available for microsecond delays.
- `printf()` retargeted to a UART if you want to use `Spectrum_PrintCSV()`.

> **Important:** The source is board-specific in a few places (`hspi2`, CS macros, PB14, and DWT). Update these to match your STM32 board before compiling.

## Files

- `cc1101.c`, `cc1101.h` — low-level CC1101 SPI/register driver.
- `cc1101_settings.h` — register profiles for spectrum mode and raw ASK/OOK RX.
- `spectrum.c`, `spectrum.h` — sweep, RSSI collection, peak detection, and CSV output.
- `examples/main_usage.c` — minimal integration example.

## Integration

1. Create or open an STM32CubeIDE project and configure **SPI2** in STM32CubeMX.
2. Connect CC1101 CS to the GPIO represented by `SPI2_CS_GPIO_Port` / `SPI2_CS_Pin`; connect SCK, MOSI, MISO, VCC, and GND. Use a logic-compatible supply for your specific CC1101 module.
3. Add the five source/header files to your project.
4. Confirm `hspi2` is the SPI handle used by the driver. Change it if you use another SPI peripheral.
5. Confirm the MISO/SO GPIO in `cc1101.c`.
6. Enable the DWT cycle counter before calling `CC1101_DelayUs()`.
7. Call `CC1101_Init()` and then `Spectrum_Init()` after HAL, GPIO, and SPI initialization.
8. Repeatedly call `Spectrum_Run(&result)`. Use `Spectrum_PrintCSV(&result)` if `printf()` is routed to a UART.

### Minimal example

See [`examples/main_usage.c`](examples/main_usage.c). The essential sequence is:

```c
CC1101_Init();
Spectrum_Init();

while (1)
{
    Spectrum_Run(&spectrum);
    Spectrum_PrintCSV(&spectrum);
    HAL_Delay(100);
}
```

The example assumes that `SystemClock_Config()`, `MX_GPIO_Init()`, `MX_SPI2_Init()`, and UART `printf()` retargeting are provided by your STM32 project.

## DWT microsecond delay requirement

`CC1101_DelayUs()` reads `DWT->CYCCNT`, so enable the counter once during startup, before using the driver. On Cortex-M targets that implement DWT, the typical CMSIS sequence is:

```c
CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
DWT->CYCCNT = 0;
DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
```

Check your MCU reference manual and CMSIS definitions. Some cores/implementations may not provide DWT cycle counting.

## CSV format

`Spectrum_PrintCSV()` prints:

```text
FREQ_HZ,RSSI_DBM
428920000,-100
...
PEAK,433920000,-55
```

The values above are illustrative, not a measurement guarantee. RSSI readings depend on module layout, antenna, gain settings, interference, and calibration. The function prints 128 frequency/RSSI rows followed by the strongest measured point.

## Raw ASK/OOK receiver profile

`cc1101_settings.h` also defines `CC1101_Settings_RX_ASK`. To apply that profile:

```c
CC1101_Init();

CC1101_ApplySettings(
    CC1101_Settings_RX_ASK,
    sizeof(CC1101_Settings_RX_ASK) / sizeof(CC1101_Settings_RX_ASK[0])
);

CC1101_SetRX();
```

This profile configures the CC1101 for asynchronous serial output on **GDO0**. Configure the corresponding STM32 GPIO as an input and sample it or connect it to a timer/input-capture peripheral to decode pulse timing. This repository does not include a complete remote-protocol decoder.

Do not apply the raw ASK/OOK profile and spectrum profile at the same time; each profile configures the radio for a different task.

## Configuration

Edit the definitions in `spectrum.h`:

- `SPECTRUM_CENTER_HZ`
- `SPECTRUM_SPAN_HZ`
- `SPECTRUM_POINTS`
- `SPECTRUM_RSSI_SAMPLES`
- `SPECTRUM_AGC_WAIT_US`

`SPECTRUM_POINTS` changes the size of the result array and the per-frequency calibration array, so account for the additional RAM usage when increasing it.

## Limitations and notes

- This is an RSSI sweep, not a calibrated laboratory spectrum analyzer. Amplitude readings are approximate.
- Resolution is limited by the CC1101 receiver bandwidth, frequency step, settling time, and RF environment.
- The current driver ignores the return status of HAL SPI transfers and uses `HAL_MAX_DELAY`; add error handling/timeouts for production use.
- `CC1101_DelayUs()` assumes the DWT counter is enabled and the HCLK frequency is stable.
- Confirm your local RF regulations before transmitting. This project is primarily intended for receive-only spectrum observation.
