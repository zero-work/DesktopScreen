/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"


//#define PIN_NUM_MISO 1  //屏幕只写不读，此引脚不用
#define PIN_NUM_MOSI 26
#define PIN_NUM_CLK  25
#define PIN_NUM_CS   27
//屏幕数据/指令选择 1-data 0-cmd
#define PIN_NUM_DC 14
#define SCREEN_GPIO_OUTPUT_DC_SEL ((1ULL<<SCREEN_GPIO_OUTPUT_DC))
//屏幕复位 0-reset
#define PIN_NUM_RES 12
#define SCREEN_GPIO_OUTPUT_RES_SEL ((1ULL<<SCREEN_GPIO_OUTPUT_RES))
//屏幕状态 1-busy 
#define PIN_NUM_BUSY 13
#define SCREEN_GPIO_INTPUT_BUSY_SEL ((1ULL<<SCREEN_GPIO_INTPUT_BUSY))


static const char *TAG = "DeskTopScreen_SPI";

void spi_send_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    //设置发送指令
    ds_gpio_set_screen_dc(0);
    //设置片选模块
    ds_gpio_set_screen_cs(0);
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    // t.flags=SPI_TRANS_USE_TXDATA;
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    //发送指令
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    //取消片选模块
    ds_gpio_set_screen_cs(1);
    assert(ret==ESP_OK);            //Should have had no issues.
}

void spi_send_data(const uint8_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    //设置发送数据
    ds_gpio_set_screen_dc(1);
    //设置片选模块
    ds_gpio_set_screen_cs(0);
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=&data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    //发送数据
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    //取消片选模块
    ds_gpio_set_screen_cs(1);
    assert(ret==ESP_OK);            //Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    printf("dc callback\n");
    ds_gpio_set_screen_dc(dc);
}

void screen_spi_init(void)
{
    esp_err_t ret = ESP_OK;
    
    ESP_LOGD(TAG, "Init SPI")
    spi_bus_config_t buscfg={
        .miso_io_num = -1,                // MISO信号线
        .mosi_io_num = PIN_NUM_MOSI,                // MOSI信号线
        .sclk_io_num = PIN_NUM_CLK,                 // SCLK信号线
        .quadwp_io_num = -1,                        // WP信号线，专用于QSPI的D2
        .quadhd_io_num = -1,                        // HD信号线，专用于QSPI的D3
        .max_transfer_sz = 64*8,                    // 最大传输数据大小
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");
    
    
    //速率 模式设置
    ESP_LOGD(TAG, "Install panel IO");
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=15*1000*1000,            //Clock out at 26 MHz
        .mode=0,                                //SPI mode 0
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        // .pre_cb=spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    
}

void screen_spi_test(void){
    spi_send_cmd(0x55);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    spi_send_data(0x55);
}