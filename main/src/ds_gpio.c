#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "ds_gpio.h"


/// @brief  
/// @param  
void ds_touch_gpio_init(void){

    static bool has_init_isr = false;
    
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //GPIO interrupt type : both rising and falling edge
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    // gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_NEGEDGE);

    if(has_init_isr == false){
    

        //install gpio isr service
        gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
        //hook isr handler for specific gpio pin
        // gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handle_t, (void*) GPIO_INPUT_IO_0);
    }
    has_init_isr = true;

}

/// @brief 
void ds_touch_gpio_isr_remove(){
    
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);

}


void ds_touch_gpio_isr_add(){
 
    // gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handle_t, (void*) GPIO_INPUT_IO_0);
 
}


void ds_screen_gpio_init(void){
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_CS_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_DC_SEL;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_RES_SEL;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = SCREEN_GPIO_INTPUT_BUSY_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    //change gpio intrrupt type for one pin
    // gpio_set_intr_type(SCREEN_GPIO_INTPUT_BUSY, GPIO_INTR_NEGEDGE);

    // //install gpio isr service
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // //hook isr handler for specific gpio pin
    // gpio_isr_handler_add(SCREEN_GPIO_INTPUT_BUSY, gpio_isr_handler, (void*) SCREEN_GPIO_INTPUT_BUSY);
}


void ds_gpio_set_screen_cs(uint32_t level){
    gpio_set_level(SCREEN_GPIO_OUTPUT_CS, level);
}

void ds_gpio_set_screen_dc(uint32_t level){
    gpio_set_level(SCREEN_GPIO_OUTPUT_DC, level);
}

void ds_gpio_set_screen_rst(uint32_t level){
    gpio_set_level(SCREEN_GPIO_OUTPUT_RES, level);
}

int ds_gpio_get_screen_busy(){
    return gpio_get_level(SCREEN_GPIO_INTPUT_BUSY);
}

void ds_gpio_set_touch_rst(uint32_t level){
    gpio_set_level(GPIO_OUTPUT_IO_0, level);
}