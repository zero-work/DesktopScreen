#ifndef _DS_SPI_H_
#define _DS_SPI_H_

void spi_send_data(const uint8_t data);
void spi_send_cmd(const uint8_t cmd);
void screen_spi_init(void);
void spi_send_datas(const uint8_t *data, size_t length);
esp_lcd_panel_handle_t epd_init(void);

#endif