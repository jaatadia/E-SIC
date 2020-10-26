#include "interruption.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#define GPIO_INPUT_IO_0     16  // Entrada en GPIO 16  → BOTON
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

extern int64_t timeRequest;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    timeRequest = esp_timer_get_time();
}


void setup_int_ext(){

    gpio_config_t io_conf;

    //interrupt of rising edge
    //io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;

    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
}
