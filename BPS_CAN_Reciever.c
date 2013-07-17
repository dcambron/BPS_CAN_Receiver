
#pragma config OSC = HS     // Oscillator 
#pragma config PWRT = OFF   // Power on delay 
#pragma config WDT = ON    // WatchDog Timer
#pragma config LVP = OFF    // Low Voltage Programming OFF



////////////////////////////////////////////////////
//CAN message configuration
//CAN message identifiers are 11 bits long
#define	MASK_BPS_SLAVE	0b00011111100
#define	MASK_BPS_READING	0b00000000001
#define MASK_ENERGY         0b01000000000
#define	MASK_CBS	0b00000000010
#define MASK_BPS_MASTER	0b00100000000

#define	BALANCEON	0xFF
#define	BANANCEOFF	0x00


#include <p18f4480.h>
#include "functions_Serial_JF.h"
#include "ECANPoll.h"
#include <usart.h>
#include <stdio.h>
//#include <string.h>

void memcpy_reduced(void *output, void *input);

void main (void)
{
	unsigned long int i;
	unsigned int voltage;
	unsigned char temp;
	unsigned int received_CAN_ID;
	signed long  int current;
	//can receiving variables
	char messageReceived = 0;
	char dataReceived[8]={0,0,0,0,0,0,0,0};	//maximum length that can be recieved
	char lengthReceived = 0, flagsReceived = 0;
	unsigned long addressReceived = 0;
	
	//set all of the unused pins to output and have them output 0;
	TRISA = 0x00;
  	TRISB = 0x08;
	TRISC = 0x00;
	TRISD = 0x00;
	TRISE = 0x00;
	LATA = 0x00;
	LATB = 0x00;
	LATC = 0x00;
	LATD = 0x00;
	LATE = 0x00;
	
	//open the serial port
	openSerialPort();
	//setup the can port
	ECANInitialize();																							    // defined ECANPoll.c
  	CIOCONbits.ENDRHI =1;  

	while(1)
	{
		messageReceived = 0;
		//LATCbits.LATC3 = ~LATCbits.LATC3;
		messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);

		while(messageReceived == 1)
		{
			messageReceived = 0;
			//if it has the reading bit set

			//printf("CAN_ID = 0x%.3lx\r\n", addressReceived);
			
			if(((MASK_BPS_READING | MASK_BPS_SLAVE) | addressReceived) == (MASK_BPS_READING | MASK_BPS_SLAVE))
			{
				//just behave as if it is a slave and print out the information received
				received_CAN_ID = (addressReceived & MASK_BPS_SLAVE) >> 2;
				//memcpy(&voltage, dataReceived, 2);	//requires string.h
				memcpy_reduced(&voltage, dataReceived);
				temp = dataReceived[2];
				printf("V[%.2d]=%u\n\r", received_CAN_ID, voltage);
				printf("T[%.2d]=%.2d\n\r", received_CAN_ID, temp);
			}
			else if(((MASK_BPS_MASTER | MASK_BPS_READING) == addressReceived)) //if current reading
			{
				if(lengthReceived == 4)
				{
					int i; int d;
					unsigned char *test ;
					signed long * current_u;
				//	signed int *stupid;
					test = (void*) dataReceived;
				//	memcpy_reduced(&current, dataReceived);

					current_u = (void*)dataReceived;
				//for(i = 0; i < 4; i++) printf("[%d]  --  %x  --- %x\n\r",i,test[i], dataReceived[i]);
				printf("BC=%ld\n\r",*current_u);
				}
			}
			else if(((MASK_BPS_MASTER | MASK_BPS_READING | MASK_ENERGY) == addressReceived)) //if energy reading
			{
				if(lengthReceived == 4)
				{
					long *energy = (void*)dataReceived;
					printf("E=%ld\n\r",*energy);
				}
			}
			else if(((MASK_BPS_MASTER | MASK_BPS_READING | MASK_CBS) == addressReceived))	//if shutdown message or cbs reading
			{
				
				if(lengthReceived == 4)
				{
					printf("Shutting Car Down\n\r");
					memcpy_reduced(&voltage, &(dataReceived[1]));
					temp = dataReceived[3];
					printf("Addr = %.2x\n\r", dataReceived[0]);
					printf("Volt = %u\n\r", voltage);
					printf("temp = %.2d\n\r", temp);
				}
				if(lengthReceived == 1)
				{
					unsigned char * module;
					module = (void*) dataReceived;
					printf("CBS=%d\n\r",*module);

				}
			}
			
			//check for any more messages
			//for(i=0; i<1000; ++i);
			messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
		}
		//for(i=0; i<1000; ++i); //max for unsigned long is 4,294,967,295//*/
	}

	return; //should never get here
}

//ripoff of memcpy so that I don't have to include the string library
void memcpy_reduced(void *output, void *input)
{
	*(char *)output = *(char *)input;
	*(char *)((char *)output+1) = *(char *)((char *)input+1);
	return;
}