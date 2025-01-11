#ifndef _DS_SPI_H_
#define _DS_SPI_H_

void spi_send_data(const uint8_t data);
void spi_send_cmd(const uint8_t cmd);
void screen_spi_init(void);
void spi_pre_transfer_callback(spi_transaction_t *t)
esp_lcd_panel_handle_t epd_init(void);
void screen_spi_test(void);


#endif