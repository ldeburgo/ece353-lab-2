#include <gme_midimsg.h>
#include <gme_error.h>

#include <stdlib.h>

int is_start_byte(uint8_t data) {
    // check the MSB of the data
    return data & 0x80;
}

int is_data_byte(uint8_t data) {
    // check the MSB of the data
    return ~data & 0x80;
}

void midi_err_check(MidiMsg *msg) {
    if (!is_start_byte(msg->byte1) ||
            !is_data_byte(msg->byte2) ||
            !is_data_byte(msg->byte3)) {
        log_error(USART_READ_ERR);
        exit(USART_READ_ERR);
    }
}
