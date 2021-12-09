#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "piops2.pio.h"

#define PS2_INPUT_PIN_BASE 16

int main() {
    stdio_init_all();

    PIO pio = pio1;

    uint offset = pio_add_program(pio, &readps2_program);
    uint sm = pio_claim_unused_sm(pio, true);
    readps2_program_init(pio, sm, offset, PS2_INPUT_PIN_BASE);

    printf("Loaded program at %d\n", offset);

    while (1) {
        u_int32_t rxdata;
        u_int8_t  code;
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
