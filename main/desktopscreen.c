#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_lcd_panel_ops.h"

#include "lvgl.h"


#include "ds_gpio.h"
#include "ds_i2c.h"
#include "ds_spi.h"


// #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_I2))

// 声明全局变量
esp_lcd_panel_handle_t panel_handle = NULL;

void my_disp_flush( lv_disp_t *disp_drv, const lv_area_t *area, lv_color_t *color_map) 
{
    // 将 LVGL 缓冲区的数据发送到墨水屏
    spi_send_cmd(0x24); // 假设 0x24 是写入数据命令
    spi_send_datas((uint8_t *)color_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
    lv_disp_flush_ready(disp_drv);
}

void lvgl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_map) {
    // 等待墨水屏空闲
    while (gpio_get_level(SCREEN_GPIO_INTPUT_BUSY)) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // 调用屏幕接口刷新区域
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);

    // 通知 LVGL 刷新完成
    lv_disp_flush_ready(disp_drv);
}

void create_main_page(void) {
    lv_obj_t *scr = lv_scr_act();  // 获取活动屏幕

    // 创建一个标签
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello, LVGL!");  // 设置标签文本
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -50); // 居中对齐

    // 创建一个按钮
    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50); // 按钮居中
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Press Me!");  // 按钮上的文字
}


void app_main(void)
{
    // 初始化 SPI 和 I2C
    screen_spi_init();
    // i2c_master_init();

    // 初始化墨水屏
    panel_handle = epd_init();
    
    // 初始化 LVGL
    lv_init();

    // 显示缓冲区
    static lv_disp_draw_buf_t disp_buf;
    lv_color_t *buf1 = heap_caps_malloc(LV_HOR_RES * 20, MALLOC_CAP_DMA);
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, LV_VER_RES * 20);


    // 注册显示驱动
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES;
    disp_drv.ver_res = LV_VER_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    

    // 创建 LVGL 界面
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
    // 主循环
    while (1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}
