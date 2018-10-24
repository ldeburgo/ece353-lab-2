#include <gme_eeprom.h>
#include <gme_counter.h>
#include <gme_usart.h>
#include <gme_digital_io.h>

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// flags to detect when we've switched from one mode to another
uint8_t is_recording_flag;
uint8_t is_playback_flag;

// methods to handle different states the GME can be in
void handle_recording(MidiMsg *msg);
void handle_playback(MidiMsg *msg);
void handle_modification(uint16_t *msg);

// "event handlers" for when we switch to recording or playback
void start_recording(void);
void start_playback(void);

// convenience methods to reduce bulk
uint16_t get_time(void);

void init_all() {
    init_usart();
    init_counter_interrupts();
    init_eeprom();
    init_io();
    sei(); // enable global interrupts
}

int main (void) {
    // initialize any values on the board
    init_all();

    // will modify one stack-allocated midi message struct
    MidiMsg msg;

    // main program loop
    while (1) {
        // check if we're recording
        if (is_recording()) {
            handle_recording(&msg);
        } else {
            is_recording_flag = 0;
        }

        // check if we're playing back
        if (is_playback()) {
            handle_playback(&msg);
        } else {
            is_playback_flag = 0;
        }
    } // end main program loop
}

void handle_recording(MidiMsg *msg) {
    // if recording flag is zero, we've just switched
    if (!is_recording_flag) {
        is_recording_flag = 1;
        start_recording();
    }

    // check if there's data ready from USART
    if (is_usart_ready()) {
        // get the current time
        msg->time_elapsed = get_time();
        // reset the timer
        reset_timer();
        // get the data
        usart_read_msg(msg);
        // store the data
        eeprom_write_msg(msg);
    }
}

void handle_playback(MidiMsg *msg) {
    // if playback flag is zero, we've just switched
    if (!is_playback_flag) {
        is_playback_flag = 1;
        start_playback();
    }

    // local copy of time-elapsed to be modified
    uint16_t time_elapsed;
    if (is_last_msg()) {
        // if this is the last message played, manually add a delay
        // so first message doesn't play immediately
        // TODO see what this delay should be
        time_elapsed = 1000; // 1 second
    } else {
        // otherwise, just read the time from the message
        time_elapsed = msg->time_elapsed;
    }

    // read the message from memory
    eeprom_read_msg(msg);

    // check if we're modifying
    if (is_modifying()) {
        handle_modification(&time_elapsed);
    }

    // wait the appropriate amount of time
    _delay_ms(time_elapsed);

    // write the message out to USART
    usart_send_msg(msg);
}

void handle_modification(uint16_t *time_elapsed) {
    // get the adjustment rate
    uint16_t adc_val = read_adc();
    double adjust_rate = (double) adc_val / ADC_MAX;

    // adjust the time_elapsed
    *time_elapsed *= adjust_rate;
}

void start_recording(void) {
    // reset EEPROM writing address
    reset_write_addr();
}

void start_playback(void) {
    // reset EEPROM reading address
    reset_read_addr();
}

uint16_t get_time(void) {
    uint16_t time_elapsed;

    if (is_first_msg()) {
        time_elapsed = 0;
    } else {
        // if an overflow has occurred, it's been more than 4 seconds
        if (overflow_occurred()) {
            time_elapsed = 4000;
            reset_overflow();
        } else {
            time_elapsed = read_timer();
        }
    }

    return time_elapsed;
}