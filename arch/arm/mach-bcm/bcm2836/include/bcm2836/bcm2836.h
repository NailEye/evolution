/**********************************************************
 Määritykset BCM2836 piirisarjaan pohjautuville laitteille
***********************************************************/

// Perustuu BCM2835 piirisarjan dokumentaatioon
// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf

#include <common/bit.h>

/***************** Memory ********************/
#define MEMORY_RAM_END                      0x3FFFFFFF

#define UNCACHED_RAM_BASE                   0xC0000000
#define BUS_ADDR_TO_PHY_ADDR(addr)          (addr | UNCACHED_RAM_BASE)

/****************** MMIO *********************/

// MMIO base-osoite
#define MMIO_BASE               0x3F000000

/****************** GPIO *********************/

// GPIO osoite offsetit (suhteessa MMIO base-osoitteeseen)
#define GPIO_BASE_OFFSET        0x200000
#define GPIO_OFFSET(offset)     GPIO_BASE_OFFSET + offset

#define GPIO_GPPUD              GPIO_OFFSET(0x94)
#define GPIO_GPPUDCLK0          GPIO_OFFSET(0x98)

// GPIO Pull Up/Down resistorin tilat
#define GPIO_GPPUD_DISABLE       0x00
#define GPIO_GPPUD_UP            0x01
#define GPIO_GPPUD_DOWN          0x02

#define GPIO_RESET               0x00000000

/********************* UART **********************/

#define UART_CLOCK                      3000000

// UART ososite offsetit (suhteessa MMIO base-osoitteeseen)
#define UART_BASE_OFFSET                0x201000
#define UART_OFFSET(offset)             UART_BASE_OFFSET + offset

#define UART_DR                         UART_OFFSET(0x00)
#define UART_RSRECR                     UART_OFFSET(0x04)
#define UART_FR                         UART_OFFSET(0x18)
#define UART_IBRD                       UART_OFFSET(0x24)
#define UART_FBRD                       UART_OFFSET(0x28)
#define UART_LCRH                       UART_OFFSET(0x2C)
#define UART_CR                         UART_OFFSET(0x30)
#define UART_IFLS                       UART_OFFSET(0x34)
#define UART_IMSC                       UART_OFFSET(0x38)
#define UART_RIS                        UART_OFFSET(0x3C)
#define UART_MIS                        UART_OFFSET(0x40)
#define UART_ICR                        UART_OFFSET(0x44)
#define UART_DMACR                      UART_OFFSET(0x48)
#define UART_ITCR                       UART_OFFSET(0x80)
#define UART_ITIP                       UART_OFFSET(0x84)
#define UART_ITOP                       UART_OFFSET(0x88)
#define UART_TDR                        UART_OFFSET(0x8C)

#define UART_DR_FE                      ENABLE_BIT(8)
#define UART_DR_PE                      ENABLE_BIT(9)
#define UART_DR_BE                      ENABLE_BIT(10)
#define UART_DR_OE                      ENABLE_BIT(11)

#define UART_FR_BUSY                    ENABLE_BIT(3)
#define UART_FR_RXFE                    ENABLE_BIT(4)
#define UART_FR_TXFF                    ENABLE_BIT(5)
#define UART_FR_RXFF                    ENABLE_BIT(6)
#define UART_FR_TXFE                    ENABLE_BIT(7)
#define UART_FR_RI                      ENABLE_BIT(8)

#define UART_CR_UARTEN                  ENABLE_BIT(0)
#define UART_CR_SIREN                   ENABLE_BIT(1)
#define UART_CR_SIRLP                   ENABLE_BIT(2)
#define UART_CR_LBE                     ENABLE_BIT(7)
#define UART_CR_TXE                     ENABLE_BIT(8)
#define UART_CR_RXE                     ENABLE_BIT(9)
#define UART_CR_RTS                     ENABLE_BIT(11)
#define UART_CR_OUT1                    ENABLE_BIT(12)
#define UART_CR_OUT2                    ENABLE_BIT(13)
#define UART_CR_RTSEN                   ENABLE_BIT(14)
#define UART_CR_CTSEN                   ENABLE_BIT(15)

#define UART_CR_DISABLE                    0x00000000

// nUARTCTS modem masked interrupt status
#define UART_ICR_CTSMIC                 ENABLE_BIT(1)
// Receive masked interrupt status
#define UART_ICR_RXIC                   ENABLE_BIT(4)
// Transmit interrupt clear
#define UART_ICR_TXIC                   ENABLE_BIT(5)
// Receive timeout interrupt clear
#define UART_ICR_RTIC                   ENABLE_BIT(6)
// Framing error interrupt clear
#define UART_ICR_FEIC                   ENABLE_BIT(7)
// Parity error interrupt clear
#define UART_ICR_PEIC                   ENABLE_BIT(8)
// Break error interrupt clear
#define UART_ICR_BEIC                   ENABLE_BIT(9)
// Overrun error interrupt clear
#define UART_ICR_OEIC                   ENABLE_BIT(10)

// Kaikki interrupt clear bitit päällä
#define UART_ICR_ALL                    UART_ICR_CTSMIC|UART_ICR_RXIC|UART_ICR_TXIC|UART_ICR_RTIC|UART_ICR_FEIC|UART_ICR_PEIC|UART_ICR_BEIC|UART_ICR_OEIC

#define UART_LCRH_BRK                   ENABLE_BIT(0)
#define UART_LCRH_PEN                   ENABLE_BIT(1)
#define UART_LCRH_EPS                   ENABLE_BIT(2)
#define UART_LCRH_STP2                  ENABLE_BIT(3)
#define UART_LCRH_FEN                   ENABLE_BIT(4)
#define UART_LCRH_WLEN_6                ENABLE_BIT(5)
#define UART_LCRH_WLEN_7                ENABLE_BIT(6)
#define UART_LCRH_WLEN_8                UART_LCRH_WLEN_6|UART_LCRH_WLEN_7
#define UART_LCRH_SPS                   ENABLE_BIT(7)

#define UART_IMSC_CTSMIM                 ENABLE_BIT(1)
#define UART_IMSC_RXIM                   ENABLE_BIT(4)
#define UART_IMSC_TXIM                   ENABLE_BIT(5)
#define UART_IMSC_RTIM                   ENABLE_BIT(6)
#define UART_IMSC_FEIM                   ENABLE_BIT(7)
#define UART_IMSC_PEIM                   ENABLE_BIT(8)
#define UART_IMSC_BEIM                   ENABLE_BIT(9)
#define UART_IMSC_OEIM                   ENABLE_BIT(10)

// Kaikki maski bitit päällä
#define UART_IMSC_ALL                    UART_IMSC_CTSMIM|UART_IMSC_RXIM|UART_IMSC_TXIM|UART_IMSC_RTIM|UART_IMSC_FEIM|UART_IMSC_PEIM|UART_IMSC_BEIM|UART_IMSC_OEIM

/************************** DMA ********************************/
// Speksin maininta: Only three registers in each channels register set are directly writeable (CS, CONBLK_AD and DEBUG).
#define DMA_BASE_OFFSET                 0x7000
#define DMA_OFFSET(offset)              DMA_BASE_OFFSET + offset

#define DMA_ENABLE                      DMA_OFFSET(0xFF0)
#define DMA_DMA0_ENABLE                 ENABLE_BIT(0)

// DMA Channel 0
#define DMA_DMA0_BASE                   DMA_OFFSET(0x00)
#define DMA_DMA0_OFFSET(offset)         DMA_DMA0_BASE + offset
#define DMA_DMA0_CS                     DMA_DMA0_OFFSET(0x00)
#define DMA_DMA0_CONBLK_AD              DMA_DMA0_OFFSET(0x04)
#define DMA_DMA0_TI                     DMA_DMA0_OFFSET(0x08)
#define DMA_DMA0_SOURCE_AD              DMA_DMA0_OFFSET(0x0C)
#define DMA_DMA0_DEST_AD                DMA_DMA0_OFFSET(0x10)
#define DMA_DMA0_TXFR_LEN               DMA_DMA0_OFFSET(0x14)
#define DMA_DMA0_STRIDE                 DMA_DMA0_OFFSET(0x18)
#define DMA_DMA0_NEXTCONBK              DMA_DMA0_OFFSET(0x1C)
#define DMA_DMA0_DEBUG                  DMA_DMA0_OFFSET(0x20)

#define DMA_DMA0_CS_ACTIVE                  ENABLE_BIT(0)
#define DMA_DMA0_CS_END                     ENABLE_BIT(1)
#define DMA_DMA0_CS_INT                     ENABLE_BIT(2)
#define DMA_DMA0_CS_DREQ                    ENABLE_BIT(3)
#define DMA_DMA0_CS_PAUSED                  ENABLE_BIT(4)
#define DMA_DMA0_CS_DREQ_STOPS_DMA          ENABLE_BIT(5)
#define DMA_DMA0_CS_WAITING_FOR_OSW         ENABLE_BIT(6)
#define DMA_DMA0_CS_ERROR                   ENABLE_BIT(8)
#define DMA_DMA0_CS_PRIORITY_START          ENABLE_BIT(16)
#define DMA_DMA0_CS_PANIC_PRIORITY_START    ENABLE_BIT(20)
#define DMA_DMA0_CS_WAIT_FOR_OSW            ENABLE_BIT(28)
#define DMA_DMA0_CS_DISDEBUG                ENABLE_BIT(29)
#define DMA_DMA0_CS_ABORT                   ENABLE_BIT(30)
#define DMA_DMA0_CS_RESET                   ENABLE_BIT(31)

#define DMA_DMA0_TI_INTEN                   ENABLE_BIT(0)
#define DMA_DMA0_TI_TDMODE                  ENABLE_BIT(1)
#define DMA_DMA0_TI_WAIT_RESP               ENABLE_BIT(3)
#define DMA_DMA0_TI_DEST_INC                ENABLE_BIT(4)
#define DMA_DMA0_TI_DEST_WIDTH_128          ENABLE_BIT(5)
#define DMA_DMA0_TI_DEST_DREQ               ENABLE_BIT(6)
#define DMA_DMA0_TI_DEST_IGNORE             ENABLE_BIT(7)
#define DMA_DMA0_TI_SRC_INC                 ENABLE_BIT(8)
#define DMA_DMA0_TI_SRC_WIDTH_128           ENABLE_BIT(9)
#define DMA_DMA0_TI_SRC_DREQ                ENABLE_BIT(10)
#define DMA_DMA0_TI_SRC_IGNORE              ENABLE_BIT(11)
#define DMA_DMA0_TI_BURST_LENGTH_START      12
#define DMA_DMA0_TI_PERMAP_START            ENABLE_BIT(16)
#define DMA_DMA0_TI_WAITS_START             ENABLE_BIT(21)
#define DMA_DMA0_TI_NO_WIDE_BURSTS          ENABLE_BIT(26)

/************************** Mailbox ****************************/

// MAILBOX osoite offsetit (suhteessa MMIO base-osoitteeseen)
#define MB_BASE_OFFSET          0xB880
#define MB_OFFSET(offset)       MB_BASE_OFFSET + offset

#define MB_READ                 MB_OFFSET(0x00)
#define MB_POLL                 MB_OFFSET(0x10)
#define MB_SENDER               MB_OFFSET(0x14)
#define MB_STATUS               MB_OFFSET(0x18)
#define MB_CONFIG               MB_OFFSET(0x1C)
#define MB_WRITE                MB_OFFSET(0x20)

#define MB_RESPONSE             0x80000000

#define MB_FULL                 0x80000000
#define MB_EMPTY                0x40000000