#include <stdio.h>
#include <unistd.h>
#include "library.h"

unsigned char i;


const unsigned char Bin2LCD[] ={"123456789*0#"};

int main(int args,char* argv[])
{
     CM3DeviceInit();
while(1)
 {	 
	CM3PortInit(KbdPort);
    i = CM3PortRead(KbdPort);
	 
    if(i!= 0xff)
    {
    CM3PortInit(LCDPort);
   	printf("lcd display on\n");

    position_set_lcd(1);    //positon

    CM3PortWrite(LCDPort,Bin2LCD[i]);	
   
    }
 } 
}
    CM3DeviceDeInit();
