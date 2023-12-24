#include "serial.h"
#include <stdlib.h>
#include <util/delay.h>

void serial_init (unsigned short ubrr) {
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    /* Enable receiver and transmitter*/
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data, 1stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void serial_stop (void) {
    /* Disable receiver and transmitter*/
    UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
    /* Reset baud rate to default */
    unsigned short ubrr = (unsigned short) (CLOCK_FREQ / 16 / 115200 - 1);
    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr;

    /* Reset frame format to default */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0C &= ~((1 << USBS0) | (1 << UPM01) | (1 << UPM00));
}

void serial_write_char (char c) {
    while (( UCSR0A & (1 << UDRE0 )) == 0); // Wait until data register empty
    UDR0 = c; // Send character
}

char serial_read_char (void) {
    while (( UCSR0A & (1 << RXC0 )) == 0); // Wait until char received
    return UDR0; // Return character
}

void serial_write_string (char* str) {
    int i = 0;
    while (str[i] != '\0') {
        serial_write_char(str[i]);
        i++;
    }
}

char* serial_read_string (void) {
    char* str = (char*) malloc(100);
    int i = 0;
    char c = serial_read_char();
    while (c != '\n') {
        str[i] = c;
        i++;
        c = serial_read_char();
    }
    str[i] = '\0';
    return str;
}
