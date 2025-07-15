#include <LPC214x.H> 

#define clearDisplay      0x1
#define homeDC            0x2
#define incrementNoShift  0x6
#define cursorUB          0xF
#define operationMode     0x3C

// SPI chip select pin P0.15
#define CS_PIN (1 << 15)

// Joystick pins (active low)
#define JOY_UP    (1 << 20)
#define JOY_DOWN  (1 << 17)
#define JOY_LEFT  (1 << 18)
#define JOY_RIGHT (1 << 19)
#define JOY_BUTTON (1 << 16)

void delay(void) {
    unsigned int i,j;
    for(i=0;i<100;i++)
        for(j=0;j<100;j++);
}

void spi_init() {
    PINSEL0 = 0x1500; // Set P0.15,16,17 as SPI pins (datasheet)
    IO0DIR |= CS_PIN; // CS as output
    IO0SET |= CS_PIN; // CS high
    S0SPCR = (1 << 5) | (1 << 7); // SPI enable, master mode
    S0SPCCR = 8; // Clock rate
}

void spi_send(unsigned char data1, unsigned char data2) {
    IO0CLR = CS_PIN; // CS low
    S0SPDR = data1;
    while (!(S0SPSR & (1 << 7)));
    S0SPDR = data2;
    while (!(S0SPSR & (1 << 7)));
    IO0SET = CS_PIN; // CS high
}

void light_dot(unsigned char x, unsigned char y) {
    // Rows and cols are active low on matrix, so invert bit at position
    unsigned char row_data = ~(1 << y);
    unsigned char col_data = ~(1 << x);
    spi_send(row_data, col_data);
}

void enable()
{
    int i;
    for(i=0;i<1000;) { i++; }
    IOCLR1 |= 1<<25;
    for(i=0;i<1000;) { i++; }
    IOSET1 |= 1<<25;
}

void sendCommand(int command)
{
    IOCLR1 |= 1<<24;
    IOCLR1 |= 0xFF0000;
    IOSET1 |= command<<16;
    enable();
}

void sendData(int data)
{
    IOSET1 |= 1<<24;
    IOCLR1 |= 0xFF0000;
    IOSET1 |= data<<16;    
    enable();
}

void configGpios()
{
    IODIR0 |= 1<<30;
    IOSET0 = 1<<30;

    IODIR0 |= 1<<22;
    IOCLR0 |= 1<<22;

    IODIR1 |= 0x3FF0000; // LCD control/data pins

    IOSET1 |= 1<<25;

    // Configure joystick pins as input
    IO0DIR &= ~(JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT | JOY_BUTTON);
}

void sendString(char *text)
{
    int i=0;
    while(text[i]!='\0')
    {
        sendData(text[i]);
        i++;
    }
}

// Convert single digit number to ASCII
char numToChar(int num) {
    return (char)(num + '0');
}

int main(void)
{
    unsigned char x = 3, y = 3;

    spi_init();
    configGpios();

    // Init LCD
    sendCommand(operationMode);
    sendCommand(cursorUB);
    sendCommand(incrementNoShift);
    sendCommand(clearDisplay);
    sendCommand(homeDC);

    while(1)
    {
        // Read joystick (active low)
        if (!(IO0PIN & JOY_UP)) {
            if (y > 0) y--;
            delay();
        }
        if (!(IO0PIN & JOY_DOWN)) {
            if (y < 7) y++;
            delay();
        }
        if (!(IO0PIN & JOY_LEFT)) {
            if (x > 0) x--;
            delay();
        }
        if (!(IO0PIN & JOY_RIGHT)) {
            if (x < 7) x++;
            delay();
        }
        if (!(IO0PIN & JOY_BUTTON)) {
            x = 3;
            y = 3;
            delay();
        }

        // Update LED matrix dot
        light_dot(x, y);

        // Update LCD display carefully with delays
        sendCommand(clearDisplay);
        delay();

        sendCommand(homeDC);
        delay();

        sendString("X: ");
        sendData(numToChar(x));

        sendCommand(0xC0); // move to second line
        delay();

        sendString("Y: ");
        sendData(numToChar(y));

        delay();
    }
}
