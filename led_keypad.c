#include <stdio.h>
#include <unistd.h>
#include "library.h"


unsigned char i;


const unsigned char Bin2LED[] =
/*01000000,01111001,00100100,00110000 0-3 */
{ 0x40    ,0x79    ,0x24    ,0x30,
/*00011001,00010010,00000010,01111000 4-7 */
  0x19    ,0x12    ,0x02    ,0x78,
/*00000000,00011000,00001000,00000011 8-B */
  0       ,0x18    ,8       ,3   ,
/*01000110,00100001,00000110,00001110 C-F */
  0x46    ,0x21    ,6       , 0x0E
};

int main(int args,char* argv[])
{
   CM3DeviceInit();
   
   CM3PortInit(KbdPort);
     
   i = CM3PortRead(KbdPort);
   if(i != 0xff){
   
   CM3PortInit(LEDPort);
   
   CM3PortWrite(LEDPort,Bin2LED[i]);
   }
   
}
CM3DeviceDeInit();
