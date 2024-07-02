#include "main.h"

extern ETH_HandleTypeDef heth;
extern ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT];
static void read_phy(uint16_t phy_address, uint32_t *register_value);
static void write_phy(uint16_t phy_address, uint16_t register_value);

uint8_t rx_buffer[ETH_RX_DESC_CNT][ETH_RX_BUF_SIZE];

void set_phy(void)
{
    const uint16_t ETH_CHIP_PHYSCSR = 0x11;
    uint32_t phyreg = 0;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, SET);

    write_phy(PHY_BCR, PHY_RESET);
    do
    {
        read_phy(PHY_BCR, &phyreg);
    } while (phyreg & PHY_RESET);

    read_phy(PHY_BCR, &phyreg);
    phyreg |= PHY_AUTONEGOTIATION;
    write_phy(PHY_BCR, phyreg);

    do
    {
        read_phy(PHY_BSR, &phyreg);
    } while (!((phyreg & 0x24) == 0x24));

    do
    {
        read_phy(ETH_CHIP_PHYSCSR, &phyreg);
    } while (!((phyreg >> 13) == 3));

    HAL_Delay(1000);

    ETH_MACConfigTypeDef MACConf = {0};
    HAL_ETH_GetMACConfig(&heth, &MACConf);
    MACConf.DuplexMode = ETH_FULLDUPLEX_MODE;
    MACConf.Speed = ETH_SPEED_100M;
    HAL_ETH_SetMACConfig(&heth, &MACConf);
}

static void read_phy(uint16_t phy_address, uint32_t *register_value)
{
    HAL_ETH_ReadPHYRegister(&heth, 0, phy_address, register_value);
}

static void write_phy(uint16_t phy_address, uint16_t register_value)
{
    HAL_ETH_WritePHYRegister(&heth, 0, phy_address, register_value);
}

void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
}

void set_rx_dma(void)
{
    for (size_t i = 0; i < ETH_RX_DESC_CNT; i++)
        DMARxDscrTab[i].DESC2 = (uint32_t)rx_buffer[i];
}
