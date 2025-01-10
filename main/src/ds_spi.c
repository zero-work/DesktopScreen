
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
#include "hal/lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#include "ds_gpio.h"


#define DMA_CHAN    2

// #define PIN_NUM_MISO 0
// #define PIN_NUM_MOSI 5
// #define PIN_NUM_CLK  25
// #define PIN_NUM_CS   27

#define PIN_NUM_MISO 23
#define PIN_NUM_MOSI 26
#define PIN_NUM_CLK  25
#define PIN_NUM_CS   27

//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define PARALLEL_LINES 16

spi_device_handle_t spi;

void spi_send_datas(const uint8_t *data, size_t length) 
{

    esp_err_t ret;
    spi_transaction_t t;

    ds_gpio_set_screen_dc(1);  // 设置为数据模式
    ds_gpio_set_screen_cs(0);  // 拉低片选引脚

    memset(&t, 0, sizeof(t));
    t.length = length * 8;    // 设置传输长度，单位为 bit
     if(!data){
        t.tx_buffer = data;       // 设置数据缓冲区指针
    }
    

    ret = spi_device_polling_transmit(spi, &t);  // 发送数据
    ds_gpio_set_screen_cs(1);  // 拉高片选引脚

    assert(ret == ESP_OK);     // 确保传输成功
}


void spi_send_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    ds_gpio_set_screen_dc(0);
    ds_gpio_set_screen_cs(0);
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    // t.flags=SPI_TRANS_USE_TXDATA;
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    ds_gpio_set_screen_cs(1);
    assert(ret==ESP_OK);            //Should have had no issues.
}


void spi_send_data(const uint8_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    ds_gpio_set_screen_dc(1);
    ds_gpio_set_screen_cs(0);
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=&data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
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

/// @brief SPI 接口初始化操作
/// @param  
void screen_spi_init(void)
{
    // 错误信息初始化
    esp_err_t ret;

    // 初始化GPIO接口
    ds_screen_gpio_init();

    // 总线初始化结构体
    spi_bus_config_t buscfg={
        .miso_io_num = PIN_NUM_MISO,                // MISO信号线
        .mosi_io_num = PIN_NUM_MOSI,                // MOSI信号线
        .sclk_io_num = PIN_NUM_CLK,                 // SCLK信号线
        .quadwp_io_num = -1,                        // WP信号线，专用于QSPI的D2，无用设置为-1
        .quadhd_io_num = -1,                        // HD信号线，专用于QSPI的D3，无用设置为-1
        .max_transfer_sz = 64*8,                    // 最大传输数据大小
        .flags = SPICOMMON_BUSFLAG_MASTER         //这个用于设置初始化的时候要检测哪些选项。比如这里设置的是spi初始化为主机模式是否成功。
    };


    // 设备初始化结构体
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=15*1000*1000,            //Clock out at 26 MHz
        .mode=0,                                //SPI mode 0
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        // .pre_cb=spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };

    esp_lcd_panel_io_handle_t io_handle = NULL;


    // 初始化SPI总线
    ret=spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);

    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    
}

esp_lcd_panel_handle_t epd_init(void) {
    // SPI 面板 IO 配置
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = SCREEN_GPIO_OUTPUT_DC,
        .cs_gpio_num = SCREEN_GPIO_OUTPUT_CS,
        .pclk_hz = 4000000, // SPI 时钟频率
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };

    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &io_handle));

    // 初始化墨水屏面板
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = SCREEN_GPIO_OUTPUT_RES,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 1,  // 墨水屏通常是单色显示
    };
    // ESP_ERROR_CHECK(esp_lcd_new_panel_epaper(io_handle, &panel_config, &panel_handle));

    // 复位和初始化面板
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    return panel_handle;
}
