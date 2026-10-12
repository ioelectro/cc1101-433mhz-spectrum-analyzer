#include "cc1101.h"
#include <stddef.h>

extern SPI_HandleTypeDef hspi2;

#define CC1101_CS_PORT              SPI2_CS_GPIO_Port
#define CC1101_CS_PIN               SPI2_CS_Pin
#define CC1101_MISO_PORT            GPIOB
#define CC1101_MISO_PIN             GPIO_PIN_14
#define CC1101_CHIP_RDY_TIMEOUT_US  1000U

#define CS_LOW() \
    HAL_GPIO_WritePin(CC1101_CS_PORT, CC1101_CS_PIN, GPIO_PIN_RESET)
#define CS_HIGH() \
    HAL_GPIO_WritePin(CC1101_CS_PORT, CC1101_CS_PIN, GPIO_PIN_SET)

static uint8_t SPI2_TxRx(uint8_t data)
{
    uint8_t rx = 0U;
    (void)HAL_SPI_TransmitReceive(&hspi2, &data, &rx, 1U, HAL_MAX_DELAY);
    return rx;
}


void CC1101_DelayUs(uint32_t us)
{
    const uint32_t cycles_per_us = HAL_RCC_GetHCLKFreq() / 1000000U;
    const uint32_t cycles = cycles_per_us * us;
    const uint32_t start = DWT->CYCCNT;
    while ((uint32_t)(DWT->CYCCNT - start) < cycles) { }
}

static uint8_t CC1101_Select(void)
{
    uint32_t elapsed = 0U;
    CS_LOW();

    while (HAL_GPIO_ReadPin(CC1101_MISO_PORT, CC1101_MISO_PIN) == GPIO_PIN_SET)
    {
        CC1101_DelayUs(1U);
        if (++elapsed >= CC1101_CHIP_RDY_TIMEOUT_US)
        {
            CS_HIGH();
            return 0U;
        }
    }
    return 1U;
}

static void CC1101_Deselect(void)
{
    CS_HIGH();
}

void CC1101_Strobe(uint8_t command)
{
    if (CC1101_Select() == 0U) return;
    (void)SPI2_TxRx(command);
    CC1101_Deselect();
}

static void CC1101_WriteReg(uint8_t addr, uint8_t value)
{
    if (CC1101_Select() == 0U) return;
    (void)SPI2_TxRx(addr);
    (void)SPI2_TxRx(value);
    CC1101_Deselect();
}

static void CC1101_WriteBurst(uint8_t addr, const uint8_t *data, uint8_t length)
{
    if ((data == NULL) || (length == 0U)) return;
    if (CC1101_Select() == 0U) return;

    (void)SPI2_TxRx(addr | CC1101_BURST);
    for (uint8_t i = 0U; i < length; i++)
        (void)SPI2_TxRx(data[i]);
    CC1101_Deselect();
}

static uint8_t CC1101_ReadReg(uint8_t addr)
{
    uint8_t value = 0U;
    if (CC1101_Select() == 0U) return 0U;
    (void)SPI2_TxRx(addr | CC1101_READ);
    value = SPI2_TxRx(0x00U);
    CC1101_Deselect();
    return value;
}

uint8_t CC1101_ReadStatus(uint8_t addr)
{
    uint8_t value = 0U;
    if (CC1101_Select() == 0U) return 0U;
    (void)SPI2_TxRx(addr | CC1101_READ | CC1101_BURST);
    value = SPI2_TxRx(0x00U);
    CC1101_Deselect();
    return value;
}

uint8_t CC1101_ReadPartNumber(void) { return CC1101_ReadStatus(CC1101_PARTNUM); }
uint8_t CC1101_ReadVersion(void)    { return CC1101_ReadStatus(CC1101_VERSION); }

void CC1101_ApplySettings(const registerSetting_t *settings, uint8_t numSettings)
{
    if (settings == NULL) return;
    for (uint8_t i = 0U; i < numSettings; i++)
        CC1101_WriteReg(settings[i].addr, settings[i].value);
}

void CC1101_SetFrequency(uint32_t freq_hz)
{
    uint32_t freq_word;
    uint8_t data[3];

    freq_word = (uint32_t)(((uint64_t)freq_hz << 16) / 26000000ULL);
    data[0] = (uint8_t)(freq_word >> 16);
    data[1] = (uint8_t)(freq_word >> 8);
    data[2] = (uint8_t)freq_word;

    CC1101_WriteBurst(CC1101_FREQ2, data, 3U);
}

void CC1101_SetIDLE(void)  { CC1101_Strobe(CC1101_SIDLE); }
void CC1101_SetRX(void)    { CC1101_Strobe(CC1101_SRX);   }
void CC1101_SetTX(void)    { CC1101_Strobe(CC1101_STX);   }
void CC1101_SetSleep(void) { CC1101_Strobe(CC1101_SPWD);  }

void CC1101_Calibrate(void)
{
    CC1101_Strobe(CC1101_SIDLE);
    CC1101_Strobe(CC1101_SCAL);
    CC1101_DelayUs(800U);
}

void CC1101_ReadCalibration(CC1101_Calibration *cal)
{
    if (cal == NULL) return;
    cal->fscal3 = CC1101_ReadReg(CC1101_FSCAL3);
    cal->fscal2 = CC1101_ReadReg(CC1101_FSCAL2);
    cal->fscal1 = CC1101_ReadReg(CC1101_FSCAL1);
}

void CC1101_WriteCalibration(const CC1101_Calibration *cal)
{
    uint8_t data[3];
    if (cal == NULL) return;
    data[0] = cal->fscal3;
    data[1] = cal->fscal2;
    data[2] = cal->fscal1;
    CC1101_WriteBurst(CC1101_FSCAL3, data, 3U);
}

int16_t CC1101_ReadRSSI(void)
{
    const uint8_t raw = CC1101_ReadStatus(CC1101_RSSI);
    int16_t rssi_code = (int16_t)raw;

    if (rssi_code >= 128) rssi_code -= 256;

    return (int16_t)((rssi_code / 2) - 74);
}

void CC1101_WritePATABLE(uint8_t value)
{
    CC1101_WriteBurst(CC1101_PATABLE, &value, 1U);
}

void CC1101_WriteFIFO(const uint8_t *data, uint8_t length)
{
    if ((data == NULL) || (length == 0U)) return;
    CC1101_WriteBurst(CC1101_FIFO, data, length);
}

void CC1101_ReadFIFO(uint8_t *data, uint8_t length)
{
    if ((data == NULL) || (length == 0U)) return;
    if (CC1101_Select() == 0U) return;

    (void)SPI2_TxRx(CC1101_FIFO | CC1101_READ | CC1101_BURST);
    for (uint8_t i = 0U; i < length; i++)
        data[i] = SPI2_TxRx(0x00U);
    CC1101_Deselect();
}


void CC1101_Init(void)
{
    CS_HIGH();
    HAL_Delay(1U);

    CC1101_Strobe(CC1101_SRES);
    HAL_Delay(1U);
}
