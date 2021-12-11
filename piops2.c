#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"

#include "piops2.pio.h"

#define PS2_INPUT_PIN_BASE 16

int main() {
    u_int32_t rxdata;
    u_int8_t  code;
    stdio_init_all();

    PIO pio = pio1;

    uint offset = pio_add_program(pio, &readps2_program);
    uint sm = pio_claim_unused_sm(pio, true);
    readps2_program_init(pio, sm, offset, PS2_INPUT_PIN_BASE);

    printf("Loaded program at %d\n", offset);

    uint offset2 = pio_add_program(pio, &pinstate_program);
    uint sm2 = pio_claim_unused_sm(pio, true);
    pinstate_program_init(pio, sm2, offset2, PS2_INPUT_PIN_BASE);

    uint offset3 = pio_add_program(pio, &writeps2_program);
    uint sm3 = pio_claim_unused_sm(pio, true);
    writeps2_program_init(pio, sm3, offset3, PS2_INPUT_PIN_BASE);

    sleep_ms(2500);

    pio_sm_put_blocking(pio, sm2, 2);
    sleep_ms(10);
    pio_sm_put_blocking(pio, sm3, 511);
//    sleep_us(2);
    pio_sm_put_blocking(pio, sm2, 1);
    sleep_us(100);
    pio_sm_put_blocking(pio, sm2, 3);

    printf("I got here!\n");
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
        if (code == 0xbe) {
            pio_sm_put_blocking(pio, sm2, 1);
            sleep_us(100);
            pio_sm_put_blocking(pio, sm2, 2);
            sleep_ms(10);
            pio_sm_put_blocking(pio, sm3, 496);
//            sleep_us(2);
            pio_sm_put_blocking(pio, sm2, 1);
            sleep_us(100);
            pio_sm_put_blocking(pio, sm2, 3);
        }
    }

}
