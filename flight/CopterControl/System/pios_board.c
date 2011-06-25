/**
 ******************************************************************************
 * @addtogroup OpenPilotSystem OpenPilot System
 * @{
 * @addtogroup OpenPilotCore OpenPilot Core
 * @{
 *
 * @file       pios_board.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Defines board specific static initializers for hardware for the OpenPilot board.
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <pios.h>
#include <openpilot.h>
#include <uavobjectsinit.h>

#if defined(PIOS_INCLUDE_SPI)

#include <pios_spi_priv.h>

/* Flash/Accel Interface
 * 
 * NOTE: Leave this declared as const data so that it ends up in the 
 * .rodata section (ie. Flash) rather than in the .bss section (RAM).
 */
void PIOS_SPI_flash_accel_irq_handler(void);
void DMA1_Channel4_IRQHandler() __attribute__ ((alias ("PIOS_SPI_flash_accel_irq_handler")));
void DMA1_Channel5_IRQHandler() __attribute__ ((alias ("PIOS_SPI_flash_accel_irq_handler")));
const struct pios_spi_cfg pios_spi_flash_accel_cfg = {
  .regs   = SPI2,
  .init   = {
    .SPI_Mode              = SPI_Mode_Master,
    .SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
    .SPI_DataSize          = SPI_DataSize_8b,
    .SPI_NSS               = SPI_NSS_Soft,
    .SPI_FirstBit          = SPI_FirstBit_MSB,
    .SPI_CRCPolynomial     = 7,
    .SPI_CPOL              = SPI_CPOL_High,
    .SPI_CPHA              = SPI_CPHA_2Edge,
    .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2, 
  },
  .use_crc = FALSE,
  .dma = {
    .ahb_clk  = RCC_AHBPeriph_DMA1,
    
    .irq = {
      .handler = PIOS_SPI_flash_accel_irq_handler,
      .flags   = (DMA1_FLAG_TC4 | DMA1_FLAG_TE4 | DMA1_FLAG_HT4 | DMA1_FLAG_GL4),
      .init    = {
	.NVIC_IRQChannel                   = DMA1_Channel4_IRQn,
	.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
	.NVIC_IRQChannelSubPriority        = 0,
	.NVIC_IRQChannelCmd                = ENABLE,
      },
    },

    .rx = {
      .channel = DMA1_Channel4,
      .init    = {
	.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR),
	.DMA_DIR                = DMA_DIR_PeripheralSRC,
	.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
	.DMA_MemoryInc          = DMA_MemoryInc_Enable,
	.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
	.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
	.DMA_Mode               = DMA_Mode_Normal,
	.DMA_Priority           = DMA_Priority_High,
	.DMA_M2M                = DMA_M2M_Disable,
      },
    },
    .tx = {
      .channel = DMA1_Channel5,
      .init    = {
	.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR),
	.DMA_DIR                = DMA_DIR_PeripheralDST,
	.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
	.DMA_MemoryInc          = DMA_MemoryInc_Enable,
	.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
	.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
	.DMA_Mode               = DMA_Mode_Normal,
	.DMA_Priority           = DMA_Priority_High,
	.DMA_M2M                = DMA_M2M_Disable,
      },
    },
  },
  .ssel = {
    .gpio = GPIOB,
    .init = {
      .GPIO_Pin   = GPIO_Pin_12,
      .GPIO_Speed = GPIO_Speed_10MHz,
      .GPIO_Mode  = GPIO_Mode_Out_PP,
    },
  },
  .sclk = {
    .gpio = GPIOB,
    .init = {
      .GPIO_Pin   = GPIO_Pin_13,
      .GPIO_Speed = GPIO_Speed_10MHz,
      .GPIO_Mode  = GPIO_Mode_AF_PP,
    },
  },
  .miso = {
    .gpio = GPIOB,
    .init = {
      .GPIO_Pin   = GPIO_Pin_14,
      .GPIO_Speed = GPIO_Speed_10MHz,
      .GPIO_Mode  = GPIO_Mode_IN_FLOATING,
    },
  },
  .mosi = {
    .gpio = GPIOB,
    .init = {
      .GPIO_Pin   = GPIO_Pin_15,
      .GPIO_Speed = GPIO_Speed_10MHz,
      .GPIO_Mode  = GPIO_Mode_AF_PP,
    },
  },
};

static uint32_t pios_spi_flash_accel_id;
void PIOS_SPI_flash_accel_irq_handler(void)
{
  /* Call into the generic code to handle the IRQ for this specific device */
	PIOS_SPI_IRQ_Handler(pios_spi_flash_accel_id);
}

#endif	/* PIOS_INCLUDE_SPI */

/*
 * ADC system
 */
#include "pios_adc_priv.h"
extern void PIOS_ADC_handler(void);
void DMA1_Channel1_IRQHandler() __attribute__ ((alias("PIOS_ADC_handler")));
// Remap the ADC DMA handler to this one
const struct pios_adc_cfg pios_adc_cfg = {
	.dma = {
		.ahb_clk  = RCC_AHBPeriph_DMA1,
		.irq = {
			.handler = PIOS_ADC_DMA_Handler,
			.flags   = (DMA1_FLAG_TC1 | DMA1_FLAG_TE1 | DMA1_FLAG_HT1 | DMA1_FLAG_GL1),
			.init    = {
				.NVIC_IRQChannel                   = DMA1_Channel1_IRQn,
				.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
				.NVIC_IRQChannelSubPriority        = 0,
				.NVIC_IRQChannelCmd                = ENABLE,
			},
		},
		.rx = {
			.channel = DMA1_Channel1,
			.init    = {
				.DMA_PeripheralBaseAddr = (uint32_t) & ADC1->DR,
				.DMA_DIR                = DMA_DIR_PeripheralSRC,
				.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
				.DMA_MemoryInc          = DMA_MemoryInc_Enable,
				.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word,
				.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word,
				.DMA_Mode               = DMA_Mode_Circular,
				.DMA_Priority           = DMA_Priority_High,
				.DMA_M2M                = DMA_M2M_Disable,
			},
		}
	}, 
	.half_flag = DMA1_IT_HT1,
	.full_flag = DMA1_IT_TC1,
};

struct pios_adc_dev pios_adc_devs[] = {
	{
		.cfg = &pios_adc_cfg,
		.callback_function = NULL,
	},
};

uint8_t pios_adc_num_devices = NELEMENTS(pios_adc_devs);

void PIOS_ADC_handler() {
	PIOS_ADC_DMA_Handler();
}

#if defined(PIOS_INCLUDE_USART)

#include "pios_usart_priv.h"

/*
 * Serial port configuration.
 * TODO: This should be dynamic in the future.
 * But for now define any compatile combination of:
 *   USE_I2C (shared with USART3)
 *   USE_TELEMETRY
 *   USE_GPS
 *   USE_SPEKTRUM
 *   USE_SBUS (USART1 only, it needs an invertor)
 * and optionally define PIOS_PORT_* to USART port numbers
 */

/* Serial telemetry: USART1 or USART3 */
#if !defined(PIOS_PORT_TELEMETRY)
#define PIOS_PORT_TELEMETRY	1
#endif

/* GPS receiver: USART1 or USART3 */
#if !defined(PIOS_PORT_GPS)
#define PIOS_PORT_GPS		3
#endif

/* Spektrum satellite receiver: USART1 or USART3 */
#if !defined(PIOS_PORT_SPEKTRUM)
#define PIOS_PORT_SPEKTRUM	3
#endif

/* Futaba S.Bus receiver: USART1 only (needs invertor) */
#if !defined(PIOS_PORT_SBUS)
#define PIOS_PORT_SBUS		1
#endif

/*
 * Define USART port configurations and check for conflicts
 * making sure they do not conflict with each other and with I2C.
 */
#define USART_GPIO(port)	(((port) == 1) ? GPIOA : GPIOB)
#define USART_RXIO(port)	(((port) == 1) ? GPIO_Pin_10 : GPIO_Pin_11)
#define USART_TXIO(port)	(((port) == 1) ? GPIO_Pin_9  : GPIO_Pin_10)
 
#if defined(USE_TELEMETRY)
#if defined(USE_I2C) && (PIOS_PORT_TELEMETRY == 3)
#error defined(USE_I2C) && (PIOS_PORT_TELEMETRY == 3)
#endif
#if (PIOS_PORT_TELEMETRY == 1)
#define PIOS_USART_TELEMETRY	USART1
#define PIOS_IRQH_TELEMETRY	USART1_IRQHandler
#define PIOS_IRQC_TELEMETRY	USART1_IRQn
#else
#define PIOS_USART_TELEMETRY	USART3
#define PIOS_IRQH_TELEMETRY	USART3_IRQHandler
#define PIOS_IRQC_TELEMETRY	USART3_IRQn
#endif
#define PIOS_GPIO_TELEMETRY	USART_GPIO(PIOS_PORT_TELEMETRY)
#define PIOS_RXIO_TELEMETRY	USART_RXIO(PIOS_PORT_TELEMETRY)
#define PIOS_TXIO_TELEMETRY	USART_TXIO(PIOS_PORT_TELEMETRY)
#endif

#if defined(USE_GPS)
#if defined(USE_I2C) && (PIOS_PORT_GPS == 3)
#error defined(USE_I2C) && (PIOS_PORT_GPS == 3)
#endif
#if defined(USE_TELEMETRY) && (PIOS_PORT_TELEMETRY == PIOS_PORT_GPS)
#error defined(USE_TELEMETRY) && (PIOS_PORT_TELEMETRY == PIOS_PORT_GPS)
#endif
#if (PIOS_PORT_GPS == 1)
#define PIOS_USART_GPS		USART1
#define PIOS_IRQH_GPS		USART1_IRQHandler
#define PIOS_IRQC_GPS		USART1_IRQn
#else
#define PIOS_USART_GPS		USART3
#define PIOS_IRQH_GPS		USART3_IRQHandler
#define PIOS_IRQC_GPS		USART3_IRQn
#endif
#define PIOS_GPIO_GPS		USART_GPIO(PIOS_PORT_GPS)
#define PIOS_RXIO_GPS		USART_RXIO(PIOS_PORT_GPS)
#define PIOS_TXIO_GPS		USART_TXIO(PIOS_PORT_GPS)
#endif

#if defined(USE_SPEKTRUM)
#if defined(USE_I2C) && (PIOS_PORT_SPEKTRUM == 3)
#error defined(USE_I2C) && (PIOS_PORT_SPEKTRUM == 3)
#endif
#if defined(USE_TELEMETRY) && (PIOS_PORT_SPEKTRUM == PIOS_PORT_TELEMETRY)
#error defined(USE_TELEMETRY) && (PIOS_PORT_SPEKTRUM == PIOS_PORT_TELEMETRY)
#endif
#if defined(USE_GPS) && (PIOS_PORT_SPEKTRUM == PIOS_PORT_GPS)
#error defined(USE_GPS) && (PIOS_PORT_SPEKTRUM == PIOS_PORT_GPS)
#endif
#if defined(USE_SBUS)
#error defined(USE_SPEKTRUM) && defined(USE_SBUS)
#endif
#if (PIOS_PORT_SPEKTRUM == 1)
#define PIOS_USART_SPEKTRUM	USART1
#define PIOS_IRQH_SPEKTRUM	USART1_IRQHandler
#define PIOS_IRQC_SPEKTRUM	USART1_IRQn
#else
#define PIOS_USART_SPEKTRUM	USART3
#define PIOS_IRQH_SPEKTRUM	USART3_IRQHandler
#define PIOS_IRQC_SPEKTRUM	USART3_IRQn
#endif
#define PIOS_GPIO_SPEKTRUM	USART_GPIO(PIOS_PORT_SPEKTRUM)
#define PIOS_RXIO_SPEKTRUM	USART_RXIO(PIOS_PORT_SPEKTRUM)
#define PIOS_TXIO_SPEKTRUM	USART_TXIO(PIOS_PORT_SPEKTRUM)
#endif

#if defined(USE_SBUS)
#if (PIOS_PORT_SBUS != 1)
#error (PIOS_PORT_SBUS != 1)
#endif
#if defined(USE_TELEMETRY) && (PIOS_PORT_SBUS == PIOS_PORT_TELEMETRY)
#error defined(USE_TELEMETRY) && (PIOS_PORT_SBUS == PIOS_PORT_TELEMETRY)
#endif
#if defined(USE_GPS) && (PIOS_PORT_SBUS == PIOS_PORT_GPS)
#error defined(USE_GPS) && (PIOS_PORT_SBUS == PIOS_PORT_GPS)
#endif
#if defined(USE_SPEKTRUM)
#error defined(USE_SPEKTRUM) && defined(USE_SBUS)
#endif
#define PIOS_USART_SBUS		USART1
#define PIOS_IRQH_SBUS		USART1_IRQHandler
#define PIOS_IRQC_SBUS		USART1_IRQn
#define PIOS_GPIO_SBUS		USART_GPIO(PIOS_PORT_SBUS)
#define PIOS_RXIO_SBUS		USART_RXIO(PIOS_PORT_SBUS)
#define PIOS_TXIO_SBUS		USART_TXIO(PIOS_PORT_SBUS)
#endif

#if defined(PIOS_INCLUDE_TELEMETRY_RF)
/*
 * Telemetry USART
 */
void PIOS_USART_telem_irq_handler(void);
void PIOS_IRQH_TELEMETRY() __attribute__ ((alias ("PIOS_USART_telem_irq_handler")));
const struct pios_usart_cfg pios_usart_telem_cfg = {
  .regs  = PIOS_USART_TELEMETRY,
  .init = {
    #if defined (PIOS_COM_TELEM_BAUDRATE)
        .USART_BaudRate        = PIOS_COM_TELEM_BAUDRATE,
    #else
        .USART_BaudRate        = 57600,
    #endif
    .USART_WordLength          = USART_WordLength_8b,
    .USART_Parity              = USART_Parity_No,
    .USART_StopBits            = USART_StopBits_1,
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    .USART_Mode                = USART_Mode_Rx | USART_Mode_Tx,
  },
  .irq = {
    .handler = PIOS_USART_telem_irq_handler,
    .init    = {
      .NVIC_IRQChannel                   = PIOS_IRQC_TELEMETRY,
      .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
      .NVIC_IRQChannelSubPriority        = 0,
      .NVIC_IRQChannelCmd                = ENABLE,
    },
  },
  .rx   = {
    .gpio = PIOS_GPIO_TELEMETRY,
    .init = {
      .GPIO_Pin   = PIOS_RXIO_TELEMETRY,
      .GPIO_Speed = GPIO_Speed_2MHz,
      .GPIO_Mode  = GPIO_Mode_IPU,
    },
  },
  .tx   = {
    .gpio = PIOS_GPIO_TELEMETRY,
    .init = {
      .GPIO_Pin   = PIOS_TXIO_TELEMETRY,
      .GPIO_Speed = GPIO_Speed_2MHz,
      .GPIO_Mode  = GPIO_Mode_AF_PP,
    },
  },
};
#endif /* PIOS_INCLUDE_TELEMETRY_RF */

#if defined(PIOS_INCLUDE_GPS)
/*
 * GPS USART
 */
void PIOS_USART_gps_irq_handler(void);
void PIOS_IRQH_GPS() __attribute__ ((alias ("PIOS_USART_gps_irq_handler")));
const struct pios_usart_cfg pios_usart_gps_cfg = {
  .regs = PIOS_USART_GPS,
  .init = {
    #if defined (PIOS_COM_GPS_BAUDRATE)
        .USART_BaudRate        = PIOS_COM_GPS_BAUDRATE,
    #else
        .USART_BaudRate        = 57600,
    #endif
    .USART_WordLength          = USART_WordLength_8b,
    .USART_Parity              = USART_Parity_No,
    .USART_StopBits            = USART_StopBits_1,
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    .USART_Mode                = USART_Mode_Rx | USART_Mode_Tx,
  },
  .irq = {
    .handler = PIOS_USART_gps_irq_handler,
    .init    = {
      .NVIC_IRQChannel                   = PIOS_IRQC_GPS,
      .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
      .NVIC_IRQChannelSubPriority        = 0,
      .NVIC_IRQChannelCmd                = ENABLE,
    },
  },
  .rx   = {
    .gpio = PIOS_GPIO_GPS,
    .init = {
      .GPIO_Pin   = PIOS_RXIO_GPS,
      .GPIO_Speed = GPIO_Speed_2MHz,
      .GPIO_Mode  = GPIO_Mode_IPU,
    },
  },
  .tx   = {
    .gpio = PIOS_GPIO_GPS,
    .init = {
      .GPIO_Pin   = PIOS_TXIO_GPS,
      .GPIO_Speed = GPIO_Speed_2MHz,
      .GPIO_Mode  = GPIO_Mode_AF_PP,
    },
  },
};
#endif

#if defined(PIOS_INCLUDE_SPEKTRUM)
/*
 * SPEKTRUM USART
 */
void PIOS_USART_spektrum_irq_handler(void);
void PIOS_IRQH_SPEKTRUM() __attribute__ ((alias ("PIOS_USART_spektrum_irq_handler")));
const struct pios_usart_cfg pios_usart_spektrum_cfg = {
	.regs = PIOS_USART_SPEKTRUM,
	.init = {
	#if defined (PIOS_COM_SPEKTRUM_BAUDRATE)
		.USART_BaudRate        = PIOS_COM_SPEKTRUM_BAUDRATE,
	#else
		.USART_BaudRate        = 115200,
	#endif
		.USART_WordLength          = USART_WordLength_8b,
		.USART_Parity              = USART_Parity_No,
		.USART_StopBits            = USART_StopBits_1,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
		.USART_Mode                = USART_Mode_Rx,
	},
	.irq = {
		.handler = PIOS_USART_spektrum_irq_handler,
		.init = {
			.NVIC_IRQChannel                   = PIOS_IRQC_SPEKTRUM,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		  },
	},
	.rx = {
		.gpio = PIOS_GPIO_SPEKTRUM,
		.init = {
			.GPIO_Pin   = PIOS_RXIO_SPEKTRUM,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_IPU,
		},
	},
	.tx = {
		.gpio = PIOS_GPIO_SPEKTRUM,
		.init = {
			.GPIO_Pin   = PIOS_TXIO_SPEKTRUM,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_IN_FLOATING,
		},
	},
};

#include <pios_spektrum_priv.h>
static uint32_t pios_usart_spektrum_id;
void PIOS_USART_spektrum_irq_handler(void)
{
	PIOS_SPEKTRUM_irq_handler(pios_usart_spektrum_id);
}

void RTC_IRQHandler();
void RTC_IRQHandler() __attribute__ ((alias ("PIOS_SUPV_irq_handler")));
const struct pios_spektrum_cfg pios_spektrum_cfg = {
	.pios_usart_spektrum_cfg = &pios_usart_spektrum_cfg,
	.gpio_init = { //used for bind feature
		.GPIO_Mode = GPIO_Mode_Out_PP,
		.GPIO_Speed = GPIO_Speed_2MHz,
	},
	.remap = 0,
	.irq = {
		.handler = RTC_IRQHandler,
		.init    = {
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.port = PIOS_GPIO_SPEKTRUM,
	.pin = PIOS_RXIO_SPEKTRUM,
};

void PIOS_SUPV_irq_handler() {
	if (RTC_GetITStatus(RTC_IT_SEC))
	{
		/* Call the right handler */
		PIOS_SPEKTRUMSV_irq_handler(pios_usart_spektrum_id);

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);
	}
}
#endif	/* PIOS_INCLUDE_SPEKTRUM */

#if defined(PIOS_INCLUDE_SBUS)
/*
 * SBUS USART
 */
void PIOS_USART_sbus_irq_handler(void);
void PIOS_IRQH_SBUS() __attribute__ ((alias ("PIOS_USART_sbus_irq_handler")));
const struct pios_usart_cfg pios_usart_sbus_cfg = {
	.regs = PIOS_USART_SBUS,
	.init = {
	#if defined (PIOS_COM_SBUS_BAUDRATE)
		.USART_BaudRate        = PIOS_COM_SBUS_BAUDRATE,
	#else
		.USART_BaudRate        = 100000,
	#endif
		.USART_WordLength          = USART_WordLength_8b,
		.USART_Parity              = USART_Parity_Even,
		.USART_StopBits            = USART_StopBits_2,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
		.USART_Mode                = USART_Mode_Rx,
	},
	.irq = {
		.handler = PIOS_USART_sbus_irq_handler,
		.init = {
			.NVIC_IRQChannel                   = PIOS_IRQC_SBUS,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		  },
	},
	.rx = {
		.gpio = PIOS_GPIO_SBUS,
		.init = {
			.GPIO_Pin   = PIOS_RXIO_SBUS,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_IPU,
		},
	},
	.tx = {
		.gpio = PIOS_GPIO_SBUS,
		.init = {
			.GPIO_Pin   = PIOS_TXIO_SBUS,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_IN_FLOATING,
		},
	},
};

static uint32_t pios_usart_sbus_id;
void PIOS_USART_sbus_irq_handler(void)
{
	PIOS_SBUS_irq_handler(pios_usart_sbus_id);
}

#include <pios_sbus_priv.h>
void RTC_IRQHandler();
void RTC_IRQHandler() __attribute__ ((alias ("PIOS_SUPV_irq_handler")));
const struct pios_sbus_cfg pios_sbus_cfg = {
	/* USART configuration structure */
	.pios_usart_sbus_cfg = &pios_usart_sbus_cfg,

	/* Invertor configuration */
	.gpio_clk_func = RCC_APB2PeriphClockCmd,
	.gpio_clk_periph = RCC_APB2Periph_GPIOB,
	.gpio_inv_port = GPIOB,
	.gpio_inv_init = {
		.GPIO_Pin = GPIO_Pin_2,
		.GPIO_Mode = GPIO_Mode_Out_PP,
		.GPIO_Speed = GPIO_Speed_2MHz,
	},
	.gpio_inv_enable = Bit_SET,
};

void PIOS_SUPV_irq_handler() {
	if (RTC_GetITStatus(RTC_IT_SEC))
	{
		/* Call the right handler */
		PIOS_SBUSSV_irq_handler(pios_usart_sbus_id);

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);
	}
}
#endif	/* PIOS_INCLUDE_SBUS */

#if defined(PIOS_INCLUDE_TELEMETRY_RF)
static uint32_t pios_usart_telem_rf_id;
void PIOS_USART_telem_irq_handler(void)
{
	PIOS_USART_IRQ_Handler(pios_usart_telem_rf_id);
}
#endif /* PIOS_INCLUDE_TELEMETRY_RF */

#if defined(PIOS_INCLUDE_GPS)
static uint32_t pios_usart_gps_id;
void PIOS_USART_gps_irq_handler(void)
{
	PIOS_USART_IRQ_Handler(pios_usart_gps_id);
}
#endif	/* PIOS_INCLUDE_GPS */

#endif	/* PIOS_INCLUDE_USART */

#if defined(PIOS_INCLUDE_COM)

#include "pios_com_priv.h"

#endif	/* PIOS_INCLUDE_COM */

/* 
 * Servo outputs 
 */
#include <pios_servo_priv.h>
const struct pios_servo_channel pios_servo_channels[] = {
	{
		.timer = TIM4,
		.port = GPIOB,
		.channel = TIM_Channel_4,
		.pin = GPIO_Pin_9,
	}, 
	{
		.timer = TIM4,
		.port = GPIOB,
		.channel = TIM_Channel_3,
		.pin = GPIO_Pin_8,
	}, 
	{
		.timer = TIM4,
		.port = GPIOB,
		.channel = TIM_Channel_2,
		.pin = GPIO_Pin_7,
	}, 
	{
		.timer = TIM1,
		.port = GPIOA,
		.channel = TIM_Channel_1,
		.pin = GPIO_Pin_8,
	}, 
	{ /* needs to remap to alternative function */
		.timer = TIM3,
		.port = GPIOB,
		.channel = TIM_Channel_1,
		.pin = GPIO_Pin_4,
	},  	
	{
		.timer = TIM2,
		.port = GPIOA,
		.channel = TIM_Channel_3,
		.pin = GPIO_Pin_2,
	},	
};

const struct pios_servo_cfg pios_servo_cfg = {
	.tim_base_init = {
		.TIM_Prescaler = (PIOS_MASTER_CLOCK / 1000000) - 1,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = ((1000000 / PIOS_SERVO_UPDATE_HZ) - 1),
		.TIM_RepetitionCounter = 0x0000,
	},
	.tim_oc_init = {
		.TIM_OCMode = TIM_OCMode_PWM1,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Disable,
		.TIM_Pulse = PIOS_SERVOS_INITIAL_POSITION,		
		.TIM_OCPolarity = TIM_OCPolarity_High,
		.TIM_OCNPolarity = TIM_OCPolarity_High,
		.TIM_OCIdleState = TIM_OCIdleState_Reset,
		.TIM_OCNIdleState = TIM_OCNIdleState_Reset,
	},
	.gpio_init = {
		.GPIO_Mode = GPIO_Mode_AF_PP,
		.GPIO_Speed = GPIO_Speed_2MHz,
	},
	.remap = GPIO_PartialRemap_TIM3,
	.channels = pios_servo_channels,
	.num_channels = NELEMENTS(pios_servo_channels),
};


/* 
 * PWM Inputs 
 */
#if defined(PIOS_INCLUDE_PWM)
#include <pios_pwm_priv.h>
const struct pios_pwm_channel pios_pwm_channels[] = {
	{
		.timer = TIM4,
		.port = GPIOB,
		.ccr = TIM_IT_CC1,
		.channel = TIM_Channel_1,
		.pin = GPIO_Pin_6,
	}, 
	{
		.timer = TIM3,
		.port = GPIOB,
		.ccr = TIM_IT_CC2,
		.channel = TIM_Channel_2,
		.pin = GPIO_Pin_5,
	}, 
	{
		.timer = TIM3,
		.port = GPIOB,
		.ccr = TIM_IT_CC3,
		.channel = TIM_Channel_3,
		.pin = GPIO_Pin_0
	}, 
	{
		.timer = TIM3,
		.port = GPIOB,
		.ccr = TIM_IT_CC4,
		.channel = TIM_Channel_4,
		.pin = GPIO_Pin_1,
	}, 
	{ 
		.timer = TIM2,
		.port = GPIOA,
		.ccr = TIM_IT_CC1,
		.channel = TIM_Channel_1,
		.pin = GPIO_Pin_0,
	},  	
	{
		.timer = TIM2,
		.port = GPIOA,
		.ccr = TIM_IT_CC2,
		.channel = TIM_Channel_2,
		.pin = GPIO_Pin_1,
	}, 		
};

void TIM2_IRQHandler();
void TIM3_IRQHandler();
void TIM4_IRQHandler();
void TIM2_IRQHandler() __attribute__ ((alias ("PIOS_TIM2_irq_handler")));
void TIM3_IRQHandler() __attribute__ ((alias ("PIOS_TIM3_irq_handler")));
void TIM4_IRQHandler() __attribute__ ((alias ("PIOS_TIM4_irq_handler")));
const struct pios_pwm_cfg pios_pwm_cfg = {
	.tim_base_init = {
		.TIM_Prescaler = (PIOS_MASTER_CLOCK / 1000000) - 1,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = 0xFFFF,
		.TIM_RepetitionCounter = 0x0000,
	},
	.tim_ic_init = {
		.TIM_ICPolarity = TIM_ICPolarity_Rising,
		.TIM_ICSelection = TIM_ICSelection_DirectTI,
		.TIM_ICPrescaler = TIM_ICPSC_DIV1,
		.TIM_ICFilter = 0x0,		
	},
	.gpio_init = {
		.GPIO_Mode = GPIO_Mode_IPD,
		.GPIO_Speed = GPIO_Speed_2MHz,
	},
	.remap = 0,
	.irq = {
		.handler = TIM2_IRQHandler,
		.init    = {
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.channels = pios_pwm_channels,
	.num_channels = NELEMENTS(pios_pwm_channels),
};
void PIOS_TIM2_irq_handler()
{
	PIOS_PWM_irq_handler(TIM2);
}
void PIOS_TIM3_irq_handler()
{
	PIOS_PWM_irq_handler(TIM3);
}
void PIOS_TIM4_irq_handler()
{
	PIOS_PWM_irq_handler(TIM4);
}
#endif

#if defined(PIOS_INCLUDE_I2C)

#include <pios_i2c_priv.h>

/*
 * I2C Adapters
 */

void PIOS_I2C_main_adapter_ev_irq_handler(void);
void PIOS_I2C_main_adapter_er_irq_handler(void);
void I2C2_EV_IRQHandler() __attribute__ ((alias ("PIOS_I2C_main_adapter_ev_irq_handler")));
void I2C2_ER_IRQHandler() __attribute__ ((alias ("PIOS_I2C_main_adapter_er_irq_handler")));

const struct pios_i2c_adapter_cfg pios_i2c_main_adapter_cfg = {
  .regs = I2C2,
  .init = {
    .I2C_Mode                = I2C_Mode_I2C,
    .I2C_OwnAddress1         = 0,
    .I2C_Ack                 = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
    .I2C_DutyCycle           = I2C_DutyCycle_2,
    .I2C_ClockSpeed          = 400000,	/* bits/s */
  },
  .transfer_timeout_ms = 50,
  .scl = {
    .gpio = GPIOB,
    .init = {
      .GPIO_Pin   = GPIO_Pin_10,
      .GPIO_Speed = GPIO_Speed_10MHz,
      .GPIO_Mode  = GPIO_Mode_AF_OD,
    },
  },
  .sda = {
    .gpio = GPIOB,
    .init = {
      .GPIO_Pin   = GPIO_Pin_11,
      .GPIO_Speed = GPIO_Speed_10MHz,
      .GPIO_Mode  = GPIO_Mode_AF_OD,
    },
  },
  .event = {
    .handler = PIOS_I2C_main_adapter_ev_irq_handler,
    .flags   = 0,		/* FIXME: check this */
    .init = {
      .NVIC_IRQChannel                   = I2C2_EV_IRQn,
      .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
      .NVIC_IRQChannelSubPriority        = 0,
      .NVIC_IRQChannelCmd                = ENABLE,
    },
  },
  .error = {
    .handler = PIOS_I2C_main_adapter_er_irq_handler,
    .flags   = 0,		/* FIXME: check this */
    .init = {
      .NVIC_IRQChannel                   = I2C2_ER_IRQn,
      .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
      .NVIC_IRQChannelSubPriority        = 0,
      .NVIC_IRQChannelCmd                = ENABLE,
    },
  },
};

uint32_t pios_i2c_main_adapter_id;
void PIOS_I2C_main_adapter_ev_irq_handler(void)
{
  /* Call into the generic code to handle the IRQ for this specific device */
  PIOS_I2C_EV_IRQ_Handler(pios_i2c_main_adapter_id);
}

void PIOS_I2C_main_adapter_er_irq_handler(void)
{
  /* Call into the generic code to handle the IRQ for this specific device */
  PIOS_I2C_ER_IRQ_Handler(pios_i2c_main_adapter_id);
}

#endif /* PIOS_INCLUDE_I2C */

#if defined(PIOS_INCLUDE_RCVR)
#include "pios_rcvr_priv.h"

uint32_t pios_rcvr_channel_to_id_map[PIOS_RCVR_MAX_DEVS];
uint32_t pios_rcvr_max_channel;
#endif /* PIOS_INCLUDE_RCVR */

extern const struct pios_com_driver pios_usb_com_driver;

uint32_t pios_com_telem_rf_id;
uint32_t pios_com_telem_usb_id;
uint32_t pios_com_gps_id;
uint32_t pios_com_spektrum_id;
uint32_t pios_com_sbus_id;

/**
 * PIOS_Board_Init()
 * initializes all the core subsystems on this specific hardware
 * called from System/openpilot.c
 */
void PIOS_Board_Init(void) {

	/* Delay system */
	PIOS_DELAY_Init();	
	
	/* Set up the SPI interface to the serial flash */
	if (PIOS_SPI_Init(&pios_spi_flash_accel_id, &pios_spi_flash_accel_cfg)) {
		PIOS_DEBUG_Assert(0);
	}

	PIOS_Flash_W25X_Init(pios_spi_flash_accel_id);	
	PIOS_ADXL345_Attach(pios_spi_flash_accel_id);
	
	PIOS_FLASHFS_Init();

	/* Initialize UAVObject libraries */
	EventDispatcherInitialize();
	UAVObjInitialize();
	UAVObjectsInitializeAll();

	/* Initialize the alarms library */
	AlarmsInitialize();

	/* Initialize the task monitor library */
	TaskMonitorInitialize();

	/* Initialize the PiOS library */
#if defined(PIOS_INCLUDE_COM)
#if defined(PIOS_INCLUDE_TELEMETRY_RF)
	if (PIOS_USART_Init(&pios_usart_telem_rf_id, &pios_usart_telem_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	if (PIOS_COM_Init(&pios_com_telem_rf_id, &pios_usart_com_driver, pios_usart_telem_rf_id)) {
		PIOS_DEBUG_Assert(0);
	}
#endif /* PIOS_INCLUDE_TELEMETRY_RF */
#if defined(PIOS_INCLUDE_GPS)
	if (PIOS_USART_Init(&pios_usart_gps_id, &pios_usart_gps_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	if (PIOS_COM_Init(&pios_com_gps_id, &pios_usart_com_driver, pios_usart_gps_id)) {
		PIOS_DEBUG_Assert(0);
	}
#endif	/* PIOS_INCLUDE_GPS */
#if defined(PIOS_INCLUDE_SPEKTRUM)
	/* SPEKTRUM init must come before comms */
	PIOS_SPEKTRUM_Init();

	if (PIOS_USART_Init(&pios_usart_spektrum_id, &pios_usart_spektrum_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	if (PIOS_COM_Init(&pios_com_spektrum_id, &pios_usart_com_driver, pios_usart_spektrum_id)) {
		PIOS_DEBUG_Assert(0);
	}
	for (uint8_t i = 0; i < PIOS_SPEKTRUM_NUM_INPUTS; i++) {
		if (!PIOS_RCVR_Init(&pios_rcvr_channel_to_id_map[pios_rcvr_max_channel],
			   &pios_spektrum_rcvr_driver,
			   i)) {
			pios_rcvr_max_channel++;
		} else {
			PIOS_DEBUG_Assert(0);
		}
	}
#endif
#if defined(PIOS_INCLUDE_SBUS)
	PIOS_SBUS_Init();

	if (PIOS_USART_Init(&pios_usart_sbus_id, &pios_usart_sbus_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
	if (PIOS_COM_Init(&pios_com_sbus_id, &pios_usart_com_driver, pios_usart_sbus_id)) {
		PIOS_DEBUG_Assert(0);
	}
	for (uint8_t i = 0; i < SBUS_NUMBER_OF_CHANNELS; i++) {
		if (!PIOS_RCVR_Init(&pios_rcvr_channel_to_id_map[pios_rcvr_max_channel],
			   &pios_sbus_rcvr_driver,
			   i)) {
			pios_rcvr_max_channel++;
		} else {
			PIOS_DEBUG_Assert(0);
		}
	}

#endif  /* PIOS_INCLUDE_SBUS */
#endif  /* PIOS_INCLUDE_COM */

	/* Remap AFIO pin */
	GPIO_PinRemapConfig( GPIO_Remap_SWJ_NoJTRST, ENABLE);
	PIOS_Servo_Init();
	
	PIOS_ADC_Init();
	PIOS_GPIO_Init();

#if defined(PIOS_INCLUDE_PWM)
#if (PIOS_PWM_NUM_INPUTS > PIOS_RCVR_MAX_DEVS)
#error More receiver inputs than available devices
#endif
	PIOS_PWM_Init();
	for (uint8_t i = 0; i < PIOS_PWM_NUM_INPUTS; i++) {
		if (!PIOS_RCVR_Init(&pios_rcvr_channel_to_id_map[pios_rcvr_max_channel],
			   &pios_pwm_rcvr_driver,
			   i)) {
			pios_rcvr_max_channel++;
		} else {
			PIOS_DEBUG_Assert(0);
		}
	}
#endif
#if defined(PIOS_INCLUDE_PPM)
#if (PIOS_PPM_NUM_INPUTS > PIOS_RCVR_MAX_DEVS)
#error More receiver inputs than available devices
#endif
	PIOS_PPM_Init();
	for (uint8_t i = 0; i < PIOS_PPM_NUM_INPUTS; i++) {
		if (!PIOS_RCVR_Init(&pios_rcvr_channel_to_id_map[pios_rcvr_max_channel],
			   &pios_ppm_rcvr_driver,
			   i)) {
			pios_rcvr_max_channel++;
		} else {
			PIOS_DEBUG_Assert(0);
		}
	}
#endif
#if defined(PIOS_INCLUDE_USB_HID)
	PIOS_USB_HID_Init(0);
#if defined(PIOS_INCLUDE_COM)
	if (PIOS_COM_Init(&pios_com_telem_usb_id, &pios_usb_com_driver, 0)) {
		PIOS_DEBUG_Assert(0);
	}
#endif	/* PIOS_INCLUDE_COM */
#endif

#if defined(PIOS_INCLUDE_I2C)
	if (PIOS_I2C_Init(&pios_i2c_main_adapter_id, &pios_i2c_main_adapter_cfg)) {
		PIOS_DEBUG_Assert(0);
	}
#endif	/* PIOS_INCLUDE_I2C */
	PIOS_IAP_Init();
	PIOS_WDG_Init();
}

/**
 * @}
 */
