#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdio.h>

#define ECHO_PIN PINB0 // Arduino pin 8,   ATmega328P Port B pin 0
#define TRIG_PIN PORTB1 // Arduino pin 9,   ATmega328P Port B pin 1

#define DETECT_LED_PIN PORTB2 // Arduino pin 10,  ATmega328P Port B pin 2
#define ECHO_LED_PIN  PORTB3 // Arduino pin 11,  ATmega328P Port B pin 3

#define POT_PIN PINC5 // Arduino pin A5,  ATmega328P Port C pin 5

#define MIC_A_PIN PINC4 // Arduino pin A4,  ATmega328P Port C pin 4
#define MIC_D_PIN PORTD3 // Arduino pin 3,   ATmega328P Port D pin 3

#define CLOCK_FREQ 16000000 // 16MHz clock frequency
#define V_SOUND 0.0343 // Speed of sound in cm/us
#define MAX_TIME1 65535 // Max value of TCCR1B register
#define ABSENT_TIME 500 // Time to wait before declaring object absent
#define PRESENT_BASE 200 // Number of times object must be detected sequentially to declare it present

int s_timer;
int p_count;
int presentCount = 0;
_Bool present = 0;
_Bool armed = 0;

void trigger() {
    // Blink LED to indicate trigger
    for (int i = 0; i < 10; i++) {
        _delay_ms(100);
        PORTB |= _BV(DETECT_LED_PIN);
        _delay_ms(100);
        PORTB &= ~_BV(DETECT_LED_PIN);
    }

}

void departAction() {
    PORTB &= ~_BV(DETECT_LED_PIN); // Turn off LED to indicate lack of object presence
    armed = 0;
}

void arriveAction() {
    PORTB |= _BV(DETECT_LED_PIN); // Turn on LED to indicate object presence
    armed = 1;
}

ISR(TIMER0_OVF_vect) {
    s_timer++;
    if (s_timer >= ABSENT_TIME) {
        if (present && (p_count-- < (presentCount * 0.01))) {
            departAction();
            present = 0;
        }
        s_timer = 0;
    }
    TCNT0 = 0; // Reset Timer
}

int main (void) {
    sei(); // Enable global interrupts
    /* Set output pins */
    DDRB |= _BV(DDB1); // Trigger pin
    DDRB |= _BV(DDB2); // Detect LED pin
    DDRB |= _BV(DDB3); // Echo LED pin
    /* Set input pins */
    DDRB &= ~_BV(ECHO_PIN); // Echo pin
    DDRC &= ~_BV(POT_PIN); // Potentiometer pin
    DDRD &= ~_BV(MIC_D_PIN); // Microphone digital pin)
    /* Set pullup on input pins */
    PORTB |= _BV(PORTB0); // Echo pin
    /* Set down on input pins */
    PORTD &= ~_BV(PORTD3); // Microphone digital pin

    /* Analog Setup */
    // Configure ADC to be left justified, use AVCC as reference, and select ADC0 as ADC input
    ADMUX |= _BV(ADLAR) | _BV(REFS0);
    ADMUX &= ~_BV(REFS1) & ~_BV(MUX3) & ~_BV(MUX2) & ~_BV(MUX1) & ~_BV(MUX0);
    // Enable ADC and set prescaler to 128
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRA &= ~_BV(ADATE) & ~_BV(ADIE) & ~_BV(ADSC);

    /* Prepare absent timer */
    TCNT0 = 0; // Reset Timer
    TCCR0B |= _BV(CS00) | _BV(CS02); // Start timer with 1024 prescaling
    TCCR0B &= ~_BV(CS01);
    TIMSK0 |= _BV(TOIE0); // Enable overflow interrupt
    s_timer = 0; // Start timer
    p_count = 0;

    while(1) {
        /* Check if microphone is triggered whilst armed*/
        if (PIND & _BV(MIC_D_PIN) && armed) {
            trigger();
        }
        /* Read potentiometer value */
        ADMUX |= _BV(MUX2) | _BV(MUX0); // Select ADC5 as ADC input
        ADMUX &= ~_BV(MUX3) & ~_BV(MUX1);
        ADCSRA |= _BV(ADSC); // Start conversion
        while(ADCSRA & _BV(ADSC)); // Wait for conversion to finish
        int presentCountMultiplier = ADCH; // Read ADC value
        /* Normalize ADC value to be between 0 and 1 */
        double presentCountMultiplierNormalized = (double)presentCountMultiplier / 255.0;
        /* Set present count */
        presentCount = (int)(PRESENT_BASE * presentCountMultiplierNormalized);
        /* Send a 10us pulse on the trigger pin */
        PORTB |= _BV(TRIG_PIN);
        _delay_us(10);
        PORTB &= ~_BV(TRIG_PIN);
        /* Wait for the echo pin to go high, then start timer */
        while(!(PINB & _BV(ECHO_PIN)));
        TCNT1 = 0; // Reset Timer
        TCCR1B |= _BV(CS10); // Start timer with no prescaling
        TCCR1B &= ~_BV(CS11) & ~_BV(CS12);
        /* Wait for the echo pin to go low or max out the timer */
        while(PINB & _BV(ECHO_PIN) && TCNT1 < MAX_TIME1);
        if (TCNT1 >= MAX_TIME1) {
            PORTB &= ~_BV(ECHO_LED_PIN);
            TCNT0 = 0; // Reset absence timer
            TCCR0B |= _BV(CS00) | _BV(CS02); // Start timer with 1024 prescaling
            TCCR0B &= ~_BV(CS01);
            TIMSK0 |= _BV(TOIE0); // Enable overflow interrupt
            p_count = 0;
        } else {
            TCCR1B &= ~_BV(CS10);
            /* Calculate distance in cm */
            double time = (double)TCNT1 / (double)CLOCK_FREQ; // Time in microseconds
            int distance = (int)(time / 2 * V_SOUND * 1e6); // Distance in cm
            /* Turn on LED if distance is between 40 and 120 cm */
            if(distance < 120 && distance > 40) {
                PORTB |= _BV(ECHO_LED_PIN);
                if (!present && p_count++ >= presentCount) {
                    arriveAction();
                    present = 1;
                    s_timer = 0;
                }
            } else {
                PORTB &= ~_BV(ECHO_LED_PIN);
                p_count = 0;
            }
        }
        /* Wait for 10ms before sending another pulse */
        _delay_ms(10);
    }
}