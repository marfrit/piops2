#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/structs/sio.h"

#include "piops2.pio.h"

#define DATA  16
#define CLOCK 17

bool oddParity(u_int8_t x)
{
    u_int8_t y = x ^ (x >> 1);
    y = y ^ (y >> 2);
    y = y ^ (y >> 4);
    y = y ^ (y >> 8);
//    y = y ^ (y >> 16);
 
    // Rightmost bit of y holds the parity value
    // if (y&1) is 1 then parity is odd else even
    if (y & 1)
        return 0;
    return 1;
}

void inhibit(){
}

u_int16_t ps2_host_send(u_int8_t data, uint sm2, PIO pio)
{
}

int main() {
    u_int32_t rxdata;
    u_int8_t  code;
    stdio_init_all();

//    gpio_set_function(DATA , GPIO_FUNC_SIO);
//    gpio_set_function(CLOCK, GPIO_FUNC_SIO);
//    sio_hw->gpio_oe = 3 << 15;

    PIO pio = pio1;

    uint offset = pio_add_program(pio, &readps2_program);
    uint sm = pio_claim_unused_sm(pio, true);
    readps2_program_init(pio, sm, offset, DATA);

    uint offset2 = pio_add_program(pio, &setpins_program);
//    gpio_set_pulls(16, true, false);
//    gpio_set_pulls(17, true, false);
    uint sm2 = pio_claim_unused_sm(pio, true);
    setpins_program_init(pio, sm2, offset2, DATA);

//    while (1)
//    {
//        sleep_ms(3);
//        pio_sm_put_blocking(pio, sm2, 0);
//        sleep_ms(5);
//        pio_sm_put_blocking(pio, sm2, 1);
//        sleep_ms(7);
//        pio_sm_put_blocking(pio, sm2, 2);
//        sleep_ms(11);
//        pio_sm_put_blocking(pio, sm2, 3);
//        sleep_ms(13);
//        pio_sm_put_blocking(pio, sm2, 0);
//        pio_sm_put_blocking(pio, sm2, 0);
//        sleep_ms(1000);
//    }
    

//    while (gpio_get(CLOCK))
//    {
//    }

//    for(u_int8_t i = 0; i < 10; i++) {
//        sleep_us(15);
//        pio_sm_put_blocking(pio, sm2, 1);
//        //x != x;
//        while (!gpio_get(CLOCK))
//        {
//        }
//        while (gpio_get(CLOCK))
//        {
//        }
//    }

//    pio_sm_put_blocking(pio, sm2, 0);

    printf("ready to go\n");
    while (1) {
        if (pio_sm_get_rx_fifo_level(pio, sm) > 0) {
            rxdata = pio_sm_get_blocking(pio, sm);
        } else {
            rxdata = 0;
        }
        if (rxdata) {
            code = (rxdata >> 22) & 0xff;
            printf("received 0x%02x\n", code);
        }
    }

}
