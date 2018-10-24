#ifndef GME_USART_H_
#define GME_USART_H_

#include <gme_midimsg.h>
#define BAUD 31250
#define F_CPU 4000000

/**
 * Initializes necessary values for USART.
 */
void init_usart(void);

/**
 * Receives data for a MIDI message from USART, storing
 * the data inside the passed midi message type.
 */
void usart_read_msg(midimsg_t *msg);

/**
 * Sends a MIDI message out to USART.
 */
void usart_send_msg(midimsg_t *msg);

/**
 * Returns true if there's data in the input buffer for USART
 * to read.
 */
 int is_usart_ready(void);

#endif /* GME_USART_H_ */
