#pragma once

#include "main.h"

/**********************
 *   driver selection
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Init script function */
struct lcd_function {
	uint16_t cmd;
	uint16_t data;
};

/* Init script commands */
enum lcd_cmd {
	LCD_START,
	LCD_END,
	LCD_CMD,
	LCD_DATA,
	LCD_DELAY
};

/*********************
 * DELAY INTERFACE
 *********************/
#define LCD_DRV_DELAY_INCLUDE  "main.h"     /*Dummy include by default*/
#define LCD_DRV_DELAY_US(us)  /*delay_us(us)*/       /*Delay the given number of microseconds*/
#define LCD_DRV_DELAY_MS(ms)  HAL_Delay(ms)          /*Delay the given number of milliseconds*/

/*********************
 * DISPLAY INTERFACE
 *********************/

/*------------
 *  Common
 *------------*/
#define LCD_DRV_DISP_INCLUDE        "main.h"
#define LCD_DRV_DISP_CMD_DATA(val)  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, val)
#define LCD_DRV_DISP_RST(val)       HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, val)
#define LCD_DRV_DISP_BLK(val)		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, val)

/*---------
 *  SPI
 *---------*/
#define LCD_DRV_DISP_SPI_CS(val)          HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, val);
#define LCD_DRV_DISP_SPI_WR_BYTE(data)    HAL_SPI_Transmit(&hspi1, &data, 1, 10);
#define LCD_DRV_DISP_SPI_WR_ARRAY(adr, n) HAL_SPI_Transmit(&hspi1, adr, n, 10);

/*------------------
 *  Parallel port
 *-----------------*/
#define LCD_DRV_DISP_PAR_CS(val)          /*par_cs_set(val)*/   /*Set the Parallel port's Chip select to 'val'*/
#define LCD_DRV_DISP_PAR_SLOW             /*par_slow()*/        /*Set low speed on the parallel port*/
#define LCD_DRV_DISP_PAR_FAST             /*par_fast()*/        /*Set high speed on the parallel port*/
#define LCD_DRV_DISP_PAR_WR_WORD(data)    /*par_wr(data)*/      /*Write a word to the parallel port*/
#define LCD_DRV_DISP_PAR_WR_ARRAY(adr, n) /*par_wr_mem(adr,n)*/ /*Write 'n' bytes to Parallel ports from 'adr'*/

/***************************
 * INPUT DEVICE INTERFACE
 ***************************/

/*----------
 *  Common
 *----------*/
#define LCD_DRV_INDEV_INCLUDE     <stdint.h>             /*Dummy include by default*/
#define LCD_DRV_INDEV_RST(val)    /*pin_x_set(val)*/     /*Set the reset pin to 'val'*/
#define LCD_DRV_INDEV_IRQ_READ    0 /*pn_x_read()*/      /*Read the IRQ pin*/

/*---------
 *  SPI
 *---------*/
#define LCD_DRV_INDEV_SPI_CS(val)            /*spi_cs_set(val)*/     /*Set the SPI's Chip select to 'val'*/
#define LCD_DRV_INDEV_SPI_XCHG_BYTE(data)    0 /*spi_xchg(val)*/     /*Write 'val' to SPI and give the read value*/

/*---------
 *  I2C
 *---------*/
#define LCD_DRV_INDEV_I2C_START              /*i2c_start()*/       /*Make an I2C start*/
#define LCD_DRV_INDEV_I2C_STOP               /*i2c_stop()*/        /*Make an I2C stop*/
#define LCD_DRV_INDEV_I2C_RESTART            /*i2c_restart()*/     /*Make an I2C restart*/
#define LCD_DRV_INDEV_I2C_WR(data)           /*i2c_wr(data)*/      /*Write a byte to the I1C bus*/
#define LCD_DRV_INDEV_I2C_READ(last_read)    0 /*i2c_rd()*/        /*Read a byte from the I2C bud*/
