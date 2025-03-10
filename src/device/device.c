#include <util/delay.h>

// PORTB
#define DDRB     *((volatile char*) 0x24)
#define PORTB    *((volatile char*) 0x25)

// PORTD
#define DDRD     *((volatile char*) 0x2A)
#define PORTD    *((volatile char*) 0x2B)
#define PIND     *((volatile char*) 0x29)
#define PIND7 PIND >> 7

#define F_CPU 16000000UL  // Define the CPU frequency (16 MHz for most ATMEGA328p)
#define BAUD 2400

// UART registers
#define UBRR0H  *((volatile char*) 0xC5)
#define UBRR0L  *((volatile char*) 0xC4)

#define TXEN0   3
#define RXEN0   4

#define UCSZ00  1
#define UCSZ01  2

#define UCSR0A  *((volatile char*) 0xC0)
#define UDRE0   5

#define UCSR0B  *((volatile char*) 0xC1)
#define UCSR0C  *((volatile char*) 0xC2)

/* UART0 I/O Data Register */
#define UDR0    *((volatile char*) 0xC6)

const unsigned long UBRR = (F_CPU / 16 / BAUD) - 1;

void uart_init(void) {
    // Set baud rate
    UBRR0H = UBRR >> 8;
    UBRR0L = UBRR;

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send(char data) {
    // Wait for the transmit buffer to be empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Send the data
    UDR0 = data;
}

int main(){
    uart_init();

    DDRB = 0b00100000;
    DDRD = 0x00;
    PORTD = 0xFF;

    // Wait to Serial port to be ready
    _delay_ms(2000);

    while (1)
    {
        char state;
            if(PIND7){
                state = '1';
                PORTB = 0b00100000;
            }else{
                state = '0';
                PORTB = 0x00;
            }
        _delay_ms(100);
        uart_send(state);
        uart_send('\0');
    }

    return 0;
}
