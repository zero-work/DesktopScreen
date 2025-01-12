#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "ds_spi.h"

void app_main(void)
{

    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    esp_err_t err = lvgl_port_init(&lvgl_cfg);


   static lv_disp_t * disp_handle;

    /* LCD IO */
    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t) 1, &io_config, &io_handle));

    /* LCD driver initialization */
    esp_lcd_panel_handle_t lcd_panel_handle;
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &lcd_panel_handle));

    /* Add LCD screen */
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = lcd_panel_handle,
        .buffer_size = DISP_WIDTH*DISP_HEIGHT,
        .double_buffer = true,
        .hres = DISP_WIDTH,
        .vres = DISP_HEIGHT,
        .monochrome = false,
        .mipi_dsi = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
            .swap_bytes = false,
        }
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);

    /* ... the rest of the initialization ... */

    /* If deinitializing LVGL port, remember to delete all displays: */
    lvgl_port_remove_disp(disp_handle);

}
