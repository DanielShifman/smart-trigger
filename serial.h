#ifndef SMARTDISTANCE_SERIAL_H
#define SMARTDISTANCE_SERIAL_H

#include <avr/io.h>

#define CLOCK_FREQ 16000000 // 16MHz clock frequency

void serial_init(unsigned short baud);
void serial_stop(void);
void serial_write_char(char c);
char serial_read_char(void);
void serial_write_string(char* str);
char* serial_read_string(void);

#endif //SMARTDISTANCE_SERIAL_H
