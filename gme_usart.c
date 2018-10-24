#include <gme_usart.h>
#include <gme_error.h>
#include <gme_midimsg.h>

#include <avr/io.h>
#include <avr/iom32.h>
#include <util/setbaud.h>
#include <interrupt.h>
#include <interrupt/interrupt_avr8.h>

#include <stdlib.h>
#include <time.h>

/**
 * // TODO
 * Find out how USART communications should work.
 * Should there be any checks in the main loop before trying to
 * * read in from USART?
 * If I don't check something in main, could it get stuck waiting for USART
 * * input?
 */

/**
 * Checks for a frame error when receiving messages.
 */
static void _frame_err_check(void);

void init_usart(void) {
    /**
     * Enable receiver and transmitter.
     */
    UCSRB |= (1 << RXEN) | (1 << TXEN);

    /**
     * Set frame format - 8 data bits, 1 stop bit, no parity.
     */
    UCSRB &= ~(1 << UCSZ2); // 8 data bits
    UCSRC |= (1 << UCSZ0) | (1 << UCSZ1); // 8 data bits
    UCSRC &= ~(1 << UPM0) & ~(1 << UPM1); // no parity
    UCSRC &= ~(1 << USBS); // 1 stop bit

    /**
     * Set mode to async.
     */
    UCSRC &= ~(1 << UMSEL);

    /**
     * Baud rate and CPU frequency are set in globals.h.
     * Uses built-in functions in <util/setbaud.h> to
     * calculate and set appropriate values for Baud rate.
     */
    UBRRL = UBRRL_VALUE;
    UBRRH = UBRRH_VALUE;
}

void usart_read_msg(midimsg_t *msg) {
    // get the first byte
    while (!(UCSRA & (1 << RXC))); // wait until receive is complete
    msg->byte1 = UDR; // store the byte
    _frame_err_check();

    // get the second byte
    while (!(UCSRA & (1 << RXC)));
    msg->byte2 = UDR;
    _frame_err_check();

    // get the third byte
    while (!(UCSRA & (1 << RXC)));
    msg->byte3 = UDR;
    _frame_err_check();
}

void usart_send_msg(midimsg_t *msg) {
    // send the first byte
    while (!(UCSRA & (1 << UDRE))); // wait until ready to transmit
    UDR = msg->byte1;

    // send the second byte
    while (!(UCSRA & (1 << UDRE))); // wait until ready to transmit
    UDR = msg->byte2;

    // send the third byte
    while (!(UCSRA & (1 << UDRE))); // wait until ready to transmit
    UDR = msg->byte3;
}

static void _frame_err_check(void) {
    if (UCSRA & (1 << FE)) {
        log_error(FRAME_ERR);
        exit(FRAME_ERR);
    }
}

int is_usart_ready(void) {
    return UCSRA & (1 << RXC);
}