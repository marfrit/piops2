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
    u_int32_t out;
    u_int8_t  code;
    stdio_init_all();

    PIO pio = pio1;

    uint offset1 = pio_add_program(pio, &readps2_program);
    uint sm1 = pio_claim_unused_sm(pio, true);
    uint offset3 = pio_add_program(pio, &setpins_program);
    uint sm3 = pio_claim_unused_sm(pio, true);
    uint offset4 = pio_add_program(pio, &writeps2_program);
    uint sm4 = pio_claim_unused_sm(pio, true);

    readps2_program_init (pio, sm1, offset1, DATA);
    setpins_program_init (pio, sm3, offset3, DATA);
    writeps2_program_init(pio, sm4, offset4, DATA);

    pio->irq_force = 2;

    sleep_ms(2500);
    pio->irq_force = 4;
    pio_sm_put_blocking(pio, sm3, 1);
    sleep_us(200);
    pio->irq_force = 4;
    pio_sm_put_blocking(pio, sm3, 3);
    sleep_us(200);
    pio->irq_force = 4;
    pio_sm_put_blocking(pio, sm3, 2);
    printf("bin drin\n");

    out = 0xf8;  // Data to write 
    out = out | (oddParity(out) << 8); // add parity to the front of data since we right shift on output
    out = 511 & (~ out);               // invert value to fit pio out pindirs syntax

    pio->irq_force = 8;
    pio_sm_put_blocking(pio, sm4, out);
    printf("bin raus 0x%04x\n", out);

    pio->irq_force = 1;
    while (1) {
        if (pio_sm_get_rx_fifo_level(pio, sm1) > 0) {
            rxdata = pio_sm_get_blocking(pio, sm1);
            pio->irq_force = 1;
        } else {
            rxdata = 0;
        }
        if (rxdata) {
            code = (rxdata >> 22) & 0xff;
            printf("received 0x%02x\n", code);
        }
    }

}
