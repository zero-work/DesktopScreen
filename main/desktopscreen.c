#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#include "lvgl.h"


#include "ds_gpio.h"
#include "ds_i2c.h"
#include "ds_spi.h"


// #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_I2))


/* RTOS resources */
QueueHandle_t  MusicCmd_Port;


void my_disp_flush( lv_disp_t *disp_drv, const lv_area_t *area, lv_color_t *color_map) {
    // 将 LVGL 缓冲区的数据发送到墨水屏
    spi_send_cmd(0x24); // 假设 0x24 是写入数据命令
    spi_send_datas((uint8_t *)color_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
    lv_disp_flush_ready(disp_drv);
}

void lv_port_disp_init(void) {

    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[200 * 10]; // 显示缓冲区
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LV_HOR_RES * 10);

    
    // static uint8_t buf1[200 * 200 / 10 * BYTES_PER_PIXEL];
    // lv_display_t * display1 = lv_display_create(200, 200);
    // lv_display_set_buffers(display1, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    static lv_disp_drv_t disp_drv;  // 显示驱动
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 200;         // 替换为墨水屏宽度
    disp_drv.ver_res = 200;         // 替换为墨水屏高度
    disp_drv.flush_cb = my_disp_flush; // 刷新屏幕回调
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
  
    // lv_display_set_flush_cb(display1, my_disp_flush);
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
    MusicCmd_Port = xQueueCreate(4,sizeof(int8_t));
    

    lv_init();

    screen_spi_init();
    lv_port_disp_init();

    // 创建页面
    create_main_page();
    
    esp_task_wdt_delete(NULL);

}


 