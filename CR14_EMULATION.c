/*****************************************************************************
*
* Atmel Corporation
*
* File              : CR14_EMULATION.c
* Compiler          : 
* Revision          : $Revision:
* Date              : $Date: 
* Updated by        : $Author: 
*
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR311 - TWI Slave Implementation
*

****************************************************************************/

#if defined(__ICCAVR__)
#include "ioavr.h"              
#include "inavr.h"
#else
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#endif
#include "TWI_slave.h"
#include <string.h>
#include <stdlib.h>

// Sample TWI transmission commands

#define INITIATE 0x06
#define COMMAND 0x08

// The AVR can be waken up by a TWI address match from all sleep modes,
// But it only wakes up from other TWI interrupts when in idle mode.
// If POWER_MANAGEMENT_ENABLED is defined the device will enter power-down 
// mode when waiting for a new command and enter idle mode when waiting
// for TWI receives and transmits to finish.
#define POWER_MANAGEMENT_ENABLED

// Compiler-independent macros (was previously IAR intrinsics)
#if defined(__ICCAVR__)
#define SEI()     __enable_interrupt()
#define SLEEP()   __sleep()
#define NOP()     __no_operation()
#else
#define SEI()     sei()
#define SLEEP()   sleep_cpu()
#define NOP()     __asm__ __volatile__ ("nop" ::)
#endif

// When there has been an error, this function is run and takes care of it
//unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg );



#if defined(__ICCAVR__)
void
#else
int
#endif
main( void )
{
  unsigned char messageBuf[TWI_BUFFER_SIZE];
  unsigned char TWI_slaveAddress;
  unsigned char tran[9];
  unsigned int num_pulse=1038;
  
  // Own TWI slave address
  TWI_slaveAddress = 0x50;

  // Initialize TWI module for slave operation. Include address and/or enable General Call.
  TWI_Slave_Initialise( (unsigned char)((TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) )); 
                       
  SEI();

  // Start the TWI transceiver to enable reception of the first command from the TWI Master.
  TWI_Start_Transceiver();

  for(;;)
  {
	  if ( ! TWI_Transceiver_Busy() )
	  {
		  if ( TWI_statusReg.RxDataInBuf )
		  {
			  TWI_Get_Data_From_Transceiver(messageBuf, 5);
			  if(messageBuf[0]<1) memset(&messageBuf[0], 0, sizeof(messageBuf));

			  
			  if(messageBuf[2]==0x0B)
			  {
				tran[0]=0x08;
				tran[1]=0x6A;
				tran[2]=0xAD;
				tran[3]=0x82;
				tran[4]=0x24;
				tran[5]=0xE7;
				tran[6]=0x30;
				tran[7]=0x02;
				tran[8]=0xD0;
			  }
			  else if(messageBuf[2]==COMMAND)
			  {
				  switch(messageBuf[3])
				  {
				  	  case 0x07:
				  		  tran[0]=0x04;
				  		  tran[1]=0x0f;
				  		  tran[2]=0x19;
				  		  tran[3]=0x14;
				  		  tran[4]=0x14;
				  		  break;
				  	  case 0x08:
				  		  tran[0]=0x04;
				  		  tran[1]=0x14;
						  tran[2]=0x4B;
						  tran[3]=0x87;
						  tran[4]=0x00;
						  break;
				  	  case 0x09:
				  		  tran[0]=0x04;
				  		  tran[1]=0x66;
						  tran[2]=0x00;
						  tran[3]=0x62;
						  tran[4]=0x00;
						  break;
				  	  case 0x0A:
				  		  tran[0]=0x04;
				  		  tran[1]=0xA3;
						  tran[2]=0x02;
						  tran[3]=0xD0;
						  tran[4]=0x02;
						  break;
				  	  case 0x0B:
						  tran[0]=0x04;
						  tran[1]=0xfc;
						  tran[2]=0x02;
						  tran[3]=0x27;
						  tran[4]=0x03;
						  break;
					  case 0x0C:
						  tran[0]=0x04;
						  tran[1]=0x52;
						  tran[2]=0x03;
						  tran[3]=0x46;
						  tran[4]=0xB4;
						  break;
					  case 0x0D:
						  tran[0]=0x04;
						  tran[1]=0xdc;
						  tran[2]=0xc2;
						  tran[3]=0x85;
						  tran[4]=0x2a;
						  break;
					  case 0x0E:
						  tran[0]=0x04;
						  tran[1]=0xdc;
						  tran[2]=0xc2;
						  tran[3]=0x85;
						  tran[4]=0x2a;
						  break;
					  case 0x0F:
						  tran[0]=0x04;
						  tran[1]=0x64;
						  tran[2]=0x32;
						  tran[3]=0xe4;
						  tran[4]=0xf4;
						  break;
					  case 0x05:
						  tran[0]=0x04;
						  tran[1]=(uint8_t)(num_pulse) & 0xFF;
						  tran[2]=(uint8_t)(num_pulse >> 8) & 0xFF;
						  tran[3]=0x00;
						  tran[4]=0x00;
					break;				  
					  case 0x06:
						  tran[0]=0x04;
						  tran[1]=(uint8_t)(num_pulse) & 0xFF;
						  tran[2]=(uint8_t)(num_pulse >> 8) & 0xFF;
						  tran[3]=0x00;
						  tran[4]=0x00;

						  break;
				  
				  }
				  
			  }
			 else if(messageBuf[2]==0x06)
			  {
				  //randNumber=gen_rand();
				  tran[0]=0x01;
				  tran[1]=(uint8_t) rand();
			  }
		    memset(&messageBuf[0], 0, sizeof(messageBuf));
			TWI_Start_Transceiver_With_Data(&tran, tran[0]+1);
		  }
		  
		  
	  }
	  NOP();   // Do something else while waiting
  }
}

  





