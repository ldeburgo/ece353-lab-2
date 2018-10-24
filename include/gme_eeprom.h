#ifndef GME_EEPROM_H_
#define GME_EEPROM_H_

#include <stdint.h>
#include "gme_midimsg.h"

#define MAX_ADDR 1024

/**
 * Initializes necessary values for EEPROM.
 */
void init_eeprom(void);

/**
 * Writes a MIDI message to EEPROM.
 * Write format:
 * (lower address) time_elapsed_low, time_elapsed_high,
 * byte1, byte2, byte3 (higher address)
 *
 * Each MIDI message takes up 5 bytes.
 */
void eeprom_write_msg(MidiMsg *msg);

/**
 * Reads a MIDI message from EEPROM.
 * Reading is sequential, continues from the previous address.
 * Wraps upon meeting end of useful data.
 */
void eeprom_read_msg(MidiMsg *msg);

/**
 * Resets the current read address.
 * Used when playback re-initializes.
 */
void reset_read_addr(void);

/**
 * Resets the current write address.
 * Used when switching to recording.
 */
void reset_write_addr(void);

/**
 * True if this is the first message being written.
 */
int is_first_msg(void);

/**
 * True if this is the last message being read.
 */
int is_last_msg(void);

#endif /* GME_EEPROM_H_ */
