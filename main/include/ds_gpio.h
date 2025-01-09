#ifndef __DS_GPIO_H
#define __DS_GPIO_H

// 设备所需的GPIO接口

#define GPIO_OUTPUT_IO_0    5
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0))
#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

//屏幕片选 0-有效
#define SCREEN_GPIO_OUTPUT_CS 27
#define SCREEN_GPIO_OUTPUT_CS_SEL ((1ULL<<SCREEN_GPIO_OUTPUT_CS))
//屏幕数据/指令选择 1-data 0-cmd
#define SCREEN_GPIO_OUTPUT_DC 14
#define SCREEN_GPIO_OUTPUT_DC_SEL ((1ULL<<SCREEN_GPIO_OUTPUT_DC))
//屏幕复位 0-reset
#define SCREEN_GPIO_OUTPUT_RES 12
#define SCREEN_GPIO_OUTPUT_RES_SEL ((1ULL<<SCREEN_GPIO_OUTPUT_RES))
//屏幕状态 1-busy 
#define SCREEN_GPIO_INTPUT_BUSY 13
#define SCREEN_GPIO_INTPUT_BUSY_SEL ((1ULL<<SCREEN_GPIO_INTPUT_BUSY))

#define PIN_NUM_MISO 23
#define PIN_NUM_MOSI 26
#define PIN_NUM_CLK  25
#define PIN_NUM_CS   27


// GPIO的几个操作
void ds_touch_gpio_init(void);
void ds_touch_gpio_isr_remove();
void ds_touch_gpio_isr_add();
void ds_screen_gpio_init(void);
void ds_gpio_set_screen_cs(uint32_t level);
void ds_gpio_set_screen_dc(uint32_t level);
void ds_gpio_set_screen_rst(uint32_t level);
int ds_gpio_get_screen_busy(void);
void ds_gpio_set_touch_rst(uint32_t level);

#endif