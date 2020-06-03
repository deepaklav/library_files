#ifndef __CM3_LIBRARY_H__
#define __CM3_LIBRARY_H__
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#define GPIOBASE_ADDR 0x7E200000
#define OFFSET_FSEL         0   // 0x0000
#define OFFSET_SET          7   // 0x001c / 4
#define OFFSET_CLR          10  // 0x0028 / 4
#define OFFSET_PINLEVEL     13  // 0x0034 / 4
#define OFFSET_PULLUPDN     37  // 0x0094 / 4
#define OFFSET_PULLUPDNCLK  38  // 0x0098 / 4

#define OUTPUT 		1
#define ALTERNATE	2
#define PUD_UP		1
#define PUD_DOWN 	2

#define BLOCK_SIZE 4096
#define PAGE_SIZE 4096
#define LEDPort     0        /* LED Port  */
#define KbdPort     4        /* Key pad port */
#define LCDPort     1        /* Lcd Port */


static volatile uint32_t *gpio_map;

static void short_wait(void);

static void set_pullupdn(int gpio, int pud);

static void setup_gpio(int gpio, int direction, int pud);

static int gpio_function(int gpio);

static void output_gpio(int gpio, int value);

static int input_gpio(int gpio);

static void cleanup(void);

static void device_cycle_led(void);

static void write_7seg_setup(void);

static void write_7seg_led(uint8_t data);

static void device_cycle_lcd(void);

static void write_lcd_setup(void);

static void pulse_enable(void);

static void cmd_pulse(void);

static void lcd_writedata(unsigned char dat);

static void lcd_writecmd(char cmd);

void position_set_lcd(int row);

static void lcd_init(void);

void clear_display(void);

static void device_cycle_sm(void);

static void setup_pin_sm(void);

static void device_cycle_keypad(void);

static void write_enable_keypad(void);

static void read_enable_keypad(void);

static void keypad_setup(void);

static void scanme(int x);

static unsigned char scan_keypad(void);

static void spi_cycle(void);

static void dac_setup(void);

static void adc_setup(void);

static int read_mcp3001(void);

void CM3PortRead(int port);

void CM3DeviceInit(void);

int CM3DeviceDeInit(void);

void CM3PortInit(int port);

void CM3PortWrite(int port,uint8_t value);

void CM3WRITEPortArray(int arr[M][N]);
#endif
