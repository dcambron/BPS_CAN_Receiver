
#pragma config OSC = HS     // Oscillator 
#pragma config PWRT = OFF   // Power on delay 
#pragma config WDT = ON    // WatchDog Timer
#pragma config LVP = OFF    // Low Voltage Programming OFF



////////////////////////////////////////////////////
//CAN message configuration
//CAN message identifiers are 11 bits long
#define	MASK_BPS_SLAVE	0b00011111100
#define	MASK_BPS_READING	0b00000000001
#define	MASK_CBS	0b00000000010
#define MASK_BPS_MASTER	0b01000000000

#define	BALANCEON	0xFF
#define	BANANCEOFF	0x00


#include <p18f4480.h>
#include "functions_Serial_JF.h"
#include "ECANPoll.h"
#include <usart.h>
#include <stdio.h>
//#include <string.h>

void transfer(void *output, void *input);

void main (void)
{
	unsigned long int i;
	unsigned int voltage;
	unsigned char temp;
	unsigned int received_CAN_ID;
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
  	CIOCONbits.ENDRHI =1;  																					    // defined ECANPoll.c
  	//CIOCONbits.CANCAP =1;  
	//RCONbits.IPEN = 0;
	//INTCONbits.GIE = 0;

	while(1)
	{
		messageReceived = 0;
		/*
		unsigned long int rxID = 0;
		unsigned char txDATA[7] = {0,0,0,0,0,0,0},rxDATA[7],rxFLAGS,rx,rxLEN;                     
        rx =  ECANReceiveMessage(&rxID, &rxDATA, &rxLEN, &rxFLAGS);
		printf("address = 0x%lx\n\rdata[0] = 0x%x\n\rdata[1] = 0x%x\n\rdata[2] = 0x%x\n\rlength = %d\n\r",
			rxID, rxDATA[0], rxDATA[1], rxDATA[2], rxLEN);//*/
		//LATCbits.LATC3 = ~LATCbits.LATC3;
		messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
      	//ECANReceiveMessage(&rxID, &rxDATA, &rxLEN, &rxFLAGS);
		
		//printf("address = 0x%x\n\rdata[0] = 0x%x\n\rdata[1] = 0x%x\n\rdata[2] = 0x%x\n\rdata[3] = 0x%x\n\rlength = %d\n\r",
		//		addressReceived, dataReceived[0], dataReceived[1], dataReceived[2], dataReceived[3], lengthReceived);
		//printf("%d\r\n", messageReceived);
		//while there are still messages in the buffer
//*
		//printf("Error = %d\r\n", ECANGetRxErrorCount());
		//printf("canid = 0x%lx\r\n", addressReceived);
		//printf("%d\r\n", messageReceived);
		while(messageReceived == 1)
		{
			messageReceived = 0;
			//if it has the reading bit set
			
			//printf("canid = 0x%lx\r\n", addressReceived);
			/*printf("data[0] = 0x%.2x\r\n", dataReceived[0]);
			printf("data[1] = 0x%.2x\r\n", dataReceived[1]);
			printf("data[2] = 0x%.2x\r\n", dataReceived[2]);//*/
			//printf("readingbitset = %d\n\r", (MASK_BPS_READING & addressRecieved) && MASK_BPS_READING);
			if((MASK_BPS_READING & addressReceived) && MASK_BPS_READING)
			{
				//just behave as if it is a slave and print out the information received
				received_CAN_ID = (addressReceived & MASK_BPS_SLAVE) >> 3;
				//memcpy(&voltage, dataReceived, 2);	//requires string.h
				transfer(&voltage, dataReceived);
				temp = dataReceived[2];
				//printf("CAN_ID = 0x%x\r\n", received_CAN_ID);
				printf("V[%.2d]=%u\n\r", received_CAN_ID, voltage);
				printf("T[%.2d]=%.2d\n\r", received_CAN_ID, temp);
			}
			
			//check for any more messages
			//for(i=0; i<1000; ++i);
			messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
			//printf("Error = %d\r\n", ECANGetRxErrorCount());
		}
		//for(i=0; i<1000; ++i); //max for unsigned long is 4,294,967,295//*/
	}

	return; //should never get here
}

//ripoff of memcpy so that I don't have to include the string library
void transfer(void *output, void *input)
{
	*(char *)output = *(char *)input;
	*(char *)((char *)output+1) = *(char *)((char *)input+1);
	return;
}