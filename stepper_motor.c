#include <stdio.h>
#include <unistd.h>
#include "library.h"

const int M = 4;
const int N = 4;
const int full_seq_drive[M][N] = { 
                   
				             {1,1,0,0},
                                             {0,1,1,0},
		                             {0,0,1,1},
		                             {1,0,0,1},
		                         };
								 
int main(int args, char *argv[])
{

      CM3DeviceInit();
	  CM3PortInit(3);
	  

printf("starting_stepper_motor\n");

while(1)
{ 
           
		   
		   CM3WRITEPortArray(full_seq_drive);
 		
			usleep(1000);              //non blocking sleep

}
                      
}
 
CM3DeviceDeInit();

}  
