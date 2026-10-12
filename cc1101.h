#ifndef CC1101_H
#define CC1101_H

#include "main.h"
#include <stdint.h>

/* SPI Access Bits */
#define CC1101_READ          0x80U
#define CC1101_BURST         0x40U

/* Command Strobes */
#define CC1101_SRES          0x30U
#define CC1101_SFSTXON       0x31U
#define CC1101_SXOFF         0x32U
#define CC1101_SCAL          0x33U
#define CC1101_SRX           0x34U
#define CC1101_STX           0x35U
#define CC1101_SIDLE         0x36U
#define CC1101_SWOR          0x37U
#define CC1101_SPWD          0x38U
#define CC1101_SFRX          0x39U
#define CC1101_SFTX          0x3AU
#define CC1101_SWORRST       0x3BU
#define CC1101_SNOP          0x3CU

/* Configuration Registers */
#define CC1101_IOCFG2        0x00U
#define CC1101_IOCFG1        0x01U
#define CC1101_IOCFG0        0x02U
#define CC1101_FIFOTHR       0x03U
#define CC1101_SYNC1         0x04U
#define CC1101_SYNC0         0x05U
#define CC1101_PKTLEN        0x06U
#define CC1101_PKTCTRL1      0x07U
#define CC1101_PKTCTRL0      0x08U
#define CC1101_ADDR          0x09U
#define CC1101_CHANNR        0x0AU
#define CC1101_FSCTRL1       0x0BU
#define CC1101_FSCTRL0       0x0CU
#define CC1101_FREQ2         0x0DU
#define CC1101_FREQ1         0x0EU
#define CC1101_FREQ0         0x0FU
#define CC1101_MDMCFG4       0x10U
#define CC1101_MDMCFG3       0x11U
#define CC1101_MDMCFG2       0x12U
#define CC1101_MDMCFG1       0x13U
#define CC1101_MDMCFG0       0x14U
#define CC1101_DEVIATN       0x15U
#define CC1101_MCSM2         0x16U
#define CC1101_MCSM1         0x17U
#define CC1101_MCSM0         0x18U
#define CC1101_FOCCFG        0x19U
#define CC1101_BSCFG         0x1AU
#define CC1101_AGCCTRL2      0x1BU
#define CC1101_AGCCTRL1      0x1CU
#define CC1101_AGCCTRL0      0x1DU
#define CC1101_WOREVT1       0x1EU
#define CC1101_WOREVT0       0x1FU
#define CC1101_WORCTRL       0x20U
#define CC1101_FREND1        0x21U
#define CC1101_FREND0        0x22U
#define CC1101_FSCAL3        0x23U
#define CC1101_FSCAL2        0x24U
#define CC1101_FSCAL1        0x25U
#define CC1101_FSCAL0        0x26U
#define CC1101_RCCTRL1       0x27U
#define CC1101_RCCTRL0       0x28U
#define CC1101_FSTEST        0x29U
#define CC1101_PTEST         0x2AU
#define CC1101_AGCTEST       0x2BU
#define CC1101_TEST2         0x2CU
#define CC1101_TEST1         0x2DU
#define CC1101_TEST0         0x2EU

/* Status Registers */
#define CC1101_PARTNUM       0x30U
#define CC1101_VERSION       0x31U
#define CC1101_FREQEST       0x32U
#define CC1101_LQI           0x33U
#define CC1101_RSSI          0x34U
#define CC1101_MARCSTATE     0x35U
#define CC1101_WORTIME1      0x36U
#define CC1101_WORTIME0      0x37U
#define CC1101_PKTSTATUS     0x38U
#define CC1101_VCO_VC_DAC    0x39U
#define CC1101_TXBYTES       0x3AU
#define CC1101_RXBYTES       0x3BU

/* FIFO and PATABLE */
#define CC1101_PATABLE       0x3EU
#define CC1101_FIFO          0x3FU

/* Structures */
typedef struct
{
    uint8_t fscal3;
    uint8_t fscal2;
    uint8_t fscal1;
} CC1101_Calibration;

typedef struct {
    uint8_t addr;
    uint8_t value;
} registerSetting_t;

/* Core functions */
void CC1101_Init(void);
void CC1101_DelayUs(uint32_t us);
void CC1101_Strobe(uint8_t command);
void CC1101_ApplySettings(const registerSetting_t *settings, uint8_t numSettings);
void CC1101_SetFrequency(uint32_t freq_hz);

/* Radio state */
void CC1101_SetIDLE(void);
void CC1101_SetRX(void);
void CC1101_SetTX(void);
void CC1101_SetSleep(void);

/* Calibration */
void CC1101_Calibrate(void);
void CC1101_ReadCalibration(CC1101_Calibration *cal);
void CC1101_WriteCalibration(const CC1101_Calibration *cal);

/* Status / RSSI */
uint8_t CC1101_ReadStatus(uint8_t addr);
uint8_t CC1101_ReadPartNumber(void);
uint8_t CC1101_ReadVersion(void);
int16_t CC1101_ReadRSSI(void);

/* PATABLE / FIFO */
void CC1101_WritePATABLE(uint8_t value);
void CC1101_WriteFIFO(const uint8_t *data, uint8_t length);
void CC1101_ReadFIFO(uint8_t *data, uint8_t length);

#endif /* CC1101_H */
