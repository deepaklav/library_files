#include <stdio.h>
#include <unistd.h>

#include "library.h"



typedef struct {
	uint32_t funcsel[6];
	uint32_t rsvd;
	uint32_t pinoutset[2];
	uint32_t rsvd1;
	uint32_t pinoutclr[2];
	uint32_t rsvd2;
	uint32_t pinlevel[2];
	uint32_t rsvd3;
	uint32_t pinevtdet[2];
	uint32_t rsvd4;
	uint32_t pinrisedet[2];
	uint32_t rsvd5;
	uint32_t pinfalldet[2];
	uint32_t rsvd6;
	uint32_t highdeten[2];
	uint32_t rsvd7;
	uint32_t lowdeten[2];
	uint32_t rsvd8;
	uint32_t asyncrisedet[2];
	uint32_t rsvd9;
	uint32_t asyncfalldet[2];
	uint32_t rsvd10;
	uint32_t gppud;
	uint32_t gppudclk[2];
	uint32_t rsvd11;
}rpi_gpio_t;

// `short_wait` waits 150 cycles
static void short_wait(void)
{
	int i;
	for (i=0; i<150; i++) {
		asm volatile("nop");
	}
}
// Sets a pullup or -down resistor on a GPIO
static void set_pullupdn(int gpio, int pud)
{
	int clk_offset = OFFSET_PULLUPDNCLK + (gpio/32);
	int shift = (gpio%32);

	if (pud == PUD_DOWN)
		*(gpio_map+OFFSET_PULLUPDN) = (*(gpio_map+OFFSET_PULLUPDN) & ~3) | PUD_DOWN;
	else if (pud == PUD_UP)
		*(gpio_map+OFFSET_PULLUPDN) = (*(gpio_map+OFFSET_PULLUPDN) & ~3) | PUD_UP;
	else  // pud == PUD_OFF
		*(gpio_map+OFFSET_PULLUPDN) &= ~3;

	short_wait();
	*(gpio_map+clk_offset) = 1 << shift;
	short_wait();
	*(gpio_map+OFFSET_PULLUPDN) &= ~3;
	*(gpio_map+clk_offset) = 0;
}

// Sets a GPIO to either output or input (input can have an optional pullup
// or -down resistor).
	static void setup_gpio(int gpio, int direction, int pud)
{
	int offset = OFFSET_FSEL + (gpio/10);
	int shift = (gpio%10)*3;

	set_pullupdn(gpio, pud);
	if (direction == OUTPUT)
		*(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift)) | (1<<shift);
	else if (direction == ALTERNATE)
		*(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift)) | (4<<shift);
	else  // direction == INPUT
		*(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift));
}

// Returns the function of a GPIO: 0=input, 1=output, 4=alt0
// Contribution by Eric Ptak <trouch@trouch.com>
	static int gpio_function(int gpio)
{
	int offset = OFFSET_FSEL + (gpio/10);
	int shift = (gpio%10)*3;
	int value = *(gpio_map+offset);
	value >>= shift;
	value &= 7;
	return value;
}

// Sets a GPIO output to 1 or 0
	static void output_gpio(int gpio, int value)
{
	int offset;
	if (value) // value == HIGH
		offset = OFFSET_SET + (gpio / 32);
	else       // value == LOW
		offset = OFFSET_CLR + (gpio / 32);
	*(gpio_map+offset) = 1 << gpio % 32;
}

// Returns the value of a GPIO input (1 or 0)
	static int input_gpio(int gpio)
{
	int offset, value, mask;
	offset = OFFSET_PINLEVEL + (gpio/32);
	mask = (1 << gpio%32);
	value = *(gpio_map+offset) & mask;
	return value;
}


	static void cleanup(void)
{
	// fixme - set all gpios back to input
	munmap((void *)gpio_map, BLOCK_SIZE);

}
static void device_cycle_led(void)
{
	output_gpio(3,0);
	output_gpio(4,1);
	output_gpio(5,0);
	usleep(100000);
	
	output_gpio(3,1);
	output_gpio(4,1);
	output_gpio(5,1);
	usleep(100000);
}

static void write_7seg_setup(void)
{


	/* GPIO SETUP*/
	setup_gpio(3,1,1);
	setup_gpio(4,1,1);
	setup_gpio(5,1,1);
	setup_gpio(6,1,1);
	setup_gpio(7,1,1);
	setup_gpio(8,1,1);
	setup_gpio(9,1,1);
    setup_gpio(10,1,1);
	setup_gpio(11,1,1);
	setup_gpio(12,1,1);
	setup_gpio(13,1,1);
	setup_gpio(14,1,1);
	setup_gpio(15,1,1);
	usleep(100000);

	/* GPIO OUTPUT */
	output_gpio(0,1);
	output_gpio(1,1);
	output_gpio(2,1);
	output_gpio(3,0);
	output_gpio(4,0);
	output_gpio(5,1);
	output_gpio(6,1);
	output_gpio(7,0);

	usleep(100000);

}


static void write_7seg_led(uint8_t data)
{
	int i;
	for(i=0; i < 8; i++)
	{
		if(data & (1<<i))
		{

			output_gpio(i+8,0);

		}
		else
		{
			output_gpio(i+8,1);
		}
	}
}

static void device_cycle_lcd(void)
{
	output_gpio(3,0);
	output_gpio(4,1);
	output_gpio(5,1);
	sleep(1);
	
	output_gpio(3,1);
	output_gpio(4,1);
	output_gpio(5,1);
}

static void write_lcd_setup(void)
{


	/* GPIO SETUP*/

	for(int i=3;i<16;i++){
		setup_gpio(i,1,1);
	}
	sleep(1);

	/* GPIO OUTPUT */
	output_gpio(0,1);
	output_gpio(1,1);
	output_gpio(2,1);
	output_gpio(3,0);
	output_gpio(4,0);
	output_gpio(5,1);
	output_gpio(6,1);
	output_gpio(7,0);

	sleep(1);

}

static void pulse_enable(void)
{
	output_gpio(8,1);
	output_gpio(9,0);
	output_gpio(10,1);
	usleep(1);

	output_gpio(10,0);
}

static void cmd_pulse(void)
{
	output_gpio(8,0);   //rs
        output_gpio(9,0);   //r/w 
	output_gpio(10,1);  //enable bit
	usleep(1);
	output_gpio(10,0);
}
static void lcd_writedata(unsigned char dat)

{     
       
	unsigned char data;

	data = (dat & 0xf0);
	output_gpio(12,(data & 0x10));
	output_gpio(13,(data & 0x20));
	output_gpio(14,(data & 0x40));
	output_gpio(15,(data & 0x80));

	pulse_enable();
	usleep(200);

	data = ((dat<<4)&0xf0); 

	output_gpio(12,(data & 0x10));
	output_gpio(13,(data & 0x20));
	output_gpio(14,(data & 0x40));
	output_gpio(15,(data & 0x80));

	pulse_enable();
	usleep(2000);  
}

static void lcd_writecmd(char cmd)
{
  
	char data;

	data = (cmd & 0xf0);;

	output_gpio(12,(data & 0x10));
	output_gpio(13,(data & 0x20));
	output_gpio(14,(data & 0x40));
	output_gpio(15,(data & 0x80));

	cmd_pulse();
	usleep(200);

	data = ((cmd<<4) & 0xf0);


        output_gpio(12,(data & 0x10));
	output_gpio(13,(data & 0x20));
	output_gpio(14,(data & 0x40));
	output_gpio(15,(data & 0x80));

	cmd_pulse();
       usleep(2000);

}



void position_set_lcd(int row)
{

   char location;
   if(row==1)
   {
	   location = (0x80);
//	  printf("location 1 %d\n",location);
		  lcd_writecmd(location); 
   }
   else
   {
	   location =(0xc0);
	//  printf("location 2 %d\n",location);
	   lcd_writecmd(location);
   }


}

static void lcd_init(void)
{


//	lcd_writecmd(0x80);  //line 1
	lcd_writecmd(0x02);  // 4 bit mode 
	lcd_writecmd(0x28);  // 2 line  5*7 dots
	lcd_writecmd(0x01);  //clear screen
	lcd_writecmd(0x0c);  //dis on cur off
	lcd_writecmd(0x06);  //inc cur



}

 void clear_display(void)
{
   lcd_writecmd(0x01);
   sleep(1);


}
static void device_cycle_sm(void)
{
	output_gpio(3,0);
	output_gpio(4,0);
	output_gpio(5,1);
	sleep(1);
	
	output_gpio(3,1);
	output_gpio(4,1);
	output_gpio(5,1);
}
static void setup_pin_sm(void)
{
	for(int i=0;i<9;i++)
	{
		setup_gpio(i+3,1,1);
		      
	}
	usleep(50000);
	output_gpio(6,1);
	output_gpio(7,0);
	usleep(50000);
}
static device_cycle_keypad(void)
{
}
static void keypad_setup(void)
{
	/* GPIO SETUP*/
	setup_gpio(3,1,1);
	setup_gpio(4,1,1);
	setup_gpio(5,1,1);
	setup_gpio(6,1,1);
	setup_gpio(7,1,1);
	setup_gpio(8,1,1);
	setup_gpio(9,1,1);
    setup_gpio(10,1,1);
	setup_gpio(11,1,1);
	setup_gpio(12,0,1);
	setup_gpio(13,0,1);
	setup_gpio(14,0,1);
	setup_gpio(15,0,1);
	usleep(100000);

	/* GPIO OUTPUT */
	output_gpio(0,1);
	output_gpio(1,1);
	output_gpio(2,1);
	output_gpio(3,0);
	output_gpio(4,0);
	output_gpio(5,1);
	output_gpio(6,1);
	output_gpio(7,0);

	usleep(100000);
}
static void device_cycle_keypad(void)
{
}
static void write_enable_keypad(void)
{
  output_gpio(6,1);
  output_gpio(7,0);
  usleep(500000); 
}
static void read_enable_keypad(void)
{
  output_gpio(6,0);
  output_gpio(7,1);
  usleep(500000);
}
static void scanme(int x)
{
	output_gpio(x,1);
	usleep(500000);
	write_enable_keypad();
	usleep(250000);
	device_cycle_keypad();
	usleep(500000);
	read_enable_keypad();

}
static unsigned char scan_keypad(void)
{
	int i;
	printf("Testing_Keyapd_Row_1\n")
	scanme(8);
	for(i=0;i<15;i++)
	{
		usleep(100000);
		if(output_gpio(15)==0)
		{
			printf("1 is pressed\n");
			return 1;
		}
		if(output_gpio(14)==0)
		{
			printf("2 is pressed\n");
			return 2;
		}
		if(output_gpio(13)==0)
		{
			printf("3 is pressed\n");
			return 3;
		}
	}
	output_gpio(8,0);
	usleep(100000);
	
	printf("Testing_Keyapd_Row_2\n")
	scanme(9);
	for(i=0;i<15;i++)
	{
		usleep(100000);
		if(output_gpio(15)==0)
		{
			printf("4 is pressed\n");
			return 4;
		}
		if(output_gpio(14)==0)
		{
			printf("5 is pressed\n");
			return 5;
		}
		if(output_gpio(13)==0)
		{
			printf("6 is pressed\n");
			return 6;
		}
	}
	output_gpio(9,0);
	usleep(100000);
	
	printf("Testing_Keyapd_Row_3\n")
	scanme(10);
	for(i=0;i<15;i++)
	{
		usleep(100000);
		if(output_gpio(15)==0)
		{
			printf("7 is pressed\n");
			return 7;
	
		}
		if(output_gpio(14)==0)
		{
			printf("8 is pressed\n");
			return 8;
		}
		if(output_gpio(13)==0)
		{
			printf("9 is pressed\n");
			return 9;
		}
	}
	output_gpio(10,0);
	usleep(100000);
	
	printf("Testing_Keyapd_Row_4\n")
	scanme(11);
	for(i=0;i<15;i++)
	{
		usleep(100000);
		if(output_gpio(15)==0)
		{
			printf("* is pressed\n");
			return 10;
		}
		if(output_gpio(14)==0)
		{
			printf("0 is pressed\n");
			return 0;
		}
		if(output_gpio(13)==0)
		{
			printf("# is pressed\n");
			return 0x11;
		}
	}
	output_gpio(11,0);
	usleep(100000);	
	return 0xff;
}

static void spi_cycle(void)
{
}
static void dac_setup(void)
{
}
static void adc_setup(void)
{
}
static int read_mcp3001(void)
{																			
  output_gpio(18,true);
  usleep(100000);
  output_gpio(18,false);
  usleep(200000);
  
 
  }
void CM3DeviceInit(void)
{
	int fd;
	volatile rpi_gpio_t *gpiobase = NULL;
	uint8_t *gpiomem;

	fd = open("/dev/gpiomem", O_RDWR|O_SYNC);
	if(fd < 0) 
	{
		fprintf(stderr, "Unable to open /dev/mem\n");
		//goto err;
	}

	gpiobase = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(gpiobase == NULL) 
	{
		fprintf(stderr, "Unable to map GPIO memory /dev/mem\n");
		close(fd);
		//goto err;
	}
	gpio_map = (volatile uint32_t *)gpiobase;
}
int CM3DeviceDeInit(void)
{
    munmap((void *)gpiobase, 4096);
	close(fd);
	return 0;
    err:
	return -1;
}
void CM3PortInit(int port)
{ 
   if(port == 0)     //led
   {
    device_cycle_led();
	 write_7seg_setup();
   }
   if(port == 1)      //lcd
   {
    device_cycle_lcd();
	write_lcd_setup();
	lcd_init();
   }
   if(port == 3)      //steppermotor
   {
    	setup_pin_sm();
	    device_cycle_sm();
	    
   }
   if(port == 4)      //keypad
   {
	   keypad_setup();
	   
   }
   if(port == 5)      //dac
   {
       spi_cycle();
	   dac_setup();
       setup-gpio(17,1,1);
       output_gpio(17,true);
   }
   if(port == 6)
   {
       spi_cycle();
	   adc_setup();
       setup-gpio(18,1,1);
       output_gpio(18,true);
   
   }
}
void CM3PortWrite(int port,uint8_t value)
{
  if(port == 0)
  {
   write_7seg_led(value);      //led
  }
  if(port == 1)
  {
   lcd_writedata(value);      //lcd
  }
  if(port == 2)
  {
   output_gpio(17,value);     //dac
  }
  
}
void CM3PortRead(int port)
{
   if(port == 4)   //keypad
   {
     scan_keypad();
   }
}
void CM3WRITEPortArray(int arr[M][N])
{
   int i;
   for(i=0;i<4;i++)
   {
   		output_gpio(8,arr[i][0]);
		output_gpio(10,arr[i][1]);
		output_gpio(9,arr[i][2]);
		output_gpio(11,arr[i][3]);
  
   }
}
