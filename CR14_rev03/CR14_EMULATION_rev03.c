/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 2516 $
* Date              : $Date: 2007-09-27 10:41:15 +0200 (to, 27 sep 2007) $
* Updated by        : $Author: mlarsson $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR311 - TWI Slave Implementation
*
* Description       : Example of how to use the driver for TWI slave 
*                     communication.
*
****************************************************************************/
/*! \page MISRA
 *
 * General disabling of MISRA rules:
 * * (MISRA C rule 1) compiler is configured to allow extensions
 * * (MISRA C rule 111) bit fields shall only be defined to be of type unsigned int or signed int
 * * (MISRA C rule 37) bitwise operations shall not be performed on signed integer types
 * As it does not work well with 8bit architecture and/or IAR

 * Other disabled MISRA rules
 * * (MISRA C rule 109) use of union - overlapping storage shall not be used
 * * (MISRA C rule 61) every non-empty case clause in a switch statement shall be terminated with a break statement
*/

#if defined(__ICCAVR__)
#include "ioavr.h"              
#include "inavr.h"
#else
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#endif
#include "CR14_EMULATION.h"
#include "TWI_slave.h"
#include <string.h>
#include <stdlib.h>

// Sample TWI transmission commands



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
  uint8_t tran[9];
  unsigned int num_pulse=1550;
  
	      	  
  srand(0);
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
				
			  if(messageBuf[2]==INITIATE)
				  {
				   
				   tran[0]=0x01;
				   tran[1]=(uint8_t) rand();
				  }
			  if(messageBuf[2]==0x0B)
			  {
				load_tran_buff(tran,Get_UID_reg);
				
			  }
			  if(messageBuf[2]==COMMAND)
			  {
				  switch(messageBuf[3])
				  {
				  	  case 0x05:
				  	  tran[0]=0x04;
				  	  tran[1] =0x0e;  //num_pulse & 0xFF;
				  	  tran[2] =0x06;  //(num_pulse >> 8) & 0xFF;
				  	  tran[3]=0x00;
				  	  tran[4]=0x00;
				  	  break;
				  	  case 0x06:
				  	  tran[0]=0x04;
				  	  tran[1] =0x0e;  //num_pulse & 0xFF;
				  	  tran[2] =0x06;  //(num_pulse >> 8) & 0xFF;
				  	  tran[3]=0x00;
				  	  tran[4]=0x00;
				  	  break;
						
					  case 0x07:
					      load_tran_buff(tran,read_reg07);
				  		  break;
				  	  case 0x08:
						  load_tran_buff(tran,read_reg08);
						  break;
				  	  case 0x09:
				  		  load_tran_buff(tran,read_reg09);
						  break;
				  	  case 0x0A:
				  		  load_tran_buff(tran,read_reg0A);
						  break;
				  	  case 0x0B:
						  load_tran_buff(tran,read_reg0B);
						  break;
					  case 0x0C:
						  load_tran_buff(tran,read_reg0C);
						  break;
					  case 0x0D:
						  load_tran_buff(tran,read_reg0D);
						  break;
					  case 0x0E:
						  load_tran_buff(tran,read_reg0E);
						  break;
					  case 0x0F:
						  load_tran_buff(tran,read_reg0F);
						  break;
					  
					   
					  

				  }
			  }
		  }


		  TWI_Start_Transceiver_With_Data(tran, tran[0]+1);
	  }
	  NOP();   // Do something else while waiting
  }
}
  
void load_tran_buff(uint8_t *tran_buffer_ptr,uint8_t *array)
{
	for(uint8_t i=0;i<array[0]+1;i++)
	{
		tran_buffer_ptr[i]=array[i];
	}
	
}





