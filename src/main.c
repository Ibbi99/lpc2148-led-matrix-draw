#include <LPC214x.H>

#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_ENTRY_INC   0x06
#define LCD_ON_CURSOR   0x0F
#define LCD_8BIT_2LINE  0x3C
#define LCD_LINE1       0x80
#define LCD_LINE2       0xC0

// SPI CS on P0.15 (GPIO)
#define CS_PIN          (1U << 15)

// Joystick (active low)
#define JOY_UP          (1U << 20)
#define JOY_DOWN        (1U << 17)
#define JOY_LEFT        (1U << 18)
#define JOY_RIGHT       (1U << 19)
#define JOY_BUTTON      (1U << 16)

// LCD on Port 1: RS=P1.24, E=P1.25, D0..D7=P1.16..P1.23
#define LCD_RS          (1U << 24)
#define LCD_E           (1U << 25)
#define LCD_DATA_SHIFT  16
#define LCD_DATA_MASK   (0xFFU << LCD_DATA_SHIFT)

static void delay_short(void) {
    volatile unsigned int i, j;
    for (i = 0; i < 100; i++) for (j = 0; j < 100; j++) { }
}

static void delay_pulse(void) {
    volatile unsigned int i;
    for (i = 0; i < 1000; i++) { }
}

static void spi_init(void) {
    // PINSEL0 = 0x1500 sets P0.4..P0.6 to SPI0 (SCK0, MISO0, MOSI0)
    PINSEL0 = 0x1500;

    IO0DIR |= CS_PIN;
    IO0SET  = CS_PIN;

    S0SPCR  = (1U << 5) | (1U << 7); // master + enable
    S0SPCCR = 8;                     // clock divider
}

static void spi_send(unsigned char a, unsigned char b) {
    IO0CLR = CS_PIN;

    S0SPDR = a;
    while (!(S0SPSR & (1U << 7))) { }

    S0SPDR = b;
    while (!(S0SPSR & (1U << 7))) { }

    IO0SET = CS_PIN;
}

static void light_dot(unsigned char x, unsigned char y) {
    unsigned char row = (unsigned char)~(1U << y);
    unsigned char col = (unsigned char)~(1U << x);
    spi_send(row, col);
}

static void lcd_pulse(void) {
    delay_pulse();
    IOCLR1 = LCD_E;
    delay_pulse();
    IOSET1 = LCD_E;
}

static void lcd_cmd(unsigned char cmd) {
    IOCLR1 = LCD_RS;
    IOCLR1 = LCD_DATA_MASK;
    IOSET1 = ((unsigned int)cmd << LCD_DATA_SHIFT);
    lcd_pulse();
}

static void lcd_data(unsigned char data) {
    IOSET1 = LCD_RS;
    IOCLR1 = LCD_DATA_MASK;
    IOSET1 = ((unsigned int)data << LCD_DATA_SHIFT);
    lcd_pulse();
}

static void lcd_puts(const char *s) {
    while (*s) lcd_data((unsigned char)*s++);
}

static char digit(unsigned char v) {
    return (char)('0' + (v % 10));
}

static void gpio_init(void) {
    IODIR0 |= (1U << 30);
    IOSET0  = (1U << 30);

    IODIR0 |= (1U << 22);
    IOCLR0  = (1U << 22);

    IODIR1 |= (LCD_DATA_MASK | LCD_RS | LCD_E);
    IOSET1  = LCD_E;

    IO0DIR &= ~(JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT | JOY_BUTTON);
}

static void lcd_init(void) {
    lcd_cmd(LCD_8BIT_2LINE);
    lcd_cmd(LCD_ON_CURSOR);
    lcd_cmd(LCD_ENTRY_INC);
    lcd_cmd(LCD_CLEAR);
    lcd_cmd(LCD_HOME);
}

static void lcd_show_xy(unsigned char x, unsigned char y) {
    lcd_cmd(LCD_LINE1);
    lcd_puts("X: ");
    lcd_data((unsigned char)digit(x));
    lcd_puts("   ");

    lcd_cmd(LCD_LINE2);
    lcd_puts("Y: ");
    lcd_data((unsigned char)digit(y));
    lcd_puts("   ");
}

int main(void) {
    unsigned char x = 3, y = 3;

    spi_init();
    gpio_init();
    lcd_init();

    while (1) {
        if (!(IO0PIN & JOY_UP))    { if (y > 0) y--; delay_short(); }
        if (!(IO0PIN & JOY_DOWN))  { if (y < 7) y++; delay_short(); }
        if (!(IO0PIN & JOY_LEFT))  { if (x > 0) x--; delay_short(); }
        if (!(IO0PIN & JOY_RIGHT)) { if (x < 7) x++; delay_short(); }
        if (!(IO0PIN & JOY_BUTTON)){ x = 3; y = 3; delay_short(); }

        light_dot(x, y);
        lcd_show_xy(x, y);

        delay_short();
    }
}
